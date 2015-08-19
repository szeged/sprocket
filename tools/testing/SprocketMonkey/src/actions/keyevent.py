# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from action import Action
from src.utils.generator import Generator

class KeyEventAction(Action):

  def __init__(self, browser):
    Action.__init__(self, browser)
    self._type = "KeyEvent Action"

  def perform(self):
    self._action = self._browser.action()
    elements = self._browser.findAllElements()
    element = Generator().select(elements)
    key = Generator().chars()
    self.debugLog("Send key: " + key + " to " + element.tag_name)
    self._action.move_to_element(element)
    self._action.send_keys(key)
    self._action.perform()
