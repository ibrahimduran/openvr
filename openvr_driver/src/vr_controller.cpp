#include <cmath>
#include <linalg.h>
#include "vr_controller.hpp"
#include "vr_hmd.hpp"
#include "vr_driver.hpp"

VRController::VRController(std::string serial, Hand hand)
{
    _hand = hand;
    _serial = serial;
    _index = vr::k_unTrackedDeviceIndexInvalid;
    _pose = IVRDevice::MakeDefaultPose(true, false);
}

VRController::Hand VRController::GetHand()
{
    return _hand;
}

std::string VRController::GetSerial()
{
    return _serial;
}

vr::EVRInitError VRController::Activate(vr::TrackedDeviceIndex_t index)
{
    // Save the device index
    _index = index;

    // Get the properties handle for our controller
    _props = vr::VRProperties()->TrackedDeviceToPropertyContainer(_index);

    // Set some universe ID (Must be 2 or higher)
    vr::VRProperties()->SetUint64Property(_props, vr::Prop_CurrentUniverseId_Uint64, 2);
    
    vr::VRProperties()->SetStringProperty(_props, vr::Prop_ControllerType_String, "vive_controller");
    vr::VRProperties()->SetStringProperty(_props, vr::Prop_InputProfilePath_String, "{htc}/input/vive_controller_profile.json");

    vr::VRProperties()->SetStringProperty(_props, vr::Prop_ModelNumber_String, "ViveMV");
    vr::VRProperties()->SetStringProperty(_props, vr::Prop_ManufacturerName_String, "HTC");
    vr::VRProperties()->SetStringProperty(_props, vr::Prop_RenderModelName_String, "vr_controller_vive_1_5");

    // vr::VRProperties()->SetStringProperty(_props, vr::Prop_NamedIconPathDeviceOff_String, "{mydriver}/icons/controller_status_off.png");
    // vr::VRProperties()->SetStringProperty(_props, vr::Prop_NamedIconPathDeviceSearching_String, "{mydriver}/icons/controller_status_ready.png");
    // vr::VRProperties()->SetStringProperty(_props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, controller_not_ready_file.c_str());
    // vr::VRProperties()->SetStringProperty(_props, vr::Prop_NamedIconPathDeviceReadyAlert_String, controller_not_ready_file.c_str());
    // vr::VRProperties()->SetStringProperty(_props, vr::Prop_NamedIconPathDeviceNotReady_String, controller_not_ready_file.c_str());
    // vr::VRProperties()->SetStringProperty(_props, vr::Prop_NamedIconPathDeviceStandby_String, controller_not_ready_file.c_str());
    // vr::VRProperties()->SetStringProperty(_props, vr::Prop_NamedIconPathDeviceAlertLow_String, controller_not_ready_file.c_str());

    vr::VRProperties()->SetStringProperty(_props, vr::Prop_TrackingSystemName_String, "VR Controller");
    vr::VRProperties()->SetInt32Property(_props, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_Controller);
    
    // Add our controller components. (These are the same as the regular vive controller)
    vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/system/click", &_components._system_click);
    vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/grip/click", &_components._grip_click);
    vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/application_menu/click", &_components._app_click);
    vr::VRDriverInput()->CreateScalarComponent(_props, "/input/trigger/value", &_components._trigger_value, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided);
    vr::VRDriverInput()->CreateScalarComponent(_props, "/input/trackpad/x", &_components._trackpad_x, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
    vr::VRDriverInput()->CreateScalarComponent(_props, "/input/trackpad/y", &_components._trackpad_y, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); 
    vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/trackpad/click", &_components._trackpad_click);
    vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/trackpad/touch", &_components._trackpad_touch);
    vr::VRDriverInput()->CreateHapticComponent(_props, "/output/haptic", &_components._haptic);

    // Give SteamVR a hint at what hand this controller is for
    if (this->_hand == Hand::LEFT) {
        vr::VRProperties()->SetInt32Property(_props, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_LeftHand);
    } else if (this->_hand == Hand::RIGHT) {
        vr::VRProperties()->SetInt32Property(_props, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_RightHand);
    } else {
        vr::VRProperties()->SetInt32Property(_props, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_OptOut);
    }

    return vr::VRInitError_None;
}

vr::ETrackedDeviceClass VRController::GetDeviceType()
{
    return vr::TrackedDeviceClass_Controller;
}

vr::TrackedDeviceIndex_t VRController::GetDeviceIndex()
{
    return this->_index;
}

VRData::Tracker* VRController::GetData()
{
    return this->data;
}

void VRController::Action(uint8_t action, char* buffer, uint8_t size)
{
    if (action == 1) {
        vr::VRDriverInput()->UpdateBooleanComponent(this->_components._system_click, buffer[0] == 1, 0);
    } else if (action == 2) {
        vr::VRDriverInput()->UpdateBooleanComponent(this->_components._app_click, buffer[0] == 1, 0);
    } else if (action == 3) {
        vr::VRDriverInput()->UpdateScalarComponent(this->_components._trigger_value, buffer[0] == 1 ? 1.0f : 0.0f, 0);
        vr::VRDriverInput()->UpdateBooleanComponent(this->_components._trigger_click, buffer[0] == 1, 0);
    } else if (action == 4) {
        vr::VRDriverInput()->UpdateBooleanComponent(this->_components._system_click, buffer[0] & 0b00000001 == 1, 0);
        vr::VRDriverInput()->UpdateBooleanComponent(this->_components._app_click, buffer[0] & 0b00000010 == 1, 0);
        vr::VRDriverInput()->UpdateScalarComponent(this->_components._trigger_value, buffer[0] == 0x04 ? 1.0f : 0.0f, 0);
        vr::VRDriverInput()->UpdateBooleanComponent(this->_components._trigger_click, buffer[0] == 0x04, 0);
    }
}

void VRController::Update()
{
    if (this->_index == vr::k_unTrackedDeviceIndexInvalid) {
        return;
    }

    _pose = IVRDevice::MakeDefaultPose(!this->isDisconnected, !this->isDisconnected);

    std::shared_ptr<VRHMD> hmd = GetDriver()->GetHMD();
    if (hmd != NULL) {
        // vr::VRDriverLog()->Log("Found HMD!");

        vr::DriverPose_t hmd_pose = hmd->GetPose();

        // Here we setup some transforms so our controllers are offset from the headset by a small amount so we can see them
        linalg::vec<float, 3> hmd_position{ (float)hmd_pose.vecPosition[0], (float)hmd_pose.vecPosition[1], (float)hmd_pose.vecPosition[2] };
        linalg::vec<float, 4> hmd_rotation{ (float)hmd_pose.qRotation.x, (float)hmd_pose.qRotation.y, (float)hmd_pose.qRotation.z, (float)hmd_pose.qRotation.w };

        // Do shaking animation if haptic vibration was requested
        float controller_y = -0.2f + 0.01f * std::sinf(8 * 3.1415f * 0.0f); // * vibrate_anim_state_

        // Left hand controller on the left, right hand controller on the right, any other handedness sticks to the middle
        float controller_x = this->_hand == Hand::LEFT ? -0.2f : (this->_hand == Hand::RIGHT ? 0.2f : 0.f);

        linalg::vec<float, 3> hmd_pose_offset = { controller_x, controller_y, -0.5f };

        hmd_pose_offset = linalg::qrot(hmd_rotation, hmd_pose_offset);

        linalg::vec<float, 3> final_pose = hmd_pose_offset + hmd_position;

        _pose.vecPosition[0] = final_pose.x;
        _pose.vecPosition[1] = final_pose.y;
        _pose.vecPosition[2] = final_pose.z;

        // _pose.vecPosition[0] = this->data->X;
        // _pose.vecPosition[1] = this->data->Y;
        // _pose.vecPosition[2] = this->data->Z;

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
    } else {
        vr::VRDriverLog()->Log("HMD pose Not found!");
    }

    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(this->_index, this->_pose, sizeof(vr::DriverPose_t));
}

void VRController::Deactivate()
{
    // Clear device id
    _index = vr::k_unTrackedDeviceIndexInvalid;
}

void VRController::EnterStandby()
{
}

void VRController::DebugRequest(const char * request, char* response_buffer, uint32_t response_buffer_size)
{
    // No custom debug requests defined
    if (response_buffer_size >= 1)
        response_buffer[0] = 0;
}

void * VRController::GetComponent(const char * component)
{
    // No extra components on this device so always return nullptr
    return nullptr;
}

vr::DriverPose_t VRController::GetPose()
{
    return _pose;
}
