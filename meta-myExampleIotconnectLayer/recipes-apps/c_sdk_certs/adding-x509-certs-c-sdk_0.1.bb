LICENSE = "CLOSED"

FILEPATH = "${THISDIR}/files"

SRC_URI += "file://${THISDIR}/files/"

HOME_DIR = "${base_prefix}/home/vlad"

do_install () {
    install -d ${D}${HOME_DIR}
    install -m 0755 ${FILEPATH}/x509device-crt.pem ${D}${HOME_DIR}
    install -m 0755 ${FILEPATH}/x509device-key.pem ${D}${HOME_DIR}
    install -m 0755 ${FILEPATH}/client1.pem ${D}${HOME_DIR}
    install -m 0755 ${FILEPATH}/rootCA.pem ${D}${HOME_DIR}
    install -m 0755 ${FILEPATH}/client1-key.pem ${D}${HOME_DIR}
}

FILES:${PN} += " \
    ${HOME_DIR}/x509device-crt.pem \
    ${HOME_DIR}/x509device-key.pem \
    ${HOME_DIR}/client1.pem \
    ${HOME_DIR}/rootCA.pem \
    ${HOME_DIR}/client1-key.pem \
"
