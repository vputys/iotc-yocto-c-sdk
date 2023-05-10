SUMMARY = "My IoT Connect image"

CORE_IMAGE_EXTRA_INSTALL += " kernel-modules"

IMAGE_INSTALL += " ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_INSTALL += " curl"
IMAGE_INSTALL += " packagegroup-core-boot"
IMAGE_INSTALL += " iot-connect"

IMAGE_FEATURES += "splash"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit module
inherit extrausers
