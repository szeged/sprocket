# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from action import Action
from src.utils.generator import Generator

class SmartClickAction(Action):

  def __init__(self, browser):
    Action.__init__(self, browser)
    self._type = "SmartClick Action"

  def perform(self):
    self._action = self._browser.action()
    self._elements = []
    self._elements.extend(self._browser.findElementsByTagName("a"))
    self._elements.extend(self._browser.findElementsByXPath("//input[@type='button' or @type='submit']"))
    self._elements.extend(self._browser.findElementsByTagName("button"))
    self._elements.extend(self._browser.findElementsByXPath("//*[@onclick]"))
    if len(self._elements) > 0:
      element = Generator().select(self._elements)
      self.debugLog("Smart Click: {" + str(element) + "}")
      self._action.click(element)
      self._action.perform()
    else:
      self.debugLog("Smart Click: {no link / button found}")
