SprocketMonkey
========
SprocketMonkey is a Monkey-testing framework for browsers with
[Selenium](http://www.seleniumhq.org/ "Selenium") support,
developed especially for testing
[Sprocket's testing branch](https://github.com/szeged/sprocket/tree/testing "Sprocket testing branch")
on a few popular websites.
Requirements
-------------
* Sprocket
* Chromedriver for Sprocket
* Python >= 2.7
* Selenium for Python (can be installed via pip install selenium)

Sprocket code
-------------
[How to get Sprocket](https://github.com/szeged/sprocket#build-steps "Sprocket")

Build your own Chromedriver
-------------
See also: [Build Chromedriver](https://github.com/szeged/sprocket/tree/testing#chromedriver "Build Chromedriver")
```shell
# in chromium/src folder you should apply the Chromedriver patch
git apply sprocket/patch/chromedriver.patch
# run the config
./build/gyp_chromium sprocket/sprocket.gyp
# build it with 'chromedriver' target
ninja -C out/Release chromedriver
```
How to use
-------------
```shell
# run the script which sets the environment variables needed to run SprocketMonkey
. ./sprocket/tools/testing/SprocketMonkey/set_env.sh
# if you have your Sprocket / Chromedriver / SprocketMonkey in other than the default folder, you should edit the script and add the correct paths to it
# run the tests
python sprocket/tools/testing/SprocketMonkey/src/tests/test.py --url www.google.com
```
