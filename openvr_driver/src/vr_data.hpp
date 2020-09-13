#pragma once

class VRData {
public:

    struct Tracker {
        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;
        float Rx = 0.0f;
        float Ry = 0.0f;
        float Rz = 0.0f;
        float Rw = 0.0f;
    };

    struct HMD : public Tracker {
    };

    struct Controller : public Tracker {
        bool TrackpadTouch = false;
        float TrackpadX = 0.0f;
        float TrackpadY = 0.0f;
        float Trigger = 0.0f;
        bool GripBtn = false;
        bool SystemBtn = false;
        bool ApplicationBtn = false;
    };

    HMD hmd;
    Controller leftHand;
    Controller rightHand;

};
