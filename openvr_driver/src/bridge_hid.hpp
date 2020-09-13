#pragma once

#include <hidapi.h>
#include "bridge.hpp"

class BridgeHID : public Bridge
{
public:
    static const uint16_t VID = 0xe502;

    BridgeHID();
    void FindDevice();
    void HandleDevice(hid_device* dev, Bridge::Connection* conn);

private:
    std::vector<Bridge::Connection *> _connections;
};
