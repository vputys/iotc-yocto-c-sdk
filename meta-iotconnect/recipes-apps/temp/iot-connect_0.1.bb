#based on https://www.digikey.co.uk/en/maker/projects/intro-to-embedded-linux-part-6-add-custom-application-to-yocto-build/509191cec6c2418d88fd374f93ea5dda
SUMMARY = "Compile and install the basic-sample from the IoT Connect C SDK"
DESCRIPTION = "this recipe pulls the C SDK from the IoT Connect git repos; Sets up the basic-sample as the target to be built using CMake and then installed into the image."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit cmake

DEPENDS += " curl"
DEPENDS += " pkgconfig"
DEPENDS += " openssl"
DEPENDS += " util-linux-libuuid"

# Where to keep downloaded source files (in tmp/work/...)
C="${WORKDIR}/git"
S="${C}/samples/basic-sample"

SRCREV_FORMAT="machine_meta"

# Where to find source files (can be local, GitHub, etc.)
SRC_URI = "git://github.com/avnet-iotconnect/iotc-generic-c-sdk.git;\
protocol=https;\
branch=main;\
destsuffix=${C};\
rev=3cb38db05b2889562773e48b65527c389acefecb\
"

SRC_URI += "gitsm://github.com/DaveGamble/cJSON.git;\
protocol=https;\
branch=master;\
destsuffix=${C}/lib/cJSON/;\
rev=324a6ac9a9b285ff7a5a3e5b2071e3624b94f2db;\
"

SRC_URI += "gitsm://github.com/avnet-iotconnect/iotc-c-lib.git;\
protocol=https;\
branch=master;\
destsuffix=${C}/lib/iotc-c-lib/;\
rev=5c5e98144651a0a9b856944ce967b2073c36b19a;\
"

SRC_URI += "gitsm://github.com/Azure/azure-iot-sdk-c.git;\
protocol=https;\
branch=main;\
destsuffix=${C}/lib/azure-iot-sdk-c/;\
rev=80d4e13ce7cdc3ef1751848e649d653d485f8412;\
"

SRC_URI += "gitsm://github.com/eclipse/paho.mqtt.c.git;\
protocol=https;\
branch=master;\
destsuffix=${C}/lib/paho.mqtt.c/;\
rev=3b7ae6348bc917d42c04efa962e4868c09bbde9f;\
"

SRC_URI += "file://0001_CMake_findPackage.patch;\
patchdir=${C};\
"

SRC_URI += "file://0002-app-config.patch;\
patchdir=${C};\
"

cmake_do_generate_toolchain_file_append() {
	cat >> ${WORKDIR}/toolchain.cmake <<EOF
$cmake_crosscompiling

set( PC_CURL_LIBRARY_DIRS "${STAGING_LIBDIR}")

#find_package(CURL REQUIRED)
EOF
}


# Create /usr/bin in rootfs and copy program to it
do_install() {
    install -d ${D}${bindir}
    install -m 0755 basic-sample ${D}${bindir}
    install -d ${D}${sysconfdir}/ssl/certs
    install -m 0755 ${S}/certs/server.pem ${D}${sysconfdir}/ssl/certs/
}
