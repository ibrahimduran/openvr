cmake_minimum_required(VERSION 3.5)
project(OpenVR_MyDriver CXX)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

set(INSTALL_PATH "C:/Program Files (x86)/Steam/steamapps/common/SteamVR/drivers")
set(OPENVR_LIB_DIR ${CMAKE_SOURCE_DIR}/deps/openvr/lib/win64 CACHE STRING "OpenVR .lib directory")
set(OPENVR_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/deps/openvr/headers CACHE STRING "OpenVR include directory")
set(HIDAPI_LIB_DIR ${CMAKE_SOURCE_DIR}/deps/hidapi/windows/x64/Debug CACHE STRING "HIDApi .lib directory")
set(HIDAPI_BIN_DIR ${CMAKE_SOURCE_DIR}/deps/hidapi/windows/x64/Debug CACHE STRING "HIDApi .dll directory")
set(HIDAPI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/deps/hidapi/hidapi CACHE STRING "HIDApi include directory")
set(LINALG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/deps/linalg CACHE STRING "Linalg include directory")

set(INCLUDE_DIR ${OPENVR_INCLUDE_DIR} ${HIDAPI_INCLUDE_DIR} ${LINALG_INCLUDE_DIR})
set(LIBRARIES ${OPENVR_LIB_DIR}/openvr_api.lib ${HIDAPI_LIB_DIR}/hidapi.lib)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/D_UNICODE>")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/DUNICODE>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/D_UNICODE>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/DUNICODE>")

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp src/*.h)

add_library(mydriver SHARED ${SOURCES};${HEADERS})
target_include_directories(mydriver PRIVATE include/)

set_target_properties(mydriver PROPERTIES PREFIX "driver_")

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

link_directories(${CMAKE_SOURCE_DIR})

target_include_directories(mydriver PRIVATE ${INCLUDE_DIR})
target_link_libraries(mydriver ${LIBRARIES})

file(GLOB testfiles "test/*.cpp")
foreach(file ${testfiles})
  get_filename_component(filename ${file} NAME_WE)
  add_executable(test_${filename} test/${filename}.cpp)
  target_include_directories(test_${filename} PRIVATE ${INCLUDE_DIR})
  target_link_libraries(test_${filename} ${LIBRARIES})
endforeach()

install(TARGETS mydriver DESTINATION ${INSTALL_PATH}/mydriver/bin/win64)
install(TARGETS mydriver DESTINATION ${INSTALL_PATH}/mydriver/bin/win32)

install(FILES ${HIDAPI_BIN_DIR}/hidapi.dll DESTINATION ${INSTALL_PATH}/mydriver/bin/win64)
install(FILES ${CMAKE_SOURCE_DIR}/driver.vrdrivermanifest DESTINATION ${INSTALL_PATH}/mydriver)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${INSTALL_PATH}/mydriver)
