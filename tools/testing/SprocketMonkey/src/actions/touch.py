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
    self._action = self._browser.touch()
    actionPriorities = {'doubleTapAction' : 2,
                        'moveAction' : 2,
                        'longPressAction' : 1,
                        'scrollAction' : 2,
                        'tapAction' : 3}
    try:
      actionToPerform = getattr(self, Generator().selectWithPriority(actionPriorities))
      actionToPerform()
    except AttributeError:
      raise AttributeError("Method calling error")

  def doubleTapAction(self):
    elements = self._browser.findAllElements()
    if len(elements) > 0:
      element = Generator().select(elements)
      self.debugLog("DoubleTap: "+ str(element.tag_name))
      self._action.double_tap(element)
      self._action.perform()
    else:
      self.debugLog("DoubleTap: {no element found}")

  def tapAction(self):
    elements = self._browser.findAllElements()
    if len(elements) > 0:
      element = Generator().select(elements)
      self.debugLog("Tap: "+ str(element.tag_name))
      self._action.tap(element)
      self._action.perform()
    else:
      self.debugLog("Tap: {no element found}")

  def moveAction(self):
    x, y = self.getRandomCoords()
    self.debugLog("Tap an Hold: {" + str(x) + "," + str(y) + "}")
    self._action.tap_and_hold(x, y)
    x, y = self.getRandomCoords()
    self._action.move(x, y)
    self.debugLog("Move and Release: {" + str(x) + "," + str(y) + "}")
    self._action.release(x, y)
    self._action.perform()

  def scrollAction(self):
    x, y = self.getRandomScrollPos()
    self.debugLog("Scroll: {" + str(x) + "," + str(y) + "}")
    self._action.scroll(x, y)
    self._action.perform()

  def longPressAction(self):
    elements = self._browser.findAllElements()
    if len(elements) > 0:
      element = Generator().select(elements)
      self.debugLog("Long Press: "+ str(element.tag_name))
      self._action.long_press(element)
      self._action.perform()
    else:
      self.debugLog("Long Press: {no element found}")

  def getRandomScrollPos(self, offset = 250):
    x = Generator().randomInt(minInt=-offset, maxInt=offset)
    y = Generator().randomInt(minInt=-offset, maxInt=offset)
    return x, y

  def getRandomCoords(self):
    size = self._browser.selectBody().size
    x = Generator().randomInt(maxInt=size['width'])
    y = Generator().randomInt(maxInt=size['height'])
    return x, y
