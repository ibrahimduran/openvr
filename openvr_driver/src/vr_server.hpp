#pragma once

#include <memory>
#include <array>
#include <vector>
#include <openvr_driver.h>

#include "ivr_device.hpp"
#include "vr_controller.hpp"
#include "vr_data.hpp"
#include "vr_hmd.hpp"

class VRServer : public vr::IServerTrackedDeviceProvider {
public:
    VRData data;

    bool AddDevice(std::shared_ptr<IVRDevice> device);
    std::shared_ptr<VRController> GetController(VRController::Hand hand);
    std::shared_ptr<VRHMD> GetHMD();

    // Inheritted from IServerTrackedDeviceProvider
    virtual vr::EVRInitError Init(vr::IVRDriverContext * pDriverContext) override;
    virtual const char * const * GetInterfaceVersions() override;
    virtual void Cleanup() override;
    virtual void RunFrame() override;
    virtual bool ShouldBlockStandbyMode() override;
    virtual void EnterStandby() override;
    virtual void LeaveStandby() override;
private:
    std::vector<std::shared_ptr<IVRDevice>> _devices = std::vector<std::shared_ptr<IVRDevice>>();

    std::vector<vr::VREvent_t> _events;
    std::chrono::milliseconds _frame_timing = std::chrono::milliseconds(16);
    std::chrono::system_clock::time_point _last_frame_time = std::chrono::system_clock::now();
};
