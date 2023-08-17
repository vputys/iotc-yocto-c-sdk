# Raspberry Pi 4 Model B 

***Note: `kirkstone` yocto branch***

- Clone these layers to your work directory:
``` 
    git clone git://git.yoctoproject.org/poky.git -b kirkstone &&
    git clone git://git.openembedded.org/meta-openembedded  -b kirkstone &&
    git clone git://git.yoctoproject.org/meta-raspberrypi.git -b kirkstone &&
    git clone git@github.com:avnet-iotconnect/iotc-yocto-c-sdk.git -b kirkstone
```

- Source work environment: 
```
source poky/oe-init-build-env build
```

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

- Assuming you're in `build` folder:

```
echo -e '\nENABLE_UART = "1"' >> conf/local.conf
```

```
echo -e '\nIMAGE_INSTALL += " iotc-c-sdk"' >> conf/local.conf
```

or add this line to your image manually `IMAGE_INSTALL += " iotc-c-sdk"`

- Build image:
```
bitbake core-image-minimal
```

**One giant copy-paste command**:

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
echo -e '\nENABLE_UART = "1"' >> conf/local.conf &&
echo -e '\nIMAGE_INSTALL += " iotc-c-sdk"' >> conf/local.conf