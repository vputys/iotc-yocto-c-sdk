# Raspberry Pi 4 Model B 

***Note: `dunfell` yocto branch***

- Clone these layers to your work directory:
``` 
    git clone git://git.yoctoproject.org/poky.git -b dunfell
    git clone git://git.openembedded.org/meta-openembedded  -b dunfell
    git clone git://git.yoctoproject.org/meta-raspberrypi.git -b dunfell
```
- Source work environment - `source <your_path>/poky/oe-init-build-env`

- Add following layers to your `bblayers.conf` file usually found in `<your_path>/build/conf/`:
```
    <your_path>/meta-openembedded/meta-oe \ 
    <your_path>/meta-openembedded/meta-python \ 
    <your_path>/meta-openembedded/meta-networking \ 
    <your_path>/meta-openembedded/meta-multimedia \
    <your_path>/meta-raspberrypi \
    <your_path>/iotc-yocto-c-sdk/meta-iotconnect \
    <your_path>/iotc-yocto-c-sdk/meta-myExampleIotconnectLayer 
```

- Change `MACHINE` variable in `local.conf` file to `MACHINE = "raspberrypi4"`

- Add these lines to the same `local.conf` file:
```
    DISTRO_FEATURES:append = " systemd"
    VIRTUAL-RUNTIME_init_manager = "systemd"
    LICENSE_FLAGS_ACCEPTED = " synaptics-killswitch"
```
- Optionally add `ENABLE_UART = "1"`

- Add IoTC C SDK to your image (in your build image .bb file or in `local.conf`) - `IMAGE_INSTALL += " iotc-c-sdk"`

- Build your image - `bitbake <your_image>`