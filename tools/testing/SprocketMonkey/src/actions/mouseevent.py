# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from action import Action
from src.utils.generator import Generator
import time

class MouseEventAction(Action):

  def __init__(self, browser):
    Action.__init__(self, browser)
    self._type = "MouseEvent Action"

  def perform(self):
    self._action = self._browser.action()
    actionPriorities = {'clickAction' : 6,
                        'dragAndDropAction' : 1,
                        'mouseMoveAction' : 2,
                        'doubleClickAction' : 3,
                        'rightClickAction' : 1,
                        'mouseHoldAction' : 1}
    try:
      actionToPerform = getattr(self, Generator().selectWithPriority(actionPriorities))
      actionToPerform()
    except AttributeError:
      raise AttributeError("Method calling error")


  def clickAction(self):
    x, y = self.getRandomCoords()
    b = self._browser.selectBody()
    self.debugLog("Click: {" + str(x) + "," + str(y) + "}")
    self._action.move_to_element_with_offset(b, x, y)
    self._action.click()
    self._action.perform()

  def dragAndDropAction(self):
    elements = self._browser.findAllElements()
    source = Generator().select(elements)
    target = Generator().select(elements)
    self.debugLog("Drag And Drop: {" + str(source) + "," + str(target) + "}")
    self._action.drag_and_drop(source, target)
    self._action.perform()

  def mouseMoveAction(self):
    x, y = self.getRandomCoords()
    b = self._browser.selectBody()
    self.debugLog("Move to: {" + str(x) + "," + str(y) + "}")
    self._action.move_to_element_with_offset(b, x, y)
    self._action.perform()

  def doubleClickAction(self):
    x, y = self.getRandomCoords()
    b = self._browser.selectBody()
    self.debugLog("Double click: {" + str(x) + "," + str(y) + "}")
    self._action.move_to_element_with_offset(b, x, y)
    self._action.double_click()
    self._action.perform()

  def rightClickAction(self):
    x, y = self.getRandomCoords()
    b = self._browser.selectBody()
    self.debugLog("Double click: {" + str(x) + "," + str(y) + "}")
    self._action.move_to_element_with_offset(b, x, y)
    self._action.context_click()
    self._action.perform()

  def mouseHoldAction(self):
    x, y = self.getRandomCoords()
    b = self._browser.selectBody()
    self.debugLog("Hold down: {" + str(x) + "," + str(y) + "}")
    self._action.move_to_element_with_offset(b, x, y)
    self._action.click_and_hold()
    self._action.perform()
    self._action.release()
    self._action.perform()

  def getRandomCoords(self):
    size = self._browser.selectBody().size
    x = Generator().randomInt(maxInt=size['width'])
    y = Generator().randomInt(maxInt=size['height'])
    return x, y
