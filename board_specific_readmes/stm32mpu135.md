# Quickstart instructions

<details>

<summary>Super-quickstart</summary>

`mkdir` at your prefferred location, `cd` into it then copy/paste the following into your terminal.

``` bash
repo init https://github.com/STMicroelectronics/oe-manifest -b kirkstone && \
mkdir .repo/local_manifests && \
git clone git@github.com:pywtk/iotc-yocto-repo-manifests.git -b kirkstone --depth 1 && \
cp iotc-yocto-repo-manifests/iotc-c-stm32mpu1.xml .repo/local_manifests/iotc-c-stm32mpu1.xml && \
repo sync && \
source layers/openembedded-core/oe-init-build-env build && \
bitbake-layers add-layer ../layers/meta-openembedded/meta-oe/ \
../layers/meta-openembedded/meta-python \
../layers/meta-st/meta-st-stm32mp \
../layers/meta-iotconnect \
../layers/meta-myExampleIotconnectLayer && \
echo 'MACHINE="stm32mp13-disco"' >> conf/local.conf && \
echo 'DISTRO_FEATURES:append = " systemd"
VIRTUAL-RUNTIME_init_manager = "systemd"' >> conf/local.conf && \
echo 'INHERIT += "extrausers"
EXTRA_USERS_PARAMS = " useradd -p '\$5\$qjMzQ4sWI8S\$AJ/zzrYE2PJoyM9e6QLOV.L/xyDn0Lmk2E/aH4wd7o.' iot; \
                       usermod  -a -G sudo iot;"
IMAGE_INSTALL += " openssh"
IMAGE_INSTALL:append = " iotc-c-sdk"' >> conf/local.conf && \
bitbake core-image-minimal && \
tmp-glibc/deploy/images/stm32mp13-disco/scripts/create_sdcard_from_flashlayout.sh tmp-glibc/deploy/images/stm32mp13-disco/flashlayout_core-image-minimal/optee/FlashLayout_sdcard_stm32mp135f-dk-optee.tsv && \
sudo dd if=tmp-glibc/deploy/images/stm32mp13-disco/flashlayout_core-image-minimal/optee/../../FlashLayout_sdcard_stm32mp135f-dk-optee.raw of=/dev/mmcblk0 bs=8M conv=fdatasync status=progress && sudo sync
```
</details>

## Create your working dir
e.g.
```bash
mkdir /tmp/st; cd /tmp/st
```
## Prepare to collect ST's sources
``` bash
repo init https://github.com/STMicroelectronics/oe-manifest -b kirkstone
```
## Prepare to also collect this repo's sources
``` bash
mkdir .repo/local_manifests && \
git clone git@github.com:pywtk/iotc-yocto-repo-manifests.git -b kirkstone --depth 1 && \
cp iotc-yocto-repo-manifests/iotc-c-stm32mpu1.xml .repo/local_manifests/iotc-c-stm32mpu1.xml
```
## Download all sources
``` bash
repo sync
```
## Instantiate your build environment
``` bash
source layers/openembedded-core/oe-init-build-env build
```
## Add Layers
``` bash
bitbake-layers add-layer ../layers/meta-openembedded/meta-oe/ \
../layers/meta-openembedded/meta-python \
../layers/meta-st/meta-st-stm32mp \
../layers/meta-iotconnect \
../layers/meta-myExampleIotconnectLayer
```
## Declare which ST platform should be built
``` bash
echo 'MACHINE="stm32mp13-disco"' >> conf/local.conf
```
## Make the required edits to the DISTRO_FEATURES
``` bash
echo 'DISTRO_FEATURES:append = " systemd"
VIRTUAL-RUNTIME_init_manager = "systemd"' >> conf/local.conf
```
## Add the SDK to the image
``` bash
echo 'IMAGE_INSTALL:append = " iotc-c-sdk"' >> conf/local.conf
```
## Add some local settings for accessability.
``` bash
echo 'EXTRA_USERS_PARAMS = " useradd -p '\$5\$qjMzQ4sWI8S\$AJ/zzrYE2PJoyM9e6QLOV.L/xyDn0Lmk2E/aH4wd7o.' iot; \
                       usermod  -a -G sudo iot;"
IMAGE_INSTALL += " openssh"' >> conf/local.conf
```
## Compile & make the image
``` bash
bitbake core-image-minimal
```
## Create the SD Card image from the yocto outputs
``` bash
tmp-glibc/deploy/images/stm32mp13-disco/scripts/create_sdcard_from_flashlayout.sh tmp-glibc/deploy/images/stm32mp13-disco/flashlayout_core-image-minimal/optee/FlashLayout_sdcard_stm32mp135f-dk-optee.tsv
```
## Burn the image to the sd card.
``` bash
sudo dd if=tmp-glibc/deploy/images/stm32mp13-disco/flashlayout_core-image-minimal/optee/../../FlashLayout_sdcard_stm32mp135f-dk-optee.raw of=/dev/mmcblk0 bs=8M conv=fdatasync status=progress && sudo sync
```
