#include <thread>
#include <string>
#include <memory>
#include <openvr_driver.h>

#include "ivr_device.hpp"

#pragma once

class Bridge
{
public:
    const struct Connection {
        std::string serial;
        vr::ETrackedDeviceClass type;
        std::shared_ptr<IVRDevice> device;
        std::thread thread;
        bool disconnected;
    };
private:
    std::vector<Bridge::Connection*> _connections;

    // virtual void Loop() = 0;
};

