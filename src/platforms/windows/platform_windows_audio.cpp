#ifdef _WIN32

#include "platforms/platform_windows.hpp"

#include <mmdeviceapi.h>
#include <functiondiscoverykeys.h>

static void freeDeviceList();

static Platform::Audio::DeviceInfo* s_devices {};
static u32 s_deviceCount {};

void Platform::Audio::refreshDeviceList() {
    // see this for info on what's happening here:
    // https://learn.microsoft.com/en-us/windows/win32/api/mmdeviceapi/

    freeDeviceList();

    // Request device info from win32 COM objects
    IMMDeviceEnumerator* deviceEnumerator {};
    IMMDeviceCollection* deviceCollection {};

    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
    deviceEnumerator->EnumAudioEndpoints(EDataFlow::eAll, DEVICE_STATEMASK_ALL, &deviceCollection);

    // Allocate new device info
    deviceCollection->GetCount(&s_deviceCount);
    s_devices = new Audio::DeviceInfo[s_deviceCount];

    for (int i = 0; i < s_deviceCount; ++i) {
        Audio::DeviceInfo* info = &s_devices[i];
        info->index = i;

        IMMDevice* device {};
        deviceCollection->Item(i, &device);

        IPropertyStore* properties {};
        device->OpenPropertyStore(STGM_READ, &properties);

        DWORD state;
        device->GetState(&state);
        switch (state) {
            case DEVICE_STATE_ACTIVE: {
                info->state = DeviceInfo::Active;
                break;
            }
            case DEVICE_STATE_DISABLED: {
                info->state = DeviceInfo::Disabled;
                break;
            }
            case DEVICE_STATE_NOTPRESENT: {
                info->state = DeviceInfo::NotPresent;
                break;
            }
            case DEVICE_STATE_UNPLUGGED: {
                info->state = DeviceInfo::Unplugged;
                break;
            }
            default: {
                break;
            }
        }

        PROPVARIANT name;
        PropVariantInit(&name);
        properties->GetValue(PKEY_Device_FriendlyName, &name);
        info->name = wcharToChar(name.pwszVal);
        PropVariantClear(&name);

        IMMEndpoint* endpoint;
        EDataFlow dataFlow;
        device->QueryInterface(IID_PPV_ARGS(&endpoint));
        endpoint->GetDataFlow(&dataFlow);
        switch (dataFlow) {
            case EDataFlow::eCapture: {
                info->dataFlow = DeviceInfo::Capture;
                break;
            }
            case EDataFlow::eRender: {
                info->dataFlow = DeviceInfo::Render;
                break;
            }
            default: {
                break;
            }
        }
        endpoint->Release();

        properties->Release();
        device->Release();
    }

    // cleanup
    deviceEnumerator->Release();
    deviceCollection->Release();
}

u32 Platform::Audio::getDeviceCount() {
    return s_deviceCount;
}

const Platform::Audio::DeviceInfo* Platform::Audio::getDeviceInfo(u32 index) {
    return &s_devices[index];
}

void audioInit() {
    Platform::Audio::refreshDeviceList();
}

void audioFree() {
    freeDeviceList();
}

static void freeDeviceList() {
    if (s_devices != nullptr) {
        for (int i = 0; i < s_deviceCount; ++i) {
            delete s_devices[i].name;
        }
        delete[] s_devices;
    }
}

#endif // _WIN32