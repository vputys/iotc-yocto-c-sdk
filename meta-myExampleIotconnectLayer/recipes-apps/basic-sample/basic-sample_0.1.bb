SUMMARY = "Recipe that compiles and basic-sample"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit cmake

DEPENDS += " iotc-c-sdk"



SRCREV_FORMAT="machine_meta"
SRCREV="${AUTOREV}"

#FILEPATH = "${THISDIR}/files"

HOME_DIR = "${base_prefix}/home"


SRC_URI += "gitsm://github.com/DaveGamble/cJSON.git;\
protocol=https;\
branch=master;\
destsuffix=${C}/lib/cJSON/;\
"
S="${WORKDIR}/src"

SRC_URI = "file://main.c;\
subdir=${S}; \
file://CMakeLists.txt;\
subdir=${S}; \
file://config/app_config.h;\
subdir=${S}"

cmake_do_generate_toolchain_file:append() {
	cat >> ${WORKDIR}/toolchain.cmake <<EOF
$cmake_crosscompiling

set( PC_CURL_LIBRARY_DIRS "${STAGING_LIBDIR}")
set( IOTC_C_LIB_INCLUDE_DIR  "${STAGING_BASELIBDIR}/iotc-c-lib/include")
set( IOTC_C_SDK_DIR  "${STAGING_DIR_TARGET}/iotc-generic-c-sdk")
set( IOTC_C_SDK_INCLUDE_DIR  "${STAGING_DIR_TARGET}/iotc-generic-c-sdk/include")
#find_package(CURL REQUIRED)
EOF
}


do_install() {
    install -d ${D}${bindir}
    
    install -m 0755 basic-sample-test ${D}${bindir}

}
