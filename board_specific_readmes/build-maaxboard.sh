#!/bin/bash
sudo apt update
sudo apt install -y gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential \
chrpath socat libsdl1.2-dev xterm sed cvs subversion coreutils texi2html docbook-utils \
python-pysqlite2 help2man make gcc g++ desktop-file-utils libgl1-mesa-dev libglu1-mesa-dev \
mercurial autoconf automake groff curl lzop asciidoc u-boot-tools cpio sudo locales python

mkdir -p imx-yocto-bsp
cd imx-yocto-bsp
repo init -u https://github.com/nxp-imx/imx-manifest  -b imx-linux-kirkstone -m imx-5.15.71-2.2.0.xml

mkdir .repo/local_manifests
git clone git@github.com:pywtk/iotc-yocto-repo-manifests.git -b kirkstone --depth 1
cp iotc-yocto-repo-manifests/iotc-c-maaxboard.xml .repo/local_manifests/
repo sync

mkdir build
DISTRO=fsl-imx-wayland-lite MACHINE=maaxboard source imx-setup-release.sh -b build
bitbake-layers add-layer ../sources/meta-maaxboard \
../sources/meta-iotconnect \
../sources/meta-myExampleIotconnectLayer

echo 'DISTRO_FEATURES:append = " systemd"
VIRTUAL-RUNTIME_init_manager = "systemd"' >> conf/local.conf
echo 'INHERIT += "extrausers"
EXTRA_USERS_PARAMS = " useradd -p '\$5\$qjMzQ4sWI8S\$AJ/zzrYE2PJoyM9e6QLOV.L/xyDn0Lmk2E/aH4wd7o.' iot; \
                       usermod  -a -G sudo iot;"
IMAGE_INSTALL += " openssh"
IMAGE_INSTALL:append = " iotc-c-sdk"' >> conf/local.conf

echo 'DL_DIR = "/home/py/yocto-downloads"' >> conf/local.conf

bitbake core-image-minimal

