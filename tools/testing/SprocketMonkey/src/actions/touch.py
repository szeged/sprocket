# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from action import Action
from src.utils.generator import Generator

class TouchAction(Action):

  def __init__(self, browser):
    Action.__init__(self, browser)
    self._type = "Touch Action"

  def perform(self):
    size = self._browser.selectBody().size
    touch = self._browser.touch()
    x = Generator().randomInt(maxInt=size['width'])
    y = Generator().randomInt(maxInt=size['height'])
    self.debugLog("Touch: {" + str(x) + "," + str(y) + "} at " + self._browser.currentUrl())
    touch.tap_and_hold(x, y)
    touch.release(x, y)
    touch.perform()
