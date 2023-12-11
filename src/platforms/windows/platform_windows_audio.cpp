#ifdef _WIN32

#include "platforms/platform_windows.hpp"

#include <mmdeviceapi.h>
#include <functiondiscoverykeys.h>
#include <audioclient.h>
#include <stdio.h>

using namespace Platform::Audio;

static void freeInternalResources();
static void freeDeviceList(DeviceInfo*& array, u32& length);

static IMMDeviceCollection* s_deviceCollection {};

static u32 s_inputDeviceInfoCount {};
static u32 s_outputDeviceInfoCount {};
static DeviceInfo* s_inputDeviceInfo {};
static DeviceInfo* s_outputDeviceInfo {};

struct InputDeviceInstance {
    bool isValid {};
    InputDeviceInstanceInfo info {};
    IAudioClient* client {};
    IAudioCaptureClient* captureClient {};
};
static const u32 MAX_INPUT_DEVICES {10};
static InputDeviceInstance s_inputDevices[MAX_INPUT_DEVICES] {};

void Platform::Audio::refreshDeviceLists() {
    // see this for info on what's happening here:
    // https://learn.microsoft.com/en-us/windows/win32/api/mmdeviceapi/

    freeInternalResources();

    // Request device info from win32 COM objects
    {
        IMMDeviceEnumerator* deviceEnumerator {};
        CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
        deviceEnumerator->EnumAudioEndpoints(EDataFlow::eAll, DEVICE_STATEMASK_ALL, &s_deviceCollection);
        deviceEnumerator->Release();
    }

    // Allocate new device info
    u32 deviceCount;
    s_deviceCollection->GetCount(&deviceCount);

    for (int i = 0; i < deviceCount; ++i) {
        EDataFlow dataFlow;

        // Query the dataflow (is this device an input or output device?)
        {
            IMMDevice* device {};
            s_deviceCollection->Item(i, &device);
            IMMEndpoint* endpoint;
            device->QueryInterface(IID_PPV_ARGS(&endpoint));
            endpoint->GetDataFlow(&dataFlow);
            endpoint->Release();
            device->Release();
        }

        switch (dataFlow) {
            case EDataFlow::eCapture: {
                s_inputDeviceInfoCount++;
                break;
            }
            case EDataFlow::eRender: {
                s_outputDeviceInfoCount++;
                break;
            }
            default: { /* Do nothing. */ }
        }
    }

    s_inputDeviceInfo = new Audio::DeviceInfo[s_inputDeviceInfoCount];
    s_outputDeviceInfo = new Audio::DeviceInfo[s_outputDeviceInfoCount];
    u32 inputIndex {};
    u32 outputIndex {};

    for (int i = 0; i < deviceCount; ++i) {

        IMMDevice* device {};
        s_deviceCollection->Item(i, &device);

        IPropertyStore* properties {};
        device->OpenPropertyStore(STGM_READ, &properties);

        Audio::DeviceInfo* info;

        // determine if *info should point into the s_inputDevice or s_outputDevice array
        {
            IMMEndpoint* endpoint;
            EDataFlow dataFlow;
            device->QueryInterface(IID_PPV_ARGS(&endpoint));
            endpoint->GetDataFlow(&dataFlow);
            endpoint->Release();
            switch (dataFlow) {
                case EDataFlow::eCapture: {
                    info = &s_inputDeviceInfo[inputIndex];
                    inputIndex++;
                    break;
                }
                case EDataFlow::eRender: {
                    info = &s_outputDeviceInfo[outputIndex];
                    outputIndex++;
                }
                default: { /* Do nothing. */ }
            }
        }

        info->index = i;

        // set info->state
        {
            DWORD state;
            device->GetState(&state);
            switch (state) {
                case DEVICE_STATE_ACTIVE: {
                    info->state = DeviceState::Active;
                    break;
                }
                case DEVICE_STATE_DISABLED: {
                    info->state = DeviceState::Disabled;
                    break;
                }
                case DEVICE_STATE_NOTPRESENT: {
                    info->state = DeviceState::NotPresent;
                    break;
                }
                case DEVICE_STATE_UNPLUGGED: {
                    info->state = DeviceState::Unplugged;
                    break;
                }
                default: {
                    break;
                }
            }
        }

        // set info->deviceName
        {
            PROPVARIANT name;
            PropVariantInit(&name);
            properties->GetValue(PKEY_Device_FriendlyName, &name);
            info->deviceName = wcharToChar(name.pwszVal);
            PropVariantClear(&name);
        }

        properties->Release();
        device->Release();
    }
}

