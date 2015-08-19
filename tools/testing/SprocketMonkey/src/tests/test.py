# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import sys
import getopt

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--url', help='test url')
parser.add_argument('--title', help='website title')
parser.add_argument('--android', action='store_true', help='test on android')
args = parser.parse_args()

try:
  from src.monkey import Monkey
except ImportError:
  raise ImportError("You should add your SprocketMonkey folder to PYTHONPATH")

m = Monkey(args.android)
m.do(args.url, args.title)
