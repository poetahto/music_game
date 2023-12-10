#ifdef _WIN32

#include "platforms/platform_windows.hpp"

#include <mmdeviceapi.h>
#include <functiondiscoverykeys.h>
#include <audioclient.h>

using namespace Platform::Audio;

static void freeDeviceList(DeviceInfo*& array, u32& length);

static IMMDeviceCollection* s_deviceCollection {};

static u32 s_inputDeviceInfoCount {};
static DeviceInfo* s_inputDeviceInfo {};
static u32 s_outputDeviceInfoCount {};
static DeviceInfo* s_outputDeviceInfo {};
static const u32 MAX_INPUT_DEVICES {10};
static IAudioClient* s_inputAudioClients[MAX_INPUT_DEVICES] {};
static IAudioCaptureClient* s_audioCaptureClients[MAX_INPUT_DEVICES] {};

void Platform::Audio::refreshDeviceLists() {
    // see this for info on what's happening here:
    // https://learn.microsoft.com/en-us/windows/win32/api/mmdeviceapi/

    freeDeviceList(s_inputDeviceInfo, s_inputDeviceInfoCount);
    freeDeviceList(s_outputDeviceInfo, s_outputDeviceInfoCount);

    if (s_deviceCollection != nullptr) {
        s_deviceCollection->Release();
    }

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
        IMMDevice* device {};
        s_deviceCollection->Item(i, &device);
        IMMEndpoint* endpoint;
        device->QueryInterface(IID_PPV_ARGS(&endpoint));
        EDataFlow dataFlow;
        endpoint->GetDataFlow(&dataFlow);
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
        endpoint->Release();
        device->Release();
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

        // check if this device is an input or output device
        {
            IMMEndpoint* endpoint;
            EDataFlow dataFlow;
            device->QueryInterface(IID_PPV_ARGS(&endpoint));
            endpoint->GetDataFlow(&dataFlow);
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
            endpoint->Release();
        }

        info->index = i;

        // check this devices state (active, disabled, ect.)
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

        // check this device's friendly name
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

InputDeviceHandle Platform::Audio::createInputDevice(u32 index) {
    IMMDevice* device {};
    s_deviceCollection->Item(index, &device);

    u32 resultIndex {};
    for (int i = 0; i < MAX_INPUT_DEVICES; ++i) {
        if (s_inputAudioClients[i] != nullptr) {
            resultIndex = i;
            break;
        }
    }

    IAudioClient* client {};
    device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&client));
    WAVEFORMATEX* waveFormat;
    client->GetMixFormat(&waveFormat);
    client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, waveFormat, nullptr);
    CoTaskMemFree(waveFormat);

    IAudioCaptureClient* captureClient {};
    client->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(&captureClient));
    s_inputAudioClients[resultIndex] = client;
    s_audioCaptureClients[resultIndex] = captureClient;

    device->Release();
    return resultIndex;
}

void Platform::Audio::freeInputDevice(InputDeviceHandle device) {
    s_inputAudioClients[device]->Release();
    s_audioCaptureClients[device]->Release();
    s_inputAudioClients[device] = nullptr;
    s_audioCaptureClients[device] = nullptr;
}

void audioInit() {
    Platform::Audio::refreshDeviceLists();
}

void audioFree() {
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