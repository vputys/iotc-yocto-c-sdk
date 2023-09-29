SUMMARY = "Recipe that compiles and basic-sample"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit cmake

#DEPENDS += " curl"
#DEPENDS += " pkgconfig"
#DEPENDS += " openssl"
#DEPENDS += " util-linux"
DEPENDS += " iotc-c-sdk"
#RDEPENDS:${PN} += "iotc-c-sdk"

# why is this here? it doesn't do anything anyways?
#RDEPENDS:${PN} += "systemd"

# Where to keep downloaded source files (in tmp/work/...)
#C="${WORKDIR}/git"
#S="${C}/samples/basic-sample"

SRCREV_FORMAT="machine_meta"
SRCREV="${AUTOREV}"



#FILEPATH = "${THISDIR}/files"

#SRC_URI += "file://${THISDIR}/files/"

HOME_DIR = "${base_prefix}/home"


SRC_URI += "gitsm://github.com/DaveGamble/cJSON.git;\
protocol=https;\
branch=master;\
destsuffix=${C}/lib/cJSON/;\
"

#SRC_URI += "gitsm://github.com/avnet-iotconnect/iotc-c-lib.git;\
#protocol=https;\
#branch=master;\
#destsuffix=${C}/lib/iotc-c-lib/;\
#"

#SRC_URI += "gitsm://github.com/Azure/azure-iot-sdk-c.git;\
#protocol=https;\
#branch=main;\
#destsuffix=${C}/lib/azure-iot-sdk-c/;\
#"

#SRC_URI += "gitsm://github.com/eclipse/paho.mqtt.c.git;\
#protocol=https;\
#branch=master;\
#destsuffix=${C}/lib/paho.mqtt.c/;\
#"

#SRC_URI += "file://0001_CMake_findPackage.patch;\
#patchdir=${C};\
#"

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


# Create /usr/bin in rootfs and copy program to it
do_install() {
    install -d ${D}${bindir}
    
    # that's from adding-x509-certs-c-sdk recipe
#    bbplain "ADDING x509 CERTS. IOTC C"
#    install -d ${D}${HOME_DIR}
#    install -m 0755 ${FILEPATH}/client1.pem ${D}${HOME_DIR}
#    install -m 0755 ${FILEPATH}/client1-key.pem ${D}${HOME_DIR}
    
    install -m 0755 basic-sample-test ${D}${bindir}
#    install -d ${D}${sysconfdir}/ssl/certs
#    install -m 0755 ${S}/certs/server.pem ${D}${sysconfdir}/ssl/certs/
}


FILES:${PN} += " \
    ${HOME_DIR}/client1.pem \
    ${HOME_DIR}/client1-key.pem \
"
