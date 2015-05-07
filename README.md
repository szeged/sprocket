![](android/java/res/mipmap-hdpi/app_icon.png) Sprocket
========
The browser is based on Chromium's Content API.
Supported platforms: Linux, Android
Build steps
-------------
#### Getting the Chromium source code
NOTE: Do NOT use --no-history command line switch when cloning Chromium repository. If the most recent revision is not compatible with Sprocket, you have to reset to the [Last Known Good Revision](https://github.com/szeged/sprocket/blob/master/LKGR "Last Known Good Revision").

[Linux official guide](http://dev.chromium.org/developers/how-tos/get-the-code "Get the code")

[Android official guide](https://code.google.com/p/chromium/wiki/AndroidBuildInstructions "Android Instructions")
#### Sprocket code
Inside Chromium's 'src' directory, run the following commands:
```shell
# make a work branch
git checkout -b sprocket
# get the code
git clone https://github.com/szeged/sprocket.git sprocket
# use a stable revision
git reset --hard "$(< sprocket/LKGR)" && gclient sync --nohooks
git apply sprocket/patch/gritsettings.patch
```
Next, run the selected platform's commands.
##### Linux/PC
```shell
# run the config
./build/gyp_chromium sprocket/sprocket.gyp
# build it with 'sprocket' target
ninja -C out/Release sprocket
# run it from the out dir
./out/Release/sprocket
```
##### Linux/ARM
```shell
# run the config
export GYP_CROSSCOMPILE=1
./build/gyp_chromium sprocket/sprocket.gyp -Ddisable_nacl=1 -Dtarget_arch=arm -Darm_float_abi
# build it with 'sprocket' target
ninja -C out/Release sprocket
# run it from the out dir
./out/Release/sprocket
```
##### Android
```shell
# run the config
./build/gyp_chromium sprocket/sprocket.gyp -DOS=android
# build it with 'sprocket_apk' target
ninja -C out/Release sprocket_apk
# install the apk
./build/android/adb_install_apk.py --apk Sprocket.apk --release
```
Additional info
---------------
[How to build a Web Browser](https://docs.google.com/document/d/1Uwvjy5Mj_CMIoLPbKo_kcdVVyrF9waocZQ1vJMuIj54 "How to build a Web Browser")
