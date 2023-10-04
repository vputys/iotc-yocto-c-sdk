SUMMARY = "Recipe that compiles and deploys a telemetry demo"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit cmake

DEPENDS += " iotc-c-sdk"

PACKAGES = "${PN} ${PN}-dev ${PN}-dbg ${PN}-staticdev"
PROVIDES = "${PN} ${PN}-dev"

PRIVATE_DATA_DIR = "${base_prefix}/usr/local/iotc"

FILES:${PN}-dev = "${PRIVATE_DATA_DIR}/* \
"

SRCREV_FORMAT="machine_meta"
SRCREV="${AUTOREV}"

SRC_URI += "gitsm://github.com/DaveGamble/cJSON.git;\
protocol=https;\
branch=master;\
destsuffix=${C}/lib/cJSON/;\
"
S="${WORKDIR}/cmke-src"

SRC_URI = "file://cmke-src; \
file://eg-private-repo-data \
"

cmake_do_generate_toolchain_file:append() {
	cat >> ${WORKDIR}/toolchain.cmake <<EOF
$cmake_crosscompiling

set( PC_CURL_LIBRARY_DIRS "${STAGING_LIBDIR}")
set( IOTC_C_LIB_INCLUDE_DIR  "${STAGING_BASELIBDIR}/iotc-c-lib/include")
set( IOTC_C_SDK_DIR  "${STAGING_DIR_TARGET}/iotc-generic-c-sdk")
set( IOTC_C_SDK_INCLUDE_DIR  "${STAGING_DIR_TARGET}/iotc-generic-c-sdk/include")
set( IOTC_C_SDK_CJSON_INCLUDE "${STAGING_BASELIBDIR}/cJSON")
#find_package(CURL REQUIRED)
EOF
}


do_install() {
    install -d ${D}${bindir}
    install -m 0755 telemetry-demo ${D}${bindir}

    for f in ${WORKDIR}/eg-private-repo-data/*
    do
        if [ -f $f ]; then
            if [ ! -d ${D}${PRIVATE_DATA_DIR} ]; then
                install -d ${D}${PRIVATE_DATA_DIR}
            fi
            install -m 0755 $f ${D}${PRIVATE_DATA_DIR}/
        fi
    done
}
