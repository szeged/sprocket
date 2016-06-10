import argparse

android = 'android'
android_arm64 = 'android_arm64'
arm = 'arm'
x86_64 = 'x86_64'

parser = argparse.ArgumentParser()
parser.add_argument('--x86_64', action='store_true', help='Build for x86_64 target')
parser.add_argument('--android', action='store_true', help='Build for Android target')
parser.add_argument('--android_arm64', action='store_true', help='Build for Android Aarch 64 target')
parser.add_argument('--arm', action='store_true', help='Build for ARM Linux target')

args = parser.parse_args()

def get_target():
  if args.android:
    return android
  if args.android_arm64:
    return android_arm64
  elif args.arm:
    return arm
  else:
    return x86_64
