#!/usr/bin/python

import os
import subprocess
import sys
from defaults import chromium_src_path, build_output_dir
from arguments import get_target, android, android_arm64, arm, x86_64

sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)

target = get_target()

# generate Ninja files with GN
os.chdir(chromium_src_path)
if target == x86_64:
    subprocess.check_call(['gn', 'gen', build_output_dir, '--args=is_debug=false'])
elif target == arm:
    subprocess.check_call(['gn', 'gen', build_output_dir, '--args=target_cpu="arm" is_debug=false'])
elif target == android:
    # target_cpu is "arm" by default
    subprocess.check_call(['gn', 'gen', build_output_dir, '--args=target_os="android" is_debug=false'])
elif target == android_arm64:
    subprocess.check_call(['gn', 'gen', build_output_dir, '--args=target_os="android" target_cpu="arm64" is_debug=false'])


# build with Ninja
if target == x86_64 or target == arm:
    subprocess.check_call(['ninja', '-C', build_output_dir, 'sprocket'])
elif target == android or target == android_arm64:
    subprocess.check_call(['ninja', '-C', build_output_dir, 'sprocket_apk'])