u32 Platform::Audio::getInputDeviceCount() {
    return s_inputDeviceInfoCount;
}

const DeviceInfo* Platform::Audio::getInputDeviceInfo(u32 index) {
    return &s_inputDeviceInfo[index];
}

InputDeviceHandle Platform::Audio::createInputDeviceInstance(u32 index) {
    IMMDevice* device {};
    s_deviceCollection->Item(index, &device);

    InputDeviceInstance* instance {};
    InputDeviceHandle handle {};

    // Search for an open slot in s_inputDevices
    for (int i = 0; i < MAX_INPUT_DEVICES; ++i) {
        if (!s_inputDevices[i].isValid) {
            instance = &s_inputDevices[i];
            handle = i;
            break;
        }
    }

    if (instance == nullptr) {
        printf("Failed to create new device: only %u can be allocated!", MAX_INPUT_DEVICES);
        return -1;
    }

    InputDeviceInstanceInfo& info = instance->info;
    instance->isValid = true;

    device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&instance->client));
    device->Release();

    WAVEFORMATEX* waveFormat;
    instance->client->GetMixFormat(&waveFormat);
    instance->client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, waveFormat, nullptr);

    // record info about the wave format
    {
        info.formatInfo.tag = waveFormat->wFormatTag;
        info.formatInfo.extraSize = waveFormat->cbSize;
        info.formatInfo.bytesPerSec = waveFormat->nAvgBytesPerSec;
        info.formatInfo.blockAlign = waveFormat->nBlockAlign;
        info.formatInfo.channels = waveFormat->nChannels;
        info.formatInfo.samplesPerSec = waveFormat->nSamplesPerSec;
        info.formatInfo.bitsPerSample = waveFormat->wBitsPerSample;
    }

    CoTaskMemFree(waveFormat);

    instance->client->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(&instance->captureClient));

    // record info about the client
    {
        instance->client->GetBufferSize(&info.bufferSize);
        instance->client->GetCurrentPadding(&info.padding);

        REFERENCE_TIME period {};
        instance->client->GetDevicePeriod(nullptr, &period);
        info.period = period;

        REFERENCE_TIME latency {};
        instance->client->GetStreamLatency(&latency);
        info.latency = latency;
    }

    return handle;
}

InputDeviceInstanceInfo* Platform::Audio::getInputDeviceInstanceInfo(InputDeviceHandle handle) {
    return &s_inputDevices[handle].info;
}

void Platform::Audio::freeInputDeviceInstance(InputDeviceHandle device) {
    InputDeviceInstance& instance = s_inputDevices[device];
    instance.client->Release();
    instance.captureClient->Release();
    ZeroMemory(&instance, sizeof(instance));
    instance.isValid = false;
}

void audioInit() {
    Platform::Audio::refreshDeviceLists();
}

void audioFree() {
    freeInternalResources();
}

static void freeInternalResources() {
    freeDeviceList(s_inputDeviceInfo, s_inputDeviceInfoCount);
    freeDeviceList(s_outputDeviceInfo, s_outputDeviceInfoCount);

    if (s_deviceCollection != nullptr) {
        s_deviceCollection->Release();
        s_deviceCollection = nullptr;
    }
}

static void freeDeviceList(DeviceInfo*& array, u32& length) {
    if (array == nullptr) {
        return;
    }
    for (int i = 0; i < length; ++i) {
        delete array[i].deviceName;
    }
    delete[] array;
    array = nullptr;
    length = 0;
}

#endif // _WIN32