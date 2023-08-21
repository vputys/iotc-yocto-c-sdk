# Raspberry Pi 4 Model B 

***Note: `kirkstone` yocto branch***

## Cloning

- Clone these layers to your work directory:
``` 
    git clone git://git.yoctoproject.org/poky.git -b kirkstone &&
    git clone git://git.openembedded.org/meta-openembedded  -b kirkstone &&
    git clone git://git.yoctoproject.org/meta-raspberrypi.git -b kirkstone &&
    git clone git@github.com:avnet-iotconnect/iotc-yocto-c-sdk.git -b kirkstone
```

## Source work environment: 

```
source poky/oe-init-build-env build
```

## Layers

- Add following layers to your `bblayers.conf` file usually found in `<your_path>/build/conf/`:

```
bitbake-layers add-layer ../meta-raspberrypi/ && \
bitbake-layers add-layer ../meta-openembedded/meta-oe/ && \
bitbake-layers add-layer ../meta-openembedded/meta-python/ && \
bitbake-layers add-layer ../meta-openembedded/meta-multimedia/ && \
bitbake-layers add-layer ../meta-openembedded/meta-networking/ && \
bitbake-layers add-layer ../iotc-yocto-c-sdk/meta-iotconnect && \
bitbake-layers add-layer ../iotc-yocto-c-sdk/meta-myExampleIotconnectLayer 
```

## Adding IoTConnect C SDK recipe

- Assuming you're in `build` folder:

```
echo -e '\nIMAGE_INSTALL += " iotc-c-sdk"' >> conf/local.conf
```
or add this line to your image manually `IMAGE_INSTALL += " iotc-c-sdk"`

- then: 

## Connectivity and uart:
```
echo -e '\nMACHINE = "raspberrypi4"\nDISTRO_FEATURES:append = " systemd"\nVIRTUAL-RUNTIME_init_manager = "systemd"\nENABLE_UART = "1"' >> conf/local.conf
```
**OR**
```
echo -e '\nMACHINE = "raspberrypi4"\nIMAGE_INSTALL += " connman"\nIMAGE_INSTALL += " connman-client"\nENABLE_UART = "1"' >> conf/local.conf
```
## Build
- Build image:
```
bitbake core-image-base
```

## One giant copy-paste command:

```
git clone git://git.yoctoproject.org/poky.git -b kirkstone &&
git clone git://git.openembedded.org/meta-openembedded  -b kirkstone &&
git clone git://git.yoctoproject.org/meta-raspberrypi.git -b kirkstone &&
git clone git@github.com:avnet-iotconnect/iotc-yocto-c-sdk.git -b kirkstone &&
source poky/oe-init-build-env build &&
bitbake-layers add-layer ../meta-raspberrypi/ && \
bitbake-layers add-layer ../meta-openembedded/meta-oe/ && \
bitbake-layers add-layer ../meta-openembedded/meta-python/ && \
bitbake-layers add-layer ../meta-openembedded/meta-multimedia/ && \
bitbake-layers add-layer ../meta-openembedded/meta-networking/ && \
bitbake-layers add-layer ../iotc-yocto-c-sdk/meta-iotconnect && \
bitbake-layers add-layer ../iotc-yocto-c-sdk/meta-myExampleIotconnectLayer &&
echo -e '\nMACHINE = "raspberrypi4"\nIMAGE_INSTALL += " connman"\nIMAGE_INSTALL += " connman-client"\nENABLE_UART = "1"' >> conf/local.conf &&
echo -e '\nIMAGE_INSTALL += " iotc-c-sdk"' >> conf/local.conf &&
bitbake core-image-base
```