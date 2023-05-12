# IoT-Connect Yocto Integration

*The following details yocto layers designed to integrate the [IoT-Connect C SDK](https://github.com/avnet-iotconnect/iotc-generic-c-sdk). The end result is a compiled version of the `basic-sample` app provided in the C SDK repo that is installed to an image target. Once said image is flashed to a target, the basic-sample app should successfully run & establish comms with an appropriately setup https://avnet.iotconnect.io/*

## Layers
There are 2 layers thus far: `meta-iotconnect` & `meta-myExampleIotconnectLayer`.
### How to include layers
To include the layers within a yocto enviroment:

1. check them out to the `sources` directory in your yocto enviroment.

1. add them to `conf/bblayers` file in your build directory

1. build with a bitbake call e.g. `./bitbake iot-connect-image`

### Description of layers
#### meta-iotconnect
Contains the git urls for checkouts, recipe definitions & a patch file for temporarily fixing the CMake `find_package` issue. This layer sets the build target to `basic-sample`. This might get further subdivided in the future to only provide IoT-Connect libraries to other layers.
#### meta-myExampleIotconnectLayer
It's expected that developers will have to provide bespoke elements for their application. In the `basic-sample` example you're required to edit `app-config.h`. This layer provides an example of how a user might specify custom requirements of an application within their layer that's then compiled & built for use: in this case the `app-config.h` specifics.
## MSC LDK Example

1. create an account on [embedded.avnet.com](https://embedded.avnet.com) 

1. download the msc-ldk from this page https://embedded.avnet.com/product/msc-sm2s-imx8plus/#manual (this was the latest at the time of writing "msc-ldk-bsp-01047-v1.9.0-20220909.pdf"

1. follow instructions in said guide to establish the build enviroment. (Up to section 4.8 at the time of writing.) This was the docker rune used in the first instance:
   ```
   docker run --privileged -t -i --dns <your networks gateway IP here>  --name msc-ldk -h docker -v `pwd`/src:/src msc-ldk /bin/bash
   ```
   > *NB: you can exit the docker enviroment as you would any bash session with `exit`. Bear in mind that to re-enter the enviroment you'll need to call  the following;*
   ```
   docker stop msc-ldk && \
   docker rm msc-ldk
   ```

1. Once the layers have been integrated as per the instructions above & recipe `iot-connect-image` has been built it can be flashed to the target using:
   ```
   sudo uuu -b emmc_burn_all.lst \
   <path-to>/imx-boot-sm2s-imx8mp-sd.bin-flash_evk \
   <path-to>/iot-connect-image-sm2s-imx8mp.wic
   ```
   where <path-to> might resolve as
   ```
   sudo uuu -b emmc_burn_all.lst \
   ../../docker-msc-ldk/src/msc-ldk/build/01047/tmp/deploy/images/sm2s-imx8mp/imx-boot-sm2s-imx8mp-sd.bin-flash_evk \
   ../../docker-msc-ldk/src/msc-ldk/build/01047/tmp/deploy/images/sm2s-imx8mp/iot-connect-image-sm2s-imx8mp.wic
   ```

1. on the target (via serial debug or ssh terminal) execute `basic-sample`

## If in doubt, you may need...

* App_Note_030_Building_from_MSC_Git_V1_7.pdf *(Application Notes from [here] (https://embedded.avnet.com/product/msc-sm2s-imx8/#application_notes))*
* App_Note_040_DTB_File_Selection_v20.pdf
* User privileges for the sample images from embedded.avnet:

 | User | Password |
 | ---- | :------: |
 | msc  | msc      |
 | root | mscldk   |

* this email address for the great help from support.boards@avnet.eu
 
 ## MaaxBoard Example
 
 Based on [this quickstart guide](https://www.avnet.com/wps/wcm/connect/onesite/35645cc9-4317-4ca0-a2fa-30cce5f9ff17/MaaXBoard-Mini-Linux-Yocto-Lite-Development_Guide-V1.0-EN.pdf?MOD=AJPERES) from [this page](https://www.avnet.com/wps/portal/us/products/avnet-boards/avnet-board-families/maaxboard/maaxboard?utm_source=hackster)
 
1. Install required packages
   ```
   sudo apt update && \
   sudo apt install -y gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential
   chrpath socat libsdl1.2-dev xterm sed cvs subversion coreutils texi2html docbook-utils
   python-pysqlite2 help2man make gcc g++ desktop-file-utils libgl1-mesa-dev libglu1-mesa-dev
   mercurial autoconf automake groff curl lzop asciidoc u-boot-tools cpio sudo locales
   ```
1. Install repo
   ```
   curl https://storage.googleapis.com/git-repo-downloads/repo > ./repo && \
   chmod a+x repo && \
   sudo mv repo /usr/bin/
   ```
1. Download meta layers from NXP using repo
   ```
   mkdir -p imx-yocto-bsp && \
   cd imx-yocto-bsp && \
   repo init -u https://github.com/nxp-imx/imx-manifest  -b imx-linux-hardknott -m imx-5.10.35-2.0.0.xml && \
   repo sync
   ```
1. Download Maaxboard sources
   ```
   git clone https://github.com/Avnet/meta-maaxboard.git -b hardknott
   ```
1. Agree to the license
   ```
   mkdir imx8mqevk && \
   DISTRO=fsl-imx-wayland MACHINE=imx8mqevk source imx-setup-release.sh -b imx8mqevk && \
   rm -rf imx8mqevk
   ```
1. Download this repo
   ```
   wget https://github.com/avnet-iotconnect/iotc-yocto-c-sdk/archive/refs/heads/main.zip && \
   unzip main.zip -d sources/ && \
   mv sources/iotc-yocto-c-sdk-main/meta-* sources/ && \
   rm -r main.zip sources/iotc-yocto-c-sdk-main/
   ```
1. Configure the build
   ```
   mkdir -p maaxboard/build && \
   source sources/poky/oe-init-build-env maaxboard/build
   ```
1. Use the build configuration templates from the MaaxBoard layer
   ```
   cp imx-yocto-bsp/sources/meta-maaxboard/conf/local.conf.sample imx-yocto-bsp/maaxboard/build/conf/local.conf && \
   cp imx-yocto-bsp/sources/meta-maaxboard/conf/bblayers.conf.sample imx-yocto-bsp/maaxboard/build/conf/bblayers.conf
   ```
1. Edit build configuration to include these layers
   ```
   echo -e '\nBBLAYERS += "${BSPDIR}/sources/meta-iotconnect"' >> maaxboard/build/conf/bblayers.conf && \
   echo 'BBLAYERS += "${BSPDIR}/sources/meta-myExampleIotconnectLayer"' >> maaxboard/build/conf/bblayers.conf
   ```
1. build!
   ```
   bitbake iot-connect-image
   ```
