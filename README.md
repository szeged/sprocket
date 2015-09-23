![](android/java/res/mipmap-hdpi/app_icon.png) Sprocket
========
**Sprocket** is an experimental browser based on **Chromium's Content API**.  
Supported platforms: **Linux** (x86_64, ARM), **Android** (ARM, Aarch64).  
Currently we have three branches: **master**, **testing** and **core**.  
>- **Core** branch contains the relevant code to a *minimal browser* which can be easily extended with new features.
>- **Master** branch contains extra *features*, for example *toolbar*, *dialogs*, *tab support*.
>- **Testing** branch contains *test specific features*, for example *developer tools*.  

You can view our build status [here](http://build.sprocket.sed.hu/waterfall "Sprocket buildbot").
Build steps
-------------
#### Getting the Chromium source code
NOTE: Do **NOT** use --no-history command line switch when cloning Chromium repository.

>- [Linux official guide](http://dev.chromium.org/developers/how-tos/get-the-code "Get the code")

>- [Android official guide](https://www.chromium.org/developers/how-tos/android-build-instructions "Android Instructions")

>- [Linux ARM cross compile official guide](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_chromium_arm.md "ARM Instructions")

#### Sprocket
Inside Chromium's **src** directory, run the following commands:
```shell
# make a working branch
git checkout -b sprocket
# get the code
git clone https://github.com/szeged/sprocket.git sprocket
# use the latest working Chromium release revision
# if it doesn't see the revision, you should try `git fetch --tags` first
git reset --hard "$(< sprocket/LKGR)" && gclient sync --with_branch_heads --jobs 16
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
./build/gyp_chromium sprocket/sprocket.gyp -Ddisable_nacl=1 -Dtarget_arch=arm -Darm_float_abi=hard
# build it with 'sprocket' target
ninja -C out/Release sprocket
# run it from the out dir
./out/Release/sprocket
```
##### Android
```shell
# run this config if you are building for ARM target
./build/gyp_chromium sprocket/sprocket.gyp -DOS=android
# OR run this config if you are building for Aarch64 target
./build/gyp_chromium sprocket/sprocket.gyp -DOS=android -Dtarget_arch=arm64
# build it with 'sprocket_apk' target
ninja -C out/Release sprocket_apk
# install the apk
./build/android/adb_install_apk.py --apk Sprocket.apk --release
# run it with specific url
./sprocket/tools/run_adb_sprocket www.google.com
# kill the running app
./sprocket/tools/kill_adb_sprocket
```
##### Sandbox (Linux)
[Official guide](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_suid_sandbox_development.md "Sandbox")
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
#####  Documentation  
>- [How to build a Web Browser](https://docs.google.com/document/d/1Uwvjy5Mj_CMIoLPbKo_kcdVVyrF9waocZQ1vJMuIj54 "How to build a Web Browser")  


##### Blog posts  
>- [Sprocket Introduction](http://browser.sed.hu/blog/20150714/sprocket-experimental-multiplatform-browser-based-content-api "Sprocket introduction")
>- [Sprocket's core branch](http://browser.sed.hu/blog/20150805/core-sprocket-minimal-experimental-browser-based-content-api "Sprocket's core branch")
>- [Monkey testing Sprocket](http://browser.sed.hu/blog/20150917/sprocketmonkey-infinite-monkey-theorem-about-sprocket "Monkey testing Sprocket")


