#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <Windows.h>
#include <openvr_driver.h>

#include "ivr_device.hpp"

class VRController : public IVRDevice
{
public:
    enum class Hand {
        LEFT,
        RIGHT,
        ANY
    };

    VRController(std::string serial, Hand hand = Hand::ANY);
    ~VRController() = default;

    Hand GetHand();

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

    // A struct for concise storage of all of the component handles for this device.
    struct Components {
        vr::VRInputComponentHandle_t
            _system_click,
            _grip_click,
            _app_click,
            _trigger_click,
            _trigger_value,
            _trackpad_x,
            _trackpad_y,
            _trackpad_click,
            _trackpad_touch,
            _haptic;
    };

    Components _components;
private:
    std::string _serial;
    Hand _hand;

    // Stores the openvr supplied device index.
    vr::TrackedDeviceIndex_t _index;

    // Stores the devices current pose.
    vr::DriverPose_t _pose;

    // Stores the timestamp of the pose.
    std::chrono::milliseconds _pose_timestamp;

    // An identifier for openvr for when we want to make property changes to this device.
    vr::PropertyContainerHandle_t _props;
};
