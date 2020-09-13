#include "vr_driver.hpp"
#include "vr_server.hpp"

static std::shared_ptr<VRServer> driver = NULL;

void *HmdDriverFactory (const char *pInterfaceName, int *pReturnCode)
{
    if (0 == strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName)) {
        if (driver == NULL) {
            driver = std::make_shared<VRServer>();
        }

        return driver.get();
    }

    if (pReturnCode) {
        *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    }

    return nullptr;
}

std::shared_ptr<VRServer> GetDriver() {
    return driver;
}
