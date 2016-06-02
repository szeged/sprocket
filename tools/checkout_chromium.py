#!/usr/bin/python

import os
import subprocess
import sys
from defaults import chromium_src_path, chromium_path, sprocket_folder_name, sprocket_path
from arguments import get_target, android, android_arm64, arm, x86_64

sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)

# check depot tools
if not "depot_tools" in os.environ["PATH"]:
    sys.exit("Error: depot_tools is not in your PATH.\nGet depot_tools at:\nhttps://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up")

target = get_target()

# checkout chromium source
if not os.path.isdir(chromium_path):
    os.mkdir(chromium_path)
os.chdir(chromium_path)
if os.listdir(chromium_path) == []:
    if target == android or target == android_arm64:
        subprocess.check_call(['fetch', '--nohooks', 'android'])
    else:
        subprocess.check_call(['fetch', '--nohooks', 'chromium'])
    os.chdir(chromium_src_path)
    if target == arm:
        subprocess.check_call(['./build/linux/sysroot_scripts/install-sysroot.py', '--arch=arm'])
    subprocess.check_call(['gclient', 'runhooks'])
    if target == android or target == android_arm64:
        subprocess.check_call(['./third_party/android_tools/sdk/tools/android', 'update', 'sdk', '--no-ui', '--filter', '57'])
else:
    os.chdir(chromium_src_path)

# create symlink in Chromium pointing to the Sprocket sources
if not os.path.exists(sprocket_folder_name):
    os.symlink(sprocket_path + '/src', sprocket_folder_name)

subprocess.check_call(['git', 'fetch', '--tags'])
lkgr = open(os.path.join(sprocket_path, 'LKGR')).read().strip('\r\n')
subprocess.check_call(['git', 'reset', '--hard', lkgr])

subprocess.check_call(['gclient', 'sync', '--with_branch_heads', '--jobs', '16'])

# connect Sprocket to Chromium's build system
subprocess.check_call(['git', 'apply', os.path.join(sprocket_path, 'patch', 'BUILD.gn.patch')])
