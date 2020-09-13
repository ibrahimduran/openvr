#include <signal.h>
#include <openvr_driver.h>
#include <openvr_capi.h>
#include <windows.h>
#include <hidapi.h>
#include <thread>
#include <bitset>

#include <iostream>
#include <fstream>

#include "vr_server.hpp"
#include "vr_controller.hpp"
#include "vr_hmd.hpp"
#include "vr_data.hpp"
#include "vr_vdisplay.hpp"

#include "bridge_hid.hpp"

vr::EVRInitError VRServer::Init(vr::IVRDriverContext *pDriverContext)
{
    vr::EVRInitError error = vr::InitServerDriverContext(pDriverContext);

    if (error != vr::EVRInitError::VRInitError_None) {
        return error;
    }

    std::string serial;

    BridgeHID *bridgeHid = new BridgeHID();

    vr::VRDriverLog()->Log("Starting up server thread...");
    std::thread t1(&BridgeHID::FindDevice, bridgeHid);
    t1.detach();

    serial = "vr_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    auto hmd = std::make_shared<VRHMD>(serial);
    hmd->data = &this->data.hmd;
    this->AddDevice(hmd);

    this->data.leftHand.X = -0.5f;
    this->data.rightHand.X = 0.5f;

    // Add a couple controllers
    serial = "vr_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    auto leftHand = std::make_shared<VRController>(serial, VRController::Hand::LEFT);
    leftHand->data = &this->data.leftHand;
    this->AddDevice(leftHand);

    // serial = "vr_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    // auto rightHand = std::make_shared<VRController>(serial, VRController::Hand::RIGHT);
    // rightHand->data = &this->data.rightHand;
    // this->AddDevice(rightHand);

    return vr::EVRInitError::VRInitError_None;
}

bool VRServer::AddDevice(std::shared_ptr<IVRDevice> device)
{
    bool result = vr::VRServerDriverHost()->TrackedDeviceAdded(device->GetSerial().c_str(), device->GetDeviceType(), device.get());

    if (result) {
        this->_devices.push_back(device);
        vr::VRDriverLog()->Log("Device added!");
    } else {
        vr::VRDriverLog()->Log("Device not added!");
    }

    return result;
}

std::shared_ptr<VRController> VRServer::GetController(VRController::Hand hand)
{
    auto controller = std::find_if(
        this->_devices.begin(),
        this->_devices.end(),
        [hand](const std::shared_ptr<IVRDevice>& device_ptr) {
            return device_ptr->GetDeviceType() == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller && ((VRController*) device_ptr.get())->GetHand() == hand;
        }
    );

    if (controller != _devices.end()) {
        return std::dynamic_pointer_cast<VRController>(*controller);
    }

    return NULL;
}

std::shared_ptr<VRHMD> VRServer::GetHMD()
{
    auto hmd = std::find_if(
        this->_devices.begin(),
        this->_devices.end(),
        [](const std::shared_ptr<IVRDevice>& device_ptr) {
            return device_ptr->GetDeviceType() == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD;
        }
    );

    if (hmd != _devices.end()) {
        return std::dynamic_pointer_cast<VRHMD>(*hmd);
    }

    return NULL;
}

void VRServer::RunFrame()
{
    vr::VREvent_t event;
    std::vector<vr::VREvent_t> events;
    while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event)))
        events.push_back(event);

    this->_events = events;

    // Update frame timing
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    this->_frame_timing = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->_last_frame_time);
    this->_last_frame_time = now;

    // Update devices
    for (auto& device : this->_devices)
        device->Update();
}

bool VRServer::ShouldBlockStandbyMode()
{
    return false;
}

void VRServer::EnterStandby()
{
}

void VRServer::LeaveStandby()
{
}

void VRServer::Cleanup()
{
}

const char * const *  VRServer::GetInterfaceVersions()
{
    return vr::k_InterfaceVersions;
}
