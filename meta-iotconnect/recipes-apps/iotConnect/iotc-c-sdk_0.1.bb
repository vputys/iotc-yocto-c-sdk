
#based on https://www.digikey.co.uk/en/maker/projects/intro-to-embedded-linux-part-6-add-custom-application-to-yocto-build/509191cec6c2418d88fd374f93ea5dda
SUMMARY = "Compile and install the basic-sample from the IoT Connect C SDK"
DESCRIPTION = "this recipe pulls the C SDK from the IoT Connect git repos; Sets up the basic-sample as the target to be built using CMake and then installed into the image."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit cmake

DEPENDS += " curl"
DEPENDS += " pkgconfig"
DEPENDS += " openssl"
DEPENDS += " util-linux"
RDEPENDS:${PN} += "systemd"

# Where to keep downloaded source files (in tmp/work/...)
C="${WORKDIR}/git"
S="${C}/samples/basic-sample"

SRCREV_FORMAT="machine_meta"
SRCREV="${AUTOREV}"
# Where to find source files (can be local, GitHub, etc.)

# This points to my branch of my fork of generic c sdk for now. TODO: change back when merged
SRC_URI = "git://github.com/avnet-iotconnect/iotc-generic-c-sdk.git;\
protocol=https;\
branch=feature/IOTSDK-226-cli-x509-json;\
destsuffix=${C};\
"

SRC_URI += "gitsm://github.com/DaveGamble/cJSON.git;\
protocol=https;\
branch=master;\
destsuffix=${C}/lib/cJSON/;\
"

SRC_URI += "gitsm://github.com/avnet-iotconnect/iotc-c-lib.git;\
protocol=https;\
branch=master;\
destsuffix=${C}/lib/iotc-c-lib/;\
"

SRC_URI += "gitsm://github.com/Azure/azure-iot-sdk-c.git;\
protocol=https;\
branch=main;\
destsuffix=${C}/lib/azure-iot-sdk-c/;\
"

SRC_URI += "gitsm://github.com/eclipse/paho.mqtt.c.git;\
protocol=https;\
branch=master;\
destsuffix=${C}/lib/paho.mqtt.c/;\
"

SRC_URI += "file://0001_CMake_findPackage.patch;\
patchdir=${C};\
"
cmake_do_generate_toolchain_file:append() {
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
