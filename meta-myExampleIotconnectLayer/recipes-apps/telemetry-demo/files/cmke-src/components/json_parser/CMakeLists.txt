#message("MESSAGE ${PROJECT_SOURCE_DIR}")
include_directories(inc)

#set(IOTC_USE_PAHO OFF CACHE BOOL "")
#set(IOTC_TPM_SUPPORT OFF CACHE BOOL "") # NOTE: IOTC_USE_PAHO needs to be set to OFF for this

#add_subdirectory(SdkSources/cJSON)

include_directories(config PRIVATE ${IOTC_C_LIB_INCLUDE_DIR} ${IOTC_C_SDK_INCLUDE_DIR} ${IOTC_C_SDK_CJSON_INCLUDE})

add_library(json_parser json_parser.c)
