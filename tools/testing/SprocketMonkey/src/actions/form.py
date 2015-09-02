# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from action import Action
from src.utils.generator import Generator

class FormAction(Action):

  def __init__(self, browser):
    Action.__init__(self, browser)
    self._type = "Form Action"

  def perform(self):
    self._elements = []
    self._elements.extend(self._browser.findElementsByTagName("textarea"))
    self._elements.extend(self._browser.findElementsByXPath("//input[@type='text' or @type='password' or @type='email' or @type='numbers' or not(@type)]"))
    self._elements.extend(self._browser.findElementsByTagName("select"))
    self._elements.extend(self._browser.findElementsByXPath("//input[@type='radio' or @type='checkbox']"))
    if len(self._elements) > 0:
      element = Generator().select(self._elements)
      if element.tag_name == 'select':
        all_options = element.find_elements_by_tag_name("option")
        option = Generator().select(all_options)
        self.debugLog("Form: " + option.get_attribute("value") + " from select")
        option.click()
      elif element.get_attribute("type") == 'radio' or element.get_attribute("type") == 'checkbox':
        self.debugLog("Form: " + element.get_attribute("value") + " " + element.get_attribute("type"))
        element.click()
      else:
        inputText = ''
        for i in range(0, Generator().randomInt(minInt=1, maxInt=10)):
          inputText += Generator().chars()
        self.debugLog("Form: " + inputText + " to " + element.get_attribute("type"))
        element.send_keys(inputText)
        if element.tag_name != 'textarea':
          element.submit()
    else:
      self.debugLog("Form: no valid form")
