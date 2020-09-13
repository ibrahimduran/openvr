#include <cmath>
#include <signal.h>
#include "vr_hmd.hpp"
#include "vr_driver.hpp"

VRHMD::VRHMD(std::string serial): _pose({ 0 })
{
    _serial = serial;
    _index = vr::k_unTrackedDeviceIndexInvalid;
    _pose = IVRDevice::MakeDefaultPose(true, false);
}

std::string VRHMD::GetSerial()
{
    return _serial;
}

vr::EVRInitError VRHMD::Activate(vr::TrackedDeviceIndex_t index)
{
    // vr::VROverlayHandle_t *pOverlayHandle;
    // vr::IVROverlay->CreateOverlay("MyOverlay", "My Overlay", pOverlayHandle);

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

vr::ETrackedDeviceClass VRHMD::GetDeviceType()
{
    return vr::TrackedDeviceClass_HMD;
}

vr::TrackedDeviceIndex_t VRHMD::GetDeviceIndex()
{
    return this->_index;
}

VRData::Tracker* VRHMD::GetData()
{
    return this->data;
}

void VRHMD::Action(uint8_t action, char* buffer, uint8_t size)
{
}

void VRHMD::Update()
{
    if (this->_index == vr::k_unTrackedDeviceIndexInvalid) {
        return;
    }

    _pose = IVRDevice::MakeDefaultPose(!this->isDisconnected, !this->isDisconnected);

    _pose.vecPosition[0] = this->data->X;
    _pose.vecPosition[1] = this->data->Y;
    _pose.vecPosition[2] = this->data->Z;

    float yaw = this->data->Ry / 90;
    float pitch = this->data->Rz / 90;
    float roll = -1 * this->data->Rx / 90;

    double cy = cos(yaw * 0.5);
    double sy = sin(yaw * 0.5);
    double cp = cos(pitch * 0.5);
    double sp = sin(pitch * 0.5);
    double cr = cos(roll * 0.5);
    double sr = sin(roll * 0.5);

    _pose.qRotation.w = cr * cp * cy + sr * sp * sy;
    _pose.qRotation.x = sr * cp * cy - cr * sp * sy;
    _pose.qRotation.y = cr * sp * cy + sr * cp * sy;
    _pose.qRotation.z = cr * cp * sy - sr * sp * cy;

    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(_index, _pose, sizeof(vr::DriverPose_t));
}

void VRHMD::Deactivate()
{
    _index = vr::k_unTrackedDeviceIndexInvalid;
}

void VRHMD::EnterStandby()
{
}

void VRHMD::DebugRequest(const char * request, char* response_buffer, uint32_t response_buffer_size)
{
    // No custom debug requests defined
    if (response_buffer_size >= 1)
        response_buffer[0] = 0;
}

void * VRHMD::GetComponent(const char * component)
{
    // This device has a display component, so check if the requested component
    // is the IVRDisplayComponent, and cast and return it
    if (std::string(component) == std::string(vr::IVRDisplayComponent_Version)) {
        return static_cast<vr::IVRDisplayComponent*>(this);
    }

    // No extra components on this device so always return nullptr
    return nullptr;
}

vr::DriverPose_t VRHMD::GetPose()
{
    return _pose;
}

void VRHMD::GetWindowBounds(int32_t * x, int32_t * y, uint32_t * width, uint32_t * height)
{
    // Use the stored display properties to return the window bounds
    *x = _display_properties.display_offset_x;
    *y = _display_properties.display_offset_y;
    *width = _display_properties.display_width;
    *height = _display_properties.display_height;
}

bool VRHMD::IsDisplayOnDesktop()
{
    return true;
}

bool VRHMD::IsDisplayRealDisplay()
{
    return false;
}

void VRHMD::GetRecommendedRenderTargetSize(uint32_t * width, uint32_t * height)
{
    // Use the stored display properties to return the render target size
    *width = _display_properties.render_width;
    *height = _display_properties.render_height;
}

void VRHMD::GetEyeOutputViewport(vr::EVREye eye, uint32_t * x, uint32_t * y, uint32_t * width, uint32_t * height)
{
    // Use the stored display properties to work out each eye's viewport size
    *y = _display_properties.display_offset_y;
    *width = _display_properties.render_width / 2;
    *height = _display_properties.render_height;

    if (eye == vr::EVREye::Eye_Left) {
        *x = _display_properties.display_offset_x;
    }
    else {
        *x = _display_properties.display_offset_x + _display_properties.render_width/2;
    }
}

void VRHMD::GetProjectionRaw(vr::EVREye eye, float * left, float * right, float * top, float * bottom)
{
    *left = -1;
    *right = 1;
    *top = -1;
    *bottom = 1;
}

vr::DistortionCoordinates_t VRHMD::ComputeDistortion(vr::EVREye eye, float u, float v)
{
    vr::DistortionCoordinates_t coordinates;
    coordinates.rfBlue[0] = u;
    coordinates.rfBlue[1] = v;
    coordinates.rfGreen[0] = u;
    coordinates.rfGreen[1] = v;
    coordinates.rfRed[0] = u;
    coordinates.rfRed[1] = v;
    return coordinates;
}
