LICENSE = "CLOSED"

FILEPATH = "${THISDIR}/files"

SRC_URI += "file://${THISDIR}/files/"

HOME_DIR = "${base_prefix}/home"

do_install () {
    bbplain "ADDING x509 CERTS"
    install -d ${D}${HOME_DIR}
    install -m 0755 ${FILEPATH}/client1.pem ${D}${HOME_DIR}
    install -m 0755 ${FILEPATH}/client1-key.pem ${D}${HOME_DIR}
}

FILES:${PN} += " \
    ${HOME_DIR}/client1.pem \
    ${HOME_DIR}/client1-key.pem \
"
