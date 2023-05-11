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
> *NB: you can exit the docker enviroment as you would any bash session with `exit`. Bear in mind that to re-enter the enviroment you'll need to call the following;*
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
