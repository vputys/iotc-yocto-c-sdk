# Pi4 build from scratch

Final directory structure (abridged)
```bash
tree -L 2 iot-connect-rpi4/
iot-connect-rpi4/
├── build
│   └── conf
├── meta-iotconnect
├── meta-iotc-python-sdk
├── meta-myExampleIotconnectLayer
├── meta-my-iotc-python-sdk-example
├── meta-openembedded
├── meta-raspberrypi
└── poky
```

```bash
# Clone the base layers
git clone git://git.yoctoproject.org/poky.git -b hardknott
git clone git://git.openembedded.org/meta-openembedded  -b hardknott
git clone git://git.yoctoproject.org/meta-raspberrypi.git -b hardknott

# Initialize bitbake
source poky/oe-init-build-env

# Add layers
bitbake-layers add-layer ../meta-raspberrypi/ && \
bitbake-layers add-layer ../meta-openembedded/meta-oe/ && \
bitbake-layers add-layer ../meta-openembedded/meta-python/ && \
bitbake-layers add-layer ../meta-openembedded/meta-multimedia/ && \
bitbake-layers add-layer ../meta-openembedded/meta-networking/

# Set machine to raspberrypi4
sed -i 's/qemux86-64/raspberrypi4/g' ./conf/local.conf 

# Build patch
echo -e '\nSECURITY_STRINGFORMAT = ""\n' >> ./conf/local.conf 

# Accept the wireless license and enable UART for serial debugging
echo -e '\nLICENSE_FLAGS_ACCEPTED = " synaptics-killswitch"\nENABLE_UART = "1"\n' >> ./conf/local.conf
```

NOTE:
The target device will not have the correct time set, using a distro with systemd fixes this, there may be alternatives but the easiest option is to include systemd to your image

```bash
# Include systemd to your `local.conf`
echo -e '\nDISTRO_FEATURES_append = " systemd"\nDISTRO_FEATURES_BACKFILL_CONSIDERED += " sysvinit"\nVIRTUAL-RUNTIME_init_manager = " systemd"\nVIRTUAL-RUNTIME_initscripts = " systemd-compat-units"\n'
 >> ./conf/local.conf
```

# C SDK stuff

```bash
# Get layers from the repo
wget https://github.com/avnet-iotconnect/iotc-yocto-c-sdk/archive/refs/heads/hardknott.zip && \
unzip hardknott.zip -d .tmp/ && \
mv .tmp/iotc-yocto-c-sdk-hardknott/meta-* . && \
rm -r hardknott.zip .tmp/ && \

# Add layers to build and include the recipe to your build
cd build && \
bitbake-layers add-layer ../meta-iotconnect/ && \
bitbake-layers add-layer ../meta-myExampleIotconnectLayer/ && \
echo -e '\nIMAGE_INSTALL += " iotc-c-sdk"' >> ./conf/local.conf 
```

# Build

```bash
bitbake core-image-base
```


# Pi optional

```bash
# Set download directories to `${HOME}/yocto`
echo -e '\nDL_DIR ?= "${HOME}/yocto/yocto-downloads"\nSSTATE_DIR ?= "${HOME}/yocto/yocto-sstate-cache"\nSSTATE_MIRRORS ?= "file://.* http://sstate.yoctoproject.org/3.1.15/PATH;downloadfilename=PATH"' >> ./conf/local.conf

# Include nano text editor
echo -e '\nCORE_IMAGE_EXTRA_INSTALL_append = " nano"\n' >> ./conf/local.conf

# Add a user `root` with password `avnet`
echo -e '\nEXTRA_IMAGE_FEATURES=""
INHERIT += "extrausers"
EXTRA_USERS_PARAMS = "\ 
\tusermod -P avnet root; \ 
"' >> ./conf/local.conf 
```
