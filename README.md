# IoT-Connect Yocto Integration
***This IoT-Connect connect layer only supports `hardknott`***

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

## Board specific examples can be found [here](board-specific/README.md)
