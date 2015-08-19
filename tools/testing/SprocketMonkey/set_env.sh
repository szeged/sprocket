# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# How to use
# this script sets the environment variables needed to run SprocketMonkey
# run it from chromium/src
# $ . ./sprocket/tools/testing/SprocketMonkey/set_env.sh
# if you have your Sprocket / Chromedriver / SprocketMonkey in other than the default folder, you should edit this script with the correct paths

BASE_DIR=`pwd`
export DRIVER_PATH=${BASE_DIR}/out/Release
export PATH=$PATH:$DRIVER_PATH
export SPROCKET_PATH=${BASE_DIR}/out/Release
export PYTHONPATH=${BASE_DIR}/sprocket/tools/testing/SprocketMonkey
