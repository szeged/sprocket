![](android/java/res/mipmap-hdpi/app_icon.png) Sprocket
========
The browser is based on Chromium's Content API.
Supported platforms: Linux, Android.
Currently we have two branches: master and core.
Core branch contains the relevant code to a minimal browser which can be easily extended with new features.
Master branch contains extra features, for example toolbar, dialogs, tab support.
Build steps
-------------
#### Getting the Chromium source code
NOTE: Do NOT use --no-history command line switch when cloning Chromium repository. If the most recent revision is not compatible with Sprocket, you have to reset to the [Last Known Good Revision](https://github.com/szeged/sprocket/blob/master/LKGR "Last Known Good Revision").

[Linux official guide](http://dev.chromium.org/developers/how-tos/get-the-code "Get the code")

[Android official guide](https://www.chromium.org/developers/how-tos/android-build-instructions "Android Instructions")

[Linux ARM cross compile official guide](https://code.google.com/p/chromium/wiki/LinuxChromiumArm "ARM Instructions")

#### Sprocket code
Inside Chromium's 'src' directory, run the following commands:
```shell
# make a work branch
git checkout -b sprocket
# get the code
git clone https://github.com/szeged/sprocket.git sprocket
# checkout Sprocket Core branch
cd sprocket && git checkout core && cd ..
# use the latest working Chromium release revision
git reset --hard "$(< sprocket/LKGR)" && gclient sync --nohooks --with_branch_heads --jobs 16
```
Next, run the selected platform's commands.
##### Linux/PC
```shell
# run the config
./build/gyp_chromium sprocket/sprocket.gyp -Dclang=0
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
# run it with specific url
./tools/run_adb_sprocket www.google.com
# kill the running app
./tools/kill_adb_sprocket
```
##### Sandbox (Linux)
[Official guide](https://code.google.com/p/chromium/wiki/LinuxSUIDSandboxDevelopment "Sandbox")
```shell
# build the sandbox
ninja -C out/Release chrome_sandbox
# install it
BUILDTYPE=Release build/update-linux-sandbox.sh
# set the env
export CHROME_DEVEL_SANDBOX=/usr/local/sbin/chrome-devel-sandbox
# run sprocket with sandbox
./out/Release/sprocket --use-sandbox
```
Additional info
---------------
[How to build a Web Browser](https://docs.google.com/document/d/1Uwvjy5Mj_CMIoLPbKo_kcdVVyrF9waocZQ1vJMuIj54 "How to build a Web Browser")
