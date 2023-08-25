# MSC LDK Example

## Interaction with Avnet Embedded

create an account on embedded.avnet.com

download the msc-ldk from this page https://embedded.avnet.com/product/msc-sm2s-imx8plus/#manual (this was the latest at the time of writing "msc-ldk-bsp-01047-v1.9.0-20220909.pdf"). You might also need documents from https://embedded.avnet.com/product/msc-sm2s-imx8plus/

***Note:*** [List of useful documentation](#msc-ldk-documentation)

As soon as you get access to embedded.avnet.com find a document named *"msc-ldk-bsp-01047-v1.9.0-20220909"* and *"App_Note_030_Building_from_MSC_Git_V1_8"* and do the RSA SSH key bit mentioned in both documents then send it to address provided in those manuals along with the project code you want to work on (*msc_01047* for SM2S-IMX8PLUS).

- ***Note:*** Do these previous steps as soon as possible as they will be blocking you from continuing work.

follow instructions in said guide (*"msc-ldk-bsp-01047-v1.9.0-20220909"*) to establish the build enviroment. (Up to section  4.7 "Building Images" (not including 4.7) (Optionally skip "Setup Optional Docker COntainer")). 

## Building MSC LDK

***IMPORTANT:*** do not use *"App_Note_030_Building_from_MSC_Git_V1_8"* as guide for building images. It provides information on building generic image and skips an important step mentioned in another document. Please use *"msc-ldk-bsp-01047-v1.9.0-20220909"* guide to build images.

***IMPORTANT:*** when using `./setup.sh --bsp=01047` to setup everything you need to use this command instead - `./setup.sh --bsp=01047 --branch=kirkstone` because default settings for this script will pull hardknott branches.

***Also note:*** *"msc-ldk-bsp-01047-v1.9.0-20220909"* document in Section 4.3 makes a typo. `git checkout 1.9.0` must be `git checkout v1.9.0`.




### Docker build

#### Getting docker

***Note:*** for this to work you might need to follow steps provided in Section 4.2 in *"msc-ldk-bsp-01047-v1.9.0-20220909"* document (Section "Setup Optional Docker Container") or combinating native build with your own Dockerfile.

This was the docker run used in the first instance:

- `docker run --privileged -t -i --dns <your networks gateway IP here>  --name msc-ldk -h docker -v "pwd"/src:/src msc-ldk /bin/bash`

#### Seting up

Follow the instructions (*"msc-ldk-bsp-01047-v1.9.0-20220909"*) in section 4.7 (Building Images Section)

Building will take a while and at the end you will get a lot of built images. Make sure `imx-boot-sm2s-imx8mp-sd.bin-flash_evk` is built.

#### Exiting and stopping docker

- - NB: you can exit the docker enviroment as you would any bash session with exit. Bear in mind that to re-enter the enviroment you'll need to call the following;

```
    docker stop msc-ldk && \
    docker rm msc-ldk
```

If you didn't change strings following `-v` option in the docker command above you should find cloned directories and built images (`build/01047/tmp/deploy/images/sm2s-imx8mp/`) under src directory in your current directory.


### Native build

Follow the instructions (*"msc-ldk-bsp-01047-v1.9.0-20220909"*) in section 4.7 (Building Images Section)

Building will take a while and at the end you will get a lot of built images. Make sure `imx-boot-sm2s-imx8mp-sd.bin-flash_evk` is built.

## Adding layers and building

clone iotc-yocto-c-sdk into `<basedir>/source`.

Add both layers from iotc-yocto-c-sdk to the `bblayers.conf` file located in `build/01047/conf`

navigate to `build/01047` and use `bitbake <your-image-name>`

After building make sure `<your_image_name>-sm2s-imx8mp.wic` is actually built

## Flashing

***Note:*** if build enviroment is not set up - head to base directory and run `source sources/yocto.git/oe-init-build-env build/01047`

Once the layers have been integrated as per the instructions above & your image has been built it can be flashed to the target using:

- ***NOTE:*** before flashing you need to see Section 6.16 (at the time of writing) Boot Options (Also see *"App_Note_035_Using_NXP_Mfgtool+uuu"* document Section 3.3 and further). It has information on what needs to be done to enable flashing via USB cable (you'll need to bridge 2 pins on the back of the board on board startup). In also contains information about DIP switches you might need to configure to enable right boot device (*"App_Note_035_Using_NXP_Mfgtool+uuu"* does not provide information on DIP switches so refer to *"MSC-SM2S-MB-EP5_User-Manual_DV3_V013"* document Section 3.16 (Boot Selection) or *"MSC_SM2S-IMX8PLUS_Manual"* Section 6.16 (Boot Options)). 

- ***Note:*** emmc_burn_all.lst and emmc_burn_loader.lst can be found in an Appendix section of *"App_Note_035_Using_NXP_Mfgtool+uuu"* document. files there contain some formatting as well as syntax problems not allowing to flash. However, they should be relatively easy to fix.

    ```
    sudo uuu -b emmc_burn_all.lst \
    <path-to>/imx-boot-sm2s-imx8mp-sd.bin-flash_evk \
    <path-to>/<your_image_name>-sm2s-imx8mp.wic
    ```

    where might resolve as

    ```
    sudo uuu -b emmc_burn_all.lst \
    ../../docker-msc-ldk/src/msc-ldk/build/01047/tmp/deploy/images/sm2s-imx8mp/imx-boot-sm2s-imx8mp-sd.bin-flash_evk \
    ../../docker-msc-ldk/src/msc-ldk/build/01047/tmp/deploy/images/sm2s-imx8mp/<your_image_name>-sm2s-imx8mp.wic
    ```
On the target (via serial debug or ssh terminal (for serial debug please see Section 3.10 RS485 / RS232 (SER0) in *"MSC-SM2S-MB-EP5_User-Manual_DV3_V013"* )) execute `basic-sample`

# Appendix


## MSC LDK Documentation

Available from: https://embedded.avnet.com/product/msc-sm2s-imx8plus/#manual

- *MSC_SM2S-IMX8PLUS_Manual*
- *msc-ldk-bsp-01047-v1.9.0-20220909* (information on getting, cloning and building msc ldk images)
- *App_Note_035_Using_NXP_Mfgtool+uuu* (important information on flashing and boot select)
- *App_Note_030_Building_from_MSC_Git_V1_8* (a bit clearer document than *"msc-ldk-bsp-01047-v1.9.0-20220909"*, however misses important step with `git checkout v1.9.0` after cloning initial repo)
- *MSC-SM2S-MB-EP5_User-Manual_DV3_V013* (can be found here: https://embedded.avnet.com/product/msc-sm2s-mb-ep5/#manual (also requires Avnet Embedded account for it))

## If in doubt, you may need...

* App_Note_030_Building_from_MSC_Git_V1_7.pdf *(Application Notes from [here] (https://embedded.avnet.com/product/msc-sm2s-imx8/#application_notes))*
* App_Note_040_DTB_File_Selection_v20.pdf
* User privileges for the sample images from embedded.avnet:

 | User | Password |
 | ---- | :------: |
 | msc  | msc      |
 | root | mscldk   |

* this email address for the great help from support.boards@avnet.eu

## Testing

***Note***: Instructions mentioned above were tested on docker image of Ubuntu 18.04 in combination with Ubuntu 22.04.