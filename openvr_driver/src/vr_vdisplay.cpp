#include <cmath>
#include <signal.h>
#include "vr_vdisplay.hpp"
#include "vr_driver.hpp"
#include <time.h>

VRVDisplay::VRVDisplay(std::string serial): _pose({ 0 })
{
    // _index = vr::k_unTrackedDeviceIndexInvalid;
    _serial = serial;

    // set default pose values
    // Create some random but unique serial
    _serial = "fc_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());

    // Set up some defalt rotation pointing down -z
    _pose.qRotation.w = 1.0;
    _pose.qRotation.x = 0.0;
    _pose.qRotation.y = 0.0;
    _pose.qRotation.z = 0.0;

    _pose.qWorldFromDriverRotation.w = 1.0;
    _pose.qWorldFromDriverRotation.x = 0.0;
    _pose.qWorldFromDriverRotation.y = 0.0;
    _pose.qWorldFromDriverRotation.z = 0.0;

    _pose.qDriverFromHeadRotation.w = 1.0;
    _pose.qDriverFromHeadRotation.x = 0.0;
    _pose.qDriverFromHeadRotation.y = 0.0;
    _pose.qDriverFromHeadRotation.z = 0.0;

    _pose.vecPosition[1] = 1.0;

    // To ensure no complaints about tracking
    _pose.poseIsValid = true;
    _pose.result = vr::ETrackingResult::TrackingResult_Running_OK;
    _pose.deviceIsConnected = true;
}

std::string VRVDisplay::GetSerial()
{
    return _serial;
}

vr::EVRInitError VRVDisplay::Activate(vr::TrackedDeviceIndex_t index)
{
    // vr::VROverlayHandle_t *pOverlayHandle;
    // vr::IVROverlay->CreateOverlay("MyOverlay", "My Overlay", pOverlayHandle);

    // set settings

    // Save the index we are given
    _index = index;

    // Get the properties handle
    _props = vr::VRProperties()->TrackedDeviceToPropertyContainer(_index);

    vr::VRProperties()->SetBoolProperty(_props, vr::Prop_DisplayDebugMode_Bool, true);

    // Set some universe ID (Must be 2 or higher)
    // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
    vr::VRProperties()->SetUint64Property(_props, vr::Prop_CurrentUniverseId_Uint64, 2);
    
    // Set the IPD to be whatever steam has configured
    vr::VRProperties()->SetFloatProperty(_props, vr::Prop_UserIpdMeters_Float, vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float));

    // Set the display FPS
    vr::VRProperties()->SetFloatProperty(_props, vr::Prop_DisplayFrequency_Float, 60.f);

    // Disable warnings about compositor not being fullscreen
    // avoid "not fullscreen" warnings from vrmonitor
    vr::VRProperties()->SetBoolProperty(_props, vr::Prop_IsOnDesktop_Bool, false);

    return vr::VRInitError_None;
}

vr::ETrackedDeviceClass VRVDisplay::GetDeviceType()
{
    return vr::TrackedDeviceClass_HMD;
}

vr::TrackedDeviceIndex_t VRVDisplay::GetDeviceIndex()
{
    return this->_index;
}

void VRVDisplay::Action(uint8_t action, char* buffer, uint8_t size)
{
}

void VRVDisplay::Update()
{
    // // Update time delta (for working out velocity)
    std::chrono::milliseconds time_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    double time_since_epoch_seconds = time_since_epoch.count() / 1000.0;
    double pose_time_delta_seconds = (time_since_epoch - _pose_timestamp).count() / 1000.0;

    // // Update pose timestamp
    _pose_timestamp = time_since_epoch;

    // // Copy the previous position data
    double previous_position[3] = { 0 };
    std::copy(std::begin(_pose.vecPosition), std::end(_pose.vecPosition), std::begin(previous_position));

    // // Update the position with our new data
    // _pose.vecPosition[0] = 2 * std::sin(time_since_epoch_seconds);
    // _pose.vecPosition[1] = 1.0;
    // _pose.vecPosition[2] = -1 + 2 * std::cos(time_since_epoch_seconds);

    // // Update the velocity
    // _pose.vecVelocity[0] = (_pose.vecPosition[0] - previous_position[0]) / pose_time_delta_seconds;
    // _pose.vecVelocity[1] = (_pose.vecPosition[1] - previous_position[1]) / pose_time_delta_seconds;
    // _pose.vecVelocity[2] = (_pose.vecPosition[2] - previous_position[2]) / pose_time_delta_seconds;

    // If we are still tracking, update openvr with our new pose data

    _pose.vecPosition[0] = data->X;
    _pose.vecPosition[1] = data->Y;
    _pose.vecPosition[2] = data->Z;
    
    _pose.vecVelocity[0] = 1.0f;
    _pose.vecVelocity[1] = 1.0f;
    _pose.vecVelocity[2] = 1.0f;

    // _pose.qRotation.x = fmax(fmin(data->Rx, 1.0f), -1.0f);
    // _pose.qRotation.y = fmax(fmin(data->Ry, 1.0f), -1.0f);
    // _pose.qRotation.z = fmax(fmin(data->Rz, 1.0f), -1.0f);
    _pose.qRotation.x = data->Rx;
    _pose.qRotation.y = data->Ry;
    _pose.qRotation.z = data->Rz;

    if (this->_index != vr::k_unTrackedDeviceIndexInvalid)
    {
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(_index, _pose, sizeof(vr::DriverPose_t));
    }
}

void VRVDisplay::Deactivate()
{
    _index = vr::k_unTrackedDeviceIndexInvalid;
}

void VRVDisplay::EnterStandby()
{
}

void VRVDisplay::DebugRequest(const char * request, char* response_buffer, uint32_t response_buffer_size)
{
    // No custom debug requests defined
    if (response_buffer_size >= 1)
        response_buffer[0] = 0;
}

void * VRVDisplay::GetComponent(const char * component)
{
    // This device has a display component, so check if the requested component
    // is the IVRDisplayComponent, and cast and return it
    if (std::string(component) == std::string(vr::IVRDisplayComponent_Version)) {
        return static_cast<vr::IVRVirtualDisplay*>(this);
    }

    // No extra components on this device so always return nullptr
    return nullptr;
}

vr::DriverPose_t VRVDisplay::GetPose()
{
    return _pose;
}

void VRVDisplay::Present(const vr::PresentInfo_t *pPresentInfo, uint32_t unPresentInfoSize)
{
    // ID3D11Device::OpenSharedResource(pPresentInfo->backbufferTextureHandle);
    m_nVsyncCounter++;
}

void VRVDisplay::WaitForPresent()
{
    Sleep(20);
    time_t seconds;
    time(&seconds);
    m_flLastVsyncTimeInSeconds = seconds;
}

bool VRVDisplay::GetTimeSinceLastVsync(float *pfSecondsSinceLastVsync, uint64_t *pulFrameCounter)
{
    time_t seconds;
    time(&seconds);

    *pfSecondsSinceLastVsync = ( float )( seconds - m_flLastVsyncTimeInSeconds );
    *pulFrameCounter = m_nVsyncCounter;
    return true;
}
