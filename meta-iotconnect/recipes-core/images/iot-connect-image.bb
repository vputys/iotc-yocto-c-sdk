SUMMARY = "My IoT Connect image"

CORE_IMAGE_EXTRA_INSTALL += " kernel-modules"

IMAGE_INSTALL += " ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_INSTALL += " curl"
IMAGE_INSTALL += " packagegroup-core-boot"
IMAGE_INSTALL += " openssh"
IMAGE_INSTALL += " iot-connect"

IMAGE_FEATURES += "splash"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit module
inherit extrausers
EXTRA_USERS_PARAMS = " useradd iot; \
                       usermod  -p 'iot' iot; \
                       usermod  -a -G sudo iot;"
