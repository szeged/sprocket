![](src/android/res/mipmap-hdpi/app_icon.png) Sprocket
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
#### Getting depot_tools

>- [Linux offical guide](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up "Setting up")


#### Initialising repository and building Sprocket
Use **--x86_64** (default), **--arm**, **--android** or **--android_arm64** switches depending on your target architecture:

```shell
# get the code
git clone https://github.com/szeged/sprocket.git sprocket
cd sprocket
# get the Chromium source code
tools/checkout_chromium.py --x86_64
# build the project
tools/build_sprocket.py --x86_64
```

#### Running Sprocket

```shell
# Linux
./chromium/src/out/Default/sprocket

# Android
# install the apk
chromium/src/build/android/adb_install_apk.py --apk chromium/src/out/Default/apks/Sprocket.apk --release
# run it with specific URL
./tools/run_adb_sprocket www.google.com
# kill the running app
./tools/kill_adb_sprocket
```

Additional info
---------------
##### Website
>- [github.io/sprocket](http://szeged.github.io/sprocket/ "Sprocket website")


##### Video
>- [Introduction video](https://youtu.be/sgKi38GbOj0 "Introduction video")


##### Guides
>- [Create your embedded browser](http://browser.sed.hu/blog/20151209/web-sprockets-create-your-embedded-browser-24-hours "Create your embedded browser")
>- [1.) First steps](http://browser.sed.hu/blog/20151211/web-sprockets-lesson-1-first-steps "Lesson 1 - First steps")
>- [2.) Crafting a window (Part I)](http://browser.sed.hu/blog/20160106/web-sprockets-lesson-2-crafting-window-part-i "Crafting a window (Part I)")
>- [3.) Crafting a window (Part II)](http://browser.sed.hu/blog/20160215/web-sprockets-lesson-3-crafting-window-part-ii "Crafting a window (Part II)")


#####  Documentation  
>- [How to build a Web Browser](https://goo.gl/Dn7RWs "How to build a Web Browser")
>- [Chromium Architecture Overview](https://goo.gl/U092Og "Chromium Architecture overview")


##### Blog posts  
>- [Sprocket Introduction](http://browser.sed.hu/blog/20150714/sprocket-experimental-multiplatform-browser-based-content-api "Sprocket introduction")
>- [Sprocket's core branch](http://browser.sed.hu/blog/20150805/core-sprocket-minimal-experimental-browser-based-content-api "Sprocket's core branch")
>- [Monkey testing Sprocket](http://browser.sed.hu/blog/20150917/sprocketmonkey-infinite-monkey-theorem-about-sprocket "Monkey testing Sprocket")
>- [Chromium Architecture Overview](http://browser.sed.hu/blog/20151016/chromium-architecture-overview "Chromium Architecture Overview")
