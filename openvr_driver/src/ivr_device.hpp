#pragma once

#include <memory>
#include <string>
#include <chrono>

#include <Windows.h>
#include <openvr_driver.h>

#include <hidapi.h>
#include "vr_data.hpp"

class IVRDevice : public vr::ITrackedDeviceServerDriver
{
public:
    VRData::Tracker * data;
    bool isDisconnected = false;

    virtual void Action(uint8_t action, char* buffer, uint8_t size) = 0;
    virtual VRData::Tracker* GetData() = 0;

    /// <summary>
    /// Returns the serial string for this device
    /// </summary>
    /// <returns>Device serial</returns>
    virtual std::string GetSerial() = 0;

    /// <summary>
    /// Runs any update logic for this device, called once per frame.
    /// </summary>
    virtual void Update() = 0;

    /// <summary>
    /// Returns the OpenVR device index, this should be 0 for HMDs.
    /// </summary>
    /// <returns>Index</returns>
    virtual vr::TrackedDeviceIndex_t GetDeviceIndex() = 0;

    /// <summary>
    /// Returns which type of device this device is
    /// </summary>
    /// <returns>The type of device</returns>
    virtual vr::ETrackedDeviceClass GetDeviceType() = 0;

    /// <summary>
    /// Makes a default device pose 
    /// </summary>
    /// <returns>Default initialised pose</returns>
    static inline vr::DriverPose_t MakeDefaultPose(bool connected = true, bool tracking = true) {
        vr::DriverPose_t out_pose = { 0 };

        out_pose.deviceIsConnected = connected;
        out_pose.poseIsValid = tracking;
        out_pose.result = tracking ? vr::ETrackingResult::TrackingResult_Running_OK : vr::ETrackingResult::TrackingResult_Running_OutOfRange;
        out_pose.willDriftInYaw = false;
        out_pose.shouldApplyHeadModel = false;
        out_pose.qDriverFromHeadRotation.w = out_pose.qWorldFromDriverRotation.w = out_pose.qRotation.w = 1.0;

        return out_pose;
    }

    /**
     * Inherited via ITrackedDeviceServerDriver
     */

    /// <summary>
    /// Activates this device
    /// Is called when vr::VRServerDriverHost()->TrackedDeviceAdded is called
    /// </summary>
    /// <param name="index">The device index</param>
    /// <returns>Error code</returns>
    virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t index) = 0;

    /// <summary>
    /// Deactivates the device
    /// </summary>
    virtual void Deactivate() = 0;

    /// <summary>
    /// Tells the device to enter stand-by mode
    /// </summary>
    virtual void EnterStandby() = 0;

    /// <summary>
    /// Gets a specific component from this device
    /// </summary>
    /// <param name="component">Requested component</param>
    /// <returns>Non-owning pointer to the component</returns>
    virtual void* GetComponent(const char* component) = 0;

    /// <summary>
    /// Handles a debug request
    /// </summary>
    /// <param name="request">Request type</param>
    /// <param name="response_buffer">Response buffer</param>
    /// <param name="response_buffer_size">Response buffer size</param>
    virtual void DebugRequest(const char* request, char* response_buffer, uint32_t response_buffer_size) = 0;

    /// <summary>
    /// Gets the current device pose
    /// </summary>
    /// <returns>Device Pose</returns>
    virtual vr::DriverPose_t GetPose() = 0;

    ~IVRDevice() = default;

    unsigned char *pData = NULL;
    unsigned int nData = NULL;
};
