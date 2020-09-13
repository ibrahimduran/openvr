#pragma once

#include <cstdlib>
#include <memory>

#include <openvr_driver.h>
#include "vr_server.hpp"

extern "C" __declspec(dllexport) void* HmdDriverFactory(const char* interface_name, int* return_code);

std::shared_ptr<VRServer> GetDriver();
