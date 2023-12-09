#ifdef _WIN32

#include "platforms/platform_windows.hpp"

#include <mmdeviceapi.h>
#include <functiondiscoverykeys.h>

static void freeDeviceList(Platform::Audio::DeviceInfo*& array, u32& length);

static u32 s_inputDeviceCount {};
static u32 s_outputDeviceCount {};
static Platform::Audio::DeviceInfo* s_inputDevices {};
static Platform::Audio::DeviceInfo* s_outputDevices {};

void Platform::Audio::refreshDeviceLists() {
    // see this for info on what's happening here:
    // https://learn.microsoft.com/en-us/windows/win32/api/mmdeviceapi/

    freeDeviceList(s_inputDevices, s_inputDeviceCount);
    freeDeviceList(s_outputDevices, s_outputDeviceCount);

    // Request device info from win32 COM objects
    IMMDeviceEnumerator* deviceEnumerator {};
    IMMDeviceCollection* deviceCollection {};

    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
    deviceEnumerator->EnumAudioEndpoints(EDataFlow::eAll, DEVICE_STATEMASK_ALL, &deviceCollection);

    // Allocate new device info
    u32 deviceCount;
    deviceCollection->GetCount(&deviceCount);

    for (int i = 0; i < deviceCount; ++i) {
        IMMDevice* device {};
        deviceCollection->Item(i, &device);
        IMMEndpoint* endpoint;
        device->QueryInterface(IID_PPV_ARGS(&endpoint));
        EDataFlow dataFlow;
        endpoint->GetDataFlow(&dataFlow);
        switch (dataFlow) {
            case EDataFlow::eCapture: {
                s_inputDeviceCount++;
                break;
            }
            case EDataFlow::eRender: {
                s_outputDeviceCount++;
                break;
            }
            default: { /* Do nothing. */ }
        }
        endpoint->Release();
        device->Release();
    }

    s_inputDevices = new Audio::DeviceInfo[s_inputDeviceCount];
    s_outputDevices = new Audio::DeviceInfo[s_outputDeviceCount];
    u32 inputIndex {};
    u32 outputIndex {};

    for (int i = 0; i < deviceCount; ++i) {

        IMMDevice* device {};
        deviceCollection->Item(i, &device);

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
                    info = &s_inputDevices[inputIndex];
                    inputIndex++;
                    break;
                }
                case EDataFlow::eRender: {
                    info = &s_outputDevices[outputIndex];
                    outputIndex++;
                    break;
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

    // cleanup
    deviceEnumerator->Release();
    deviceCollection->Release();
}

u32 Platform::Audio::getInputDeviceCount() {
    return s_inputDeviceCount;
}

u32 Platform::Audio::getOutputDeviceCount() {
    return s_outputDeviceCount;
}

const Platform::Audio::DeviceInfo* Platform::Audio::getInputDeviceInfo(u32 index) {
    return &s_inputDevices[index];
}

const Platform::Audio::DeviceInfo* Platform::Audio::getOutputDeviceInfo(u32 index) {
    return &s_outputDevices[index];
}

Platform::Audio::InputDeviceHandle Platform::Audio::createInputDevice(u32 index) {
}

Platform::Audio::OutputDeviceHandle Platform::Audio::createOutputDevice(u32 index) {
}

void Platform::Audio::freeInputDevice(Platform::Audio::InputDeviceHandle device) {
}

void Platform::Audio::freeOutputDevice(Platform::Audio::OutputDeviceHandle device) {
}

void audioInit() {
    Platform::Audio::refreshDeviceLists();
}

void audioFree() {
    freeDeviceList(s_inputDevices, s_inputDeviceCount);
    freeDeviceList(s_outputDevices, s_outputDeviceCount);
}

static void freeDeviceList(Platform::Audio::DeviceInfo*& array, u32& length) {
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