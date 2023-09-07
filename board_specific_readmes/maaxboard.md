## MaaxBoard Example
 
Based on [this quickstart guide](https://www.avnet.com/wps/wcm/connect/onesite/35645cc9-4317-4ca0-a2fa-30cce5f9ff17/MaaXBoard-Mini-Linux-Yocto-Lite-Development_Guide-V1.0-EN.pdf?MOD=AJPERES) from [this page](https://www.avnet.com/wps/portal/us/products/avnet-boards/avnet-board-families/maaxboard/maaxboard?utm_source=hackster)
      
This example has been tested on Ubuntu 20.04 without issue.

The final directory structure is shown below:
```bash
$ tree -L 2 imx-yocto-bsp/
imx-yocto-bsp/
├── maaxboard
│   └── build
└── sources
    ├── base
    ├── meta-browser
    ├── meta-clang
    ├── meta-freescale
    ├── meta-freescale-3rdparty
    ├── meta-freescale-distro
    ├── meta-imx
    ├── meta-iotconnect
    ├── meta-maaxboard
    ├── meta-myExampleIotconnectLayer
    ├── meta-nxp-demo-experience
    ├── meta-openembedded
    ├── meta-python2
    ├── meta-qt5
    ├── meta-timesys
    └── poky
```
<details>

<summary>Super-quickstart</summary>

```bash
sudo apt update && \
sudo apt install -y gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential \
chrpath socat libsdl1.2-dev xterm sed cvs subversion coreutils texi2html docbook-utils \
python-pysqlite2 help2man make gcc g++ desktop-file-utils libgl1-mesa-dev libglu1-mesa-dev \
mercurial autoconf automake groff curl lzop asciidoc u-boot-tools cpio sudo locales python && \
\
mkdir -p imx-yocto-bsp && \
cd imx-yocto-bsp && \
repo init -u https://github.com/nxp-imx/imx-manifest  -b imx-linux-kirkstone -m imx-5.15.71-2.2.0.xml && \
\
mkdir .repo/local_manifests && \
git clone git@github.com:pywtk/iotc-yocto-repo-manifests.git -b kirkstone --depth 1 && \
cp iotc-yocto-repo-manifests/iotc-c-maaxboard.xml .repo/local_manifests/ && \
repo sync && \
\
mkdir build && \
DISTRO=fsl-imx-wayland-lite MACHINE=maaxboard source imx-setup-release.sh -b build && \
bitbake-layers add-layer ../sources/meta-maaxboard \
../sources/meta-iotconnect \
../sources/meta-myExampleIotconnectLayer && \
\
echo 'DISTRO_FEATURES:append = " systemd"
VIRTUAL-RUNTIME_init_manager = "systemd"' >> conf/local.conf && \
echo 'INHERIT += "extrausers"
EXTRA_USERS_PARAMS = " useradd -p '\$5\$qjMzQ4sWI8S\$AJ/zzrYE2PJoyM9e6QLOV.L/xyDn0Lmk2E/aH4wd7o.' iot; \
                       usermod  -a -G sudo iot;"
IMAGE_INSTALL += " openssh"
IMAGE_INSTALL:append = " iotc-c-sdk"' >> conf/local.conf && \
\
bitbake core-image-minimal
```
</details>

1. Install required packages
   ```bash
   sudo apt update && \
   sudo apt install -y gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential
   chrpath socat libsdl1.2-dev xterm sed cvs subversion coreutils texi2html docbook-utils
   python-pysqlite2 help2man make gcc g++ desktop-file-utils libgl1-mesa-dev libglu1-mesa-dev
   mercurial autoconf automake groff curl lzop asciidoc u-boot-tools cpio sudo locales python
   ```
1. Install repo
   ```bash
   curl https://storage.googleapis.com/git-repo-downloads/repo > ./repo && \
   chmod a+x repo && \
   sudo mv repo /usr/bin/
   ```
1. Download meta layers from NXP using repo
   ```bash
   mkdir -p imx-yocto-bsp && \
   cd imx-yocto-bsp && \
   repo init -u https://github.com/nxp-imx/imx-manifest  -b imx-linux-kirkstone -m imx-5.10.35-2.0.0.xml && \
   repo sync
   ```

All instructions will take place from the `imx-yocto-bsp` directory unless stated otherwise.

1. Download Maaxboard sources
   ```bash
   git clone https://github.com/Avnet/meta-maaxboard.git -b kirkstone sources/meta-maaxboard
   ```
1. Agree to the license
   ```bash
   mkdir imx8mqevk && \
   DISTRO=fsl-imx-wayland MACHINE=imx8mqevk source imx-setup-release.sh -b imx8mqevk && \
   cd .. && \
   rm -rf imx8mqevk
   ```
1. Download this repo
   ```bash
   wget https://github.com/avnet-iotconnect/iotc-yocto-c-sdk/archive/refs/heads/main.zip && \
   unzip main.zip -d sources/ && \
   mv sources/iotc-yocto-c-sdk-main/meta-* sources/ && \
   rm -r main.zip sources/iotc-yocto-c-sdk-main/
   ```
1. Configure the build
   ```bash
   mkdir -p maaxboard/build && \
   source sources/poky/oe-init-build-env maaxboard/build
   ```
From the `imx-yocto-bsp/maaxboard/build` directory (which you should be in from the previous stage)
1. Use the build configuration templates from the MaaxBoard layer
   ```bash
   mkdir -p conf
   cp ../../sources/meta-maaxboard/conf/local.conf.sample ./conf/local.conf && \
   cp ../../sources/meta-maaxboard/conf/bblayers.conf.sample ./conf/bblayers.conf
   ```
1. Edit build configuration to include these layers
   ```bash
   echo -e '\nBBLAYERS += "${BSPDIR}/sources/meta-iotconnect"' >> conf/bblayers.conf && \
   echo 'BBLAYERS += "${BSPDIR}/sources/meta-myExampleIotconnectLayer"' >> conf/bblayers.conf
   ```
1. Add recipes to your image
   ```bash
   echo `\nIMAGE_INSTALL += " iotc-c-sdk" >> <your_image_path>.bb`
   ```
   or 
   ```bash
   echo `\nIMAGE_INSTALL += " iotc-c-sdk" >> conf/local.conf`
   ```
1. build!
   ```bash
   bitbake <your_image_name>
   ```
### Testing

Instructions for using a serial adapter and UART are found [here](https://www.hackster.io/monica/getting-started-with-maaxboard-headless-setup-24102b)  

If you haven't already added a user, you can add the default user `root` with password `avnet` to your `build/conf`

from the `imx-yocto-bsp` directory

   ```bash
echo -e '\nEXTRA_IMAGE_FEATURES=""
INHERIT += "extrausers"
EXTRA_USERS_PARAMS = "\ 
\tusermod -P avnet root; \ 
"' >> maaxboard/build/conf/local.conf 
   ```
