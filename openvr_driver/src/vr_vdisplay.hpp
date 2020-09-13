#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <Windows.h>
#include <openvr_driver.h>

#include "ivr_device.hpp"

class VRVDisplay : public IVRDevice, public vr::IVRVirtualDisplay
{
public:
    VRVDisplay(std::string serial);
    ~VRVDisplay() = default;

    // Inherited via IVRVirtualDisplay
    virtual void Present(const vr::PresentInfo_t *pPresentInfo, uint32_t unPresentInfoSize);
    virtual void WaitForPresent();
    virtual bool GetTimeSinceLastVsync(float *pfSecondsSinceLastVsync, uint64_t *pulFrameCounter);

    // Inheritted from IVRDevice
    VRData::HMD * data;
    virtual void Action(uint8_t action, char* buffer, uint8_t size) override;

    virtual std::string GetSerial() override;
    virtual void Update() override;
    virtual vr::TrackedDeviceIndex_t GetDeviceIndex() override;
    virtual vr::ETrackedDeviceClass GetDeviceType() override;
    virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t index) override;
    virtual void Deactivate() override;
    virtual void EnterStandby() override;
    virtual void* GetComponent(const char* component) override;
    virtual void DebugRequest(const char* request, char* response_buffer, uint32_t response_buffer_size) override;
    virtual vr::DriverPose_t GetPose() override;
private:
    std::string _serial;

    // Stores the openvr supplied device index.
    vr::TrackedDeviceIndex_t _index;

    // Stores the devices current pose.
    vr::DriverPose_t _pose;

    // Stores the timestamp of the pose.
    std::chrono::milliseconds _pose_timestamp;

    // An identifier for openvr for when we want to make property changes to this device.
    vr::PropertyContainerHandle_t _props;
    
    struct DisplayProperties {
        int display_offset_x = 0;
        int display_offset_y = 0;
        int display_width = 1920;
        int display_height = 1080;
        int render_width = 1920;
        int render_height = 1080;
    };

    DisplayProperties _display_properties;

    uint64_t m_nVsyncCounter = 1;
    uint64_t m_flLastVsyncTimeInSeconds = 0;

};
