#include <thread>
#include <Windows.h>
#include <hidapi.h>
#include <openvr_driver.h>
#include "bridge_hid.hpp"
#include <iostream>
#include <fstream>

#include "bridge.hpp"
#include "vr_controller.hpp"
#include "vr_data.hpp"
#include "vr_hmd.hpp"
#include "vr_driver.hpp"

BridgeHID::BridgeHID()
{
}

void BridgeHID::FindDevice()
{
    vr::VRDriverLog()->Log("Looking up supported VR devices...");
    struct hid_device_info *devs, *cur_dev;
    hid_device *handle = NULL;

    std::string serial2 = "vr_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    auto aa = std::make_shared<VRController>(serial2);
    aa->data = &(GetDriver()->data.leftHand);

    while (true)
    {
        devs = hid_enumerate(0x0, 0x0);
        cur_dev = devs;

        while (cur_dev) {
            if (cur_dev->vendor_id == BridgeHID::VID) {

                uint16_t pid = cur_dev->product_id;
                std::string serial = "vr_" + std::to_string(pid);
                vr::ETrackedDeviceClass type = vr::TrackedDeviceClass_Invalid;

                if (cur_dev->usage_page == 1 && cur_dev->usage == 4) {
                    type = vr::TrackedDeviceClass_Controller;
                } else if (cur_dev->usage_page == 3 && cur_dev->usage == 6) {
                    type = vr::TrackedDeviceClass_HMD;
                }

                // check if this is first time this device is connecting
                auto existing = std::find_if(
                    this->_connections.begin(),
                    this->_connections.end(),
                    [serial](const Bridge::Connection* conn) {
                        return conn->serial == serial;
                    }
                );

                // reconnect this device
                if (existing != _connections.end()) {
                    if (!(*existing)->disconnected) {
                        cur_dev = cur_dev->next;
                        continue;
                    }

                    vr::VRDriverLog()->Log("Device will be restored.");

                    (*existing)->thread.~thread();
                    (*existing)->thread = std::thread(&BridgeHID::HandleDevice, this, hid_open_path(cur_dev->path), *existing);
                    (*existing)->thread.detach();

                    vr::VRDriverLog()->Log("Device connection has been restored.");
                    cur_dev = cur_dev->next;
                    continue;
                }

                if (type == vr::TrackedDeviceClass_Invalid) {
                    vr::VRDriverLog()->Log("Device connection has been rejected for unknown type.");
                    cur_dev = cur_dev->next;
                    continue;
                }

                std::shared_ptr<IVRDevice> device = NULL;

                // instantiate new device if there isn't already one
                if (type == vr::TrackedDeviceClass_HMD && GetDriver()->GetHMD() == NULL) {
                    device = std::make_shared<VRHMD>(serial);
                    device->data = &GetDriver()->data.hmd;
                } else if (type == vr::TrackedDeviceClass_Controller && GetDriver()->GetController(VRController::Hand::LEFT) == NULL) {
                    device = std::make_shared<VRController>(serial, VRController::Hand::LEFT);
                    device->data = &GetDriver()->data.leftHand;
                } else if (type == vr::TrackedDeviceClass_Controller && GetDriver()->GetController(VRController::Hand::RIGHT) == NULL) {
                    device = std::make_shared<VRController>(serial, VRController::Hand::RIGHT);
                    device->data = &GetDriver()->data.rightHand;
                } else {
                    vr::VRDriverLog()->Log("Device connection has been rejected for existing device.");
                    cur_dev = cur_dev->next;
                    continue;
                }

                GetDriver()->AddDevice(device);

                Bridge::Connection * conn = new Bridge::Connection();
                conn->type = type;
                conn->device = device;
                conn->serial = serial;
                conn->thread = std::thread(&BridgeHID::HandleDevice, this, hid_open_path(cur_dev->path), conn);
                conn->thread.detach();

                _connections.push_back(conn);

                vr::VRDriverLog()->Log("Device has been added.");
            }

            cur_dev = cur_dev->next;
        }

        hid_free_enumeration(devs);
        Sleep(3000);
    }
}

void BridgeHID::HandleDevice(hid_device* dev, Bridge::Connection* conn)
{
    conn->disconnected = false;
    conn->device->isDisconnected = false;

    uint8_t * buffer = new uint8_t[200];

    // +1 offset for report id from hidapi
    uint8_t* buttons = (uint8_t*) &buffer[1];
    float* Rx = (float*) &buffer[2];
    float* Ry = (float*) &buffer[6];
    float* Rz = (float*) &buffer[10];

    VRData::Tracker* data = conn->device->GetData();

    int bytes = hid_read(dev, buffer, 14);
    while (bytes > 0) {
        data->Ry = *Ry;
        data->Rx = *Rx;
        data->Rz = *Rz;
        data->Rw = 1.0f;

        conn->device->Action(4, (char*) buttons, 1);

        bytes = hid_read(dev, buffer, 14);
    }

    conn->disconnected = true;
    conn->device->isDisconnected = true;

    vr::VRDriverLog()->Log("Disconnected!");
}
