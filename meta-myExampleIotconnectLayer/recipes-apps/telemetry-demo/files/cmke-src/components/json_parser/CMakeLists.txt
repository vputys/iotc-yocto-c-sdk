include_directories(inc)

include_directories(config PRIVATE ${IOTC_C_LIB_INCLUDE_DIR} ${IOTC_C_SDK_INCLUDE_DIR} ${IOTC_C_SDK_CJSON_INCLUDE})

add_library(json_parser json_parser.c)
