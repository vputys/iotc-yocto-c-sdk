FILESEXTRAPATHS:prepend := "${THISDIR}:"


SRC_URI += "file://app_config.h;\
subdir=${S};\
"

SRC_URI += "file://app_config.h;\
subdir=${C}/samples/cli-json-sample/symmkey;\
"


SRC_URI += "file://x509_config/app_config.h;\
subdir=${C}/samples/cli-json-sample;\
"

CLI_JSON_S="${C}/samples/cli-json-sample"

RDEPENDS:${PN} += " adding-x509-certs-c-sdk"



do_configure:append() {
    install -d ${CLI_JSON_S}/x509_config
    install -d ${CLI_JSON_S}/symmkey
}

do_compile:append(){
    
    install -d ${CLI_JSON_S}/build
    
    cd ${CLI_JSON_S}/build
    cmake .. --toolchain ${WORKDIR}/toolchain.cmake
    
    cmake --build ${CLI_JSON_S}/build --target cli-json-sample
    cmake --build ${CLI_JSON_S}/build --target cli-json-sample-x509
     
    cd -
}

do_install:append() {
    install -d ${D}${bindir}
    install -m 0755 ${CLI_JSON_S}/build/cli-json-sample ${D}${bindir}
    install -m 0755 ${CLI_JSON_S}/build/cli-json-sample-x509 ${D}${bindir}
}