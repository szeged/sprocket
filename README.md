Sprocket
========
The browser is based on Chromium's Content API.
Currently only works on Linux. Tested on Ubuntu 14.04.
Build steps
-------------
#### Getting the Chromium source code
[The official guide](http://dev.chromium.org/developers/how-tos/get-the-code "Get the code")
#### Sprocket code
Inside chromium's 'src' directory, run the following commands.
```shell
# make a work branch
git checkout -b sprocket
# get the code
git clone https://github.com/szeged/sprocket.git sprocket
# apply pathes to chrome
git apply sprocket/patch/gritsettings.patch
git apply sprocket/patch/build_target.patch
# run the config
./build/gyp_chromium
# build it with 'sprocket' target
ninja -C out/Release sprocket
# run it from the out dir
./out/Release/sprocket
```
Additional info
---------------
[How to build a Web Browser](https://docs.google.com/document/d/1Uwvjy5Mj_CMIoLPbKo_kcdVVyrF9waocZQ1vJMuIj54 "How to build a Web Browser")
