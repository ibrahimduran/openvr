#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <Windows.h>
#include <openvr_driver.h>

#include "ivr_device.hpp"

class VRHMD : public IVRDevice, public vr::IVRDisplayComponent
{
public:
    VRHMD(std::string serial);
    ~VRHMD() = default;

    // Inherited via IVRDisplayComponent
    virtual void GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override;
    virtual bool IsDisplayOnDesktop() override;
    virtual bool IsDisplayRealDisplay() override;
    virtual void GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight) override;
    virtual void GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override;
    virtual void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) override;
    virtual vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) override;

    // Inheritted from IVRDevice
    virtual void Action(uint8_t action, char* buffer, uint8_t size) override;
    virtual VRData::Tracker* GetData();

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
};
