Sprocket
========
The browser is based on Chromium's Content API.
Build steps
-------------
#### Getting the Chromium source code
The official guide: http://dev.chromium.org/developers/how-tos/get-the-code
#### Sprocket code
Inside chromium's 'src' directory, run the following commands.
```shell
# get the code
git clone https://github.com/szeged/sprocket.git minibrowser
# make a work branch
git checkout -b sprocket
# apply pathes to chrome
git apply minibrowser/patch/gritsettings.patch
git apply minibrowser/patch/build_target.patch
# run the config
./build/gyp_chromium
# build it with 'minibrowser' target
ninja -C out/Release minibrowser
# run it from the out dir
./out/Release/minibrowser
```