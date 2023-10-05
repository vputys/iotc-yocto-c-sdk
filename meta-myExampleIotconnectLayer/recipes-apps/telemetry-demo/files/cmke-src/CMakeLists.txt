cmake_minimum_required(VERSION 3.0.2)

project(telemetry-demo)

set(IOTC_USE_PAHO OFF CACHE BOOL "")
set(IOTC_TPM_SUPPORT OFF CACHE BOOL "") # NOTE: IOTC_USE_PAHO needs to be set to OFF for this
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../iotc-generic-c-sdk SdkSources)
add_subdirectory(${IOTC_C_SDK_DIR} SdkSources)

include(GNUInstallDirs)

include_directories(config PRIVATE ${IOTC_C_LIB_INCLUDE_DIR} ${IOTC_C_SDK_INCLUDE_DIR})
add_executable(telemetry-demo main.c)

target_link_libraries(telemetry-demo iotc-c-generic-sdk)
target_sources(telemetry-demo PUBLIC json_parser.c)

