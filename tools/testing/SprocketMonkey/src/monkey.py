# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

try:
  from sprocket import Sprocket
  from utils.generator import Generator
  from actions.mouseevent import MouseEventAction
  from actions.keyevent import KeyEventAction
  from actions.scroll import ScrollAction
  from actions.smartclick import SmartClickAction
  from actions.touch import TouchAction
  from actions.form import FormAction
except ImportError:
  raise ImportError("You should add your SprocketMonkey folder to PYTHONPATH")

import time

class Monkey:
  _browser = None
  _onAndroid = False
  _minRunTime = 120
  _windowHandle = None
  _currentUrl = None
  _actions = []
  _priorities = None

  def __init__(self, android = False):
    self._onAndroid = android
    self._browser = Sprocket(android)
    self.addActions()

  def setMinRuntime(self,time):
    self._minRunTime = time

  def do(self, url, title):
    print "Monkey See, Monkey Click, Type and Scroll!"
    try:
      if url is None:
        url = "http://www.google.com"
        title = None
      if title is None:
        title = self.getDefaultTitle(url)
      title = title.lower()
      url = self.sanitizeUrl(url)

      st = time.time()
      # run at least 2 mins
      while time.time() - st < self._minRunTime:
        # load the url
        self._browser.goto(url)
        self._windowHandle = self._browser.currentWindowHandle()
        self.closeOtherWindows()

        while title in self._browser.title().lower():
          try:
            self.closeOtherWindows()
            if "data:," in self._browser.title():
              print "too back, goto",url
              self._browser.goto(url)
              self._currentUrl = None

            self.performNextAction()
          except Exception, e:
            print e
        print "we moved away from",title,"to",self._browser.title()
    except Exception, e:
      print "Monkey error:",e
    finally:
      self._browser.quit()
      print "Exiting..."
    print "done"

  def closeOtherWindows(self):
    handles = self._browser.windowHandles()
    if len(handles) == 1:
      return
    print "More than one window, try to close one of it..."
    for handle in handles:
      if handle != self._windowHandle:
          self._browser.switchToWindow(handle)
          self._browser.close()

    self._browser.switchToWindow(self._windowHandle)
    print "Window closed"

  def addActions(self):
    mouseEventAction = MouseEventAction(self._browser)
    keyEventAction = KeyEventAction(self._browser)
    scrollAction = ScrollAction(self._browser)
    smartClickAction = SmartClickAction(self._browser)
    formAction = FormAction(self._browser)
    touchAction = TouchAction(self._browser)

    self._actions.append(mouseEventAction)
    self._actions.append(keyEventAction)
    self._actions.append(scrollAction)
    self._actions.append(smartClickAction)
    self._actions.append(formAction)
    self._actions.append(touchAction)

    #set priority for each action
    self._priorities = { self._actions.index(mouseEventAction) : 3,
                         self._actions.index(keyEventAction)   : 2,
                         self._actions.index(scrollAction)     : 1,
                         self._actions.index(smartClickAction) : 2,
                         self._actions.index(formAction)       : 2,
                         self._actions.index(touchAction)      : ((0, 2)[self._onAndroid])}

  def performNextAction(self):
    actionIndex = Generator().selectWithPriority(self._priorities)
    self._actions[actionIndex].perform()

  def sanitizeUrl(self, url):
    if not url.startswith('http'):
      url = 'http://' + url
    return url

  def getDefaultTitle(self, url):
    import re
    urlParts = re.split('://|\.', url)
    titleIndex = 0
    if 'http' in urlParts:
      titleIndex += 1
    if 'www' in urlParts:
      titleIndex += 1
    return urlParts[titleIndex]
