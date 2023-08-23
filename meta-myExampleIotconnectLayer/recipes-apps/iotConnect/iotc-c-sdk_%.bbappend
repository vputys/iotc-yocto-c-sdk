FILESEXTRAPATHS:prepend := "${THISDIR}:"

SRC_URI += "file://app_config.h;\
subdir=${S}/symmkey;\
"

SRC_URI += "file://x509_config/app_config.h;\
subdir=${S}/;\
"

RDEPENDS:${PN} += " adding-x509-certs-c-sdk"

do_configure:append() {
    install -d {S}/x509_config
    install -d {S}/symmkey
}

do_install:append() {
    install -m 0755 basic-sample-x509 ${D}${bindir}
}