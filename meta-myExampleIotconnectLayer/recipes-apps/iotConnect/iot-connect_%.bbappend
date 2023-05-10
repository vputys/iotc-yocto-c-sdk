FILESEXTRAPATHS_prepend := "${THISDIR}:"

SRC_URI += "file://app_config.h;\
subdir=${S}/config;\
"
