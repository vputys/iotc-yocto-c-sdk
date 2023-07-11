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

### Interaction with Avnet Embedded

***Note***: Tested on docker image of Ubuntu 18.04 in combination with Ubuntu 22.04.

create an account on embedded.avnet.com

download the msc-ldk from this page https://embedded.avnet.com/product/msc-sm2s-imx8plus/#manual (this was the latest at the time of writing "msc-ldk-bsp-01047-v1.9.0-20220909.pdf"). You might also need documents from https://embedded.avnet.com/product/msc-sm2s-imx8plus/

***Note:*** [List of useful documentation](#msc-ldk-documentation)

As soon as you get access to embedded.avnet.com find a document named *"msc-ldk-bsp-01047-v1.9.0-20220909"* and *"App_Note_030_Building_from_MSC_Git_V1_8"* and do the RSA SSH key bit mentioned in both documents then send it to address provided in those manuals along with the project code you want to work on (*msc_01047* for SM2S-IMX8PLUS). Do it ASAP too because it'll block you from downloading sm2s-imx8plus bsp and, thus, next steps.

- ***Note:*** Do these previous steps as soon as possible as they will be blocking you from continuing work.

follow instructions in said guide (*"msc-ldk-bsp-01047-v1.9.0-20220909"*) to establish the build enviroment. (Up to section  4.7 "Building Images" (not including 4.7) at the time of writing.) 

### Building MSC LDK

***IMPORTANT:*** do not use *"App_Note_030_Building_from_MSC_Git_V1_8"* as guide for building images. It provides information on building generic image and skips an important step mentioned in another document. Problem is - iotc-yocto-c-sdk uses hardknott Yocto and generic MSC LDK uses kirkstone Yocto and require manual fixes to make them compatible. Please use *"msc-ldk-bsp-01047-v1.9.0-20220909"* to build images. If compatibility issues still appear - do `git checkout v1.9.0` before setting up the enviroment or building OR [look for manual fixes here](#tweaking-iotc-yocto-c-sdk-to-work-with-yocto-kirkstone).

***Also note:*** *"msc-ldk-bsp-01047-v1.9.0-20220909"* document in Section 4.3 makes a typo. `git checkout 1.9.0` must be `git checkout v1.9.0`.


#### Docker build

##### Getting docker

***Note:*** for this to work you might need to follow steps provided in Section 4.2 in *"msc-ldk-bsp-01047-v1.9.0-20220909"* document (Section "Setup Optional Docker Container") or combinating native build with your own Dockerfile.

This was the docker run used in the first instance:

- `docker run --privileged -t -i --dns <your networks gateway IP here>  --name msc-ldk -h docker -v `pwd`/src:/src msc-ldk /bin/bash`

##### Seting up

Follow the instructions (*"msc-ldk-bsp-01047-v1.9.0-20220909"*) in section 4.7 (Building Images Section)

Building will take a while and at the end you will get a lot of built images. Make sure `imx-boot-sm2s-imx8mp-sd.bin-flash_evk` is built.

##### Exiting and stopping docker

- - NB: you can exit the docker enviroment as you would any bash session with exit. Bear in mind that to re-enter the enviroment you'll need to call the following;

```
    docker stop msc-ldk && \
    docker rm msc-ldk
```

If you didn't change strings following `-v` option in the docker command above you should find cloned directories and built images (`build/01047/tmp/deploy/images/sm2s-imx8mp/`) under src directory in your current directory.


#### Native build

Follow the instructions (*"msc-ldk-bsp-01047-v1.9.0-20220909"*) in section 4.7 (Building Images Section)

Building will take a while and at the end you will get a lot of built images. Make sure `imx-boot-sm2s-imx8mp-sd.bin-flash_evk` is built.

### Building `iot-connect-image` for sm2s-imx8mp

clone iotc-yocto-c-sdk into `<basedir>/source`.

Add both layers from iotc-yocto-c-sdk to the `bblayers.conf` file located in `build/01047/conf`

***Note:*** ioct-yocto-c-sdk is based on hardknott version of Yocto. If kirkstone is required please look at [this section](#tweaking-iotc-yocto-c-sdk-to-work-with-yocto-kirkstone)

navigate to `build/01047` and use `bitbake iot-connect-image`

After building make sure `iot-connect-image-sm2s-imx8mp.wic` is actually built

### Flashing

***Note:*** if build enviroment is not set up - head to base directory and run `source sources/yocto.git/oe-init-build-env build/01047`

Once the layers have been integrated as per the instructions above & recipe iot-connect-image has been built it can be flashed to the target using:

- ***NOTE:*** before flashing you need to see Section 6.16 (at the time of writing) Boot Options (Also see *"App_Note_035_Using_NXP_Mfgtool+uuu"* document Section 3.3 and further). It has information on what needs to be done to enable flashing via USB cable (you'll need to bridge 2 pins on the back of the board on board startup). In also contains information about DIP switches you might need to configure to enable right boot device (*"App_Note_035_Using_NXP_Mfgtool+uuu"* does not provide information on DIP switches so refer to *"MSC-SM2S-MB-EP5_User-Manual_DV3_V013"* document Section 3.16 (Boot Selection) or *"MSC_SM2S-IMX8PLUS_Manual"* Section 6.16 (Boot Options)). 

- ***Note:*** emmc_burn_all.lst and emmc_burn_loader.lst can be found in an Appendix section of *"App_Note_035_Using_NXP_Mfgtool+uuu"* document. files there contain some formatting as well as syntax problems not allowing to flash. However, they should be relatively easy to fix.

    ```
    sudo uuu -b emmc_burn_all.lst \
    <path-to>/imx-boot-sm2s-imx8mp-sd.bin-flash_evk \
    <path-to>/iot-connect-image-sm2s-imx8mp.wic
    ```

    where might resolve as

    ```
    sudo uuu -b emmc_burn_all.lst \
    ../../docker-msc-ldk/src/msc-ldk/build/01047/tmp/deploy/images/sm2s-imx8mp/imx-boot-sm2s-imx8mp-sd.bin-flash_evk \
    ../../docker-msc-ldk/src/msc-ldk/build/01047/tmp/deploy/images/sm2s-imx8mp/iot-connect-image-sm2s-imx8mp.wic
    ```
On the target (via serial debug or ssh terminal (for serial debug please see Section 3.10 RS485 / RS232 (SER0) in *"MSC-SM2S-MB-EP5_User-Manual_DV3_V013"* )) execute `basic-sample`



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
   
<details>
   <summary>Quickstart</summary>
   
   1. Setup your build environment if you haven't already
   
      ```
      sudo apt update && \
      sudo apt install -y gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential \
      chrpath socat libsdl1.2-dev xterm sed cvs subversion coreutils texi2html docbook-utils \
      python-pysqlite2 help2man make gcc g++ desktop-file-utils libgl1-mesa-dev libglu1-mesa-dev \
      mercurial autoconf automake groff curl lzop asciidoc u-boot-tools cpio sudo locales && \
      curl https://storage.googleapis.com/git-repo-downloads/repo > ./repo && \
      chmod a+x repo && \
      sudo mv repo /usr/bin/
      ```
   
   1. cut & paste the below into your working directory
      
       ```
       mkdir -p imx-yocto-bsp && \
       cd imx-yocto-bsp && \
       repo init -u https://github.com/nxp-imx/imx-manifest  -b imx-linux-hardknott -m imx-5.10.35-2.0.0.xml && \
       repo sync && \
       git -C sources clone https://github.com/Avnet/meta-maaxboard.git -b hardknott && \
       mkdir imx8mqevk && \
       DISTRO=fsl-imx-wayland MACHINE=imx8mqevk source imx-setup-release.sh -b imx8mqevk && \
       cd .. && rm -rf imx8mqevk && \
       mkdir -p maaxboard/build && \
       source sources/poky/oe-init-build-env maaxboard/build && \
       cd ../../ && \
       cp sources/meta-maaxboard/conf/local.conf.sample maaxboard/build/conf/local.conf && \
       cp sources/meta-maaxboard/conf/bblayers.conf.sample maaxboard/build/conf/bblayers.conf && \
       wget https://github.com/avnet-iotconnect/iotc-yocto-c-sdk/archive/refs/heads/main.zip && \
       unzip main.zip -d sources/ && \
       mv sources/iotc-yocto-c-sdk-main/meta-* sources/ && \
       rm -r main.zip sources/iotc-yocto-c-sdk-main/ && \
       echo -e '\nBBLAYERS += "${BSPDIR}/sources/meta-iotconnect"' >> maaxboard/build/conf/bblayers.conf && \
       echo 'BBLAYERS += "${BSPDIR}/sources/meta-myExampleIotconnectLayer"' >> maaxboard/build/conf/bblayers.conf && \
       bitbake iot-connect-image
      ```
</details>

This example has been tested on Ubuntu 20.04 without issue.

The final directory structure is shown below
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
   repo init -u https://github.com/nxp-imx/imx-manifest  -b imx-linux-hardknott -m imx-5.10.35-2.0.0.xml && \
   repo sync
   ```

All instructions will take place from the `imx-yocto-bsp` directory unless stated otherwise.

1. Download Maaxboard sources
   ```bash
   git clone https://github.com/Avnet/meta-maaxboard.git -b hardknott sources/meta-maaxboard
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

1. Add default user `root` with password `avnet` for iot-connect-base-image
   ```bash
   echo -e 'EXTRA_USERS_PARAMS = "\ \n\tusermod -P avnet root; \ \n"' >> sources/meta-iotconnect/recipes-core/images/iot-connect-image.bb 
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
1. build!
   ```bash
   bitbake iot-connect-image
   ```
Testing instructions for using a serial adapter and UART are found [here](https://www.hackster.io/monica/getting-started-with-maaxboard-headless-setup-24102b)  


## Appendix


### MSC LDK Documentation

Available from: https://embedded.avnet.com/product/msc-sm2s-imx8plus/#manual

- *MSC_SM2S-IMX8PLUS_Manual*
- *msc-ldk-bsp-01047-v1.9.0-20220909* (information on getting, cloning and building msc ldk images)
- *App_Note_035_Using_NXP_Mfgtool+uuu* (important information on flashing and boot select)
- *App_Note_030_Building_from_MSC_Git_V1_8* (a bit clearer document than *"msc-ldk-bsp-01047-v1.9.0-20220909"*, however misses important step with `git checkout v1.9.0` after cloning initial repo)
- *MSC-SM2S-MB-EP5_User-Manual_DV3_V013* (can be found here: https://embedded.avnet.com/product/msc-sm2s-mb-ep5/#manual (also requires Avnet Embedded account for it))

### Tweaking iotc-yocto-c-sdk to work with yocto kirkstone

#### meta-iotconnect

##### layer.conf

in `meta-iotconnect/conf/layer.conf`:

- change `LAYERSERIER_COMPAT_meta-iotconnect` to `LAYERSERIES_COMPAT_meta-iotconnect = "kirkstone"`

##### iot-connect_%.bb

in `meta-iotconnect/recipes-apps/iotConnect/iot-connect_0.1.bb` (v0.1 at the time of writing):

- change `cmake_do_generate_toolchain_file_append()` to `cmake_do_generate_toolchain_file:append()`

#### meta-myExampleIoTConnectLayer

##### layer.conf

in `meta-myExampleIoTConnectLayer/conf/layer.conf`:
- change `LAYERSERIES_COMPAT_meta-myExampleIoTConnectLayer` to `LAYERSERIES_COMPAT_meta-myExampleIotconnectLayer = "kirkstone"`

##### iot-connect_%.bbappend

in `meta-myExampleIoTConnectLayer/recipes-apps/iotConnect/iot-connect_%.bbappen`:
- change `FILESEXTRAPATHS_prepend := "${THISDIR}:"` to `FILESEXTRAPATHS:prepend := "${THISDIR}:"`