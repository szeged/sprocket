# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from action import Action
from src.utils.generator import Generator

class ScrollAction(Action):

  def __init__(self, browser):
    Action.__init__(self, browser)
    self._type = "Scroll Action"

  def perform(self):
    size = self._browser.selectBody().size
    x = Generator().randomInt(maxInt=size['width'])
    y = Generator().randomInt(maxInt=size['height'])
    self.debugLog("Scroll to: {" + str(x) + "," + str(y) + "}")
    self._browser.executeScript("window.scrollTo(arguments[0], arguments[1]);", x, y)
