# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import os

class Sprocket:

  _driver = None

  def __init__(self, android = False):

    try:
      from selenium import webdriver
    except ImportError:
      raise ImportError('You need to install Selenium. https://pypi.python.org/pypi/selenium or pip install selenium')
    capabilities = None
    driverPath = None
    if android:
      capabilities = {'chromeOptions': { 'androidPackage': 'hu.uszeged.sprocket_apk'}}
    else:
      sprocketPath = None
      try:
        sprocketPath = os.environ['SPROCKET_PATH']
        capabilities = {'chromeOptions': { 'binary': sprocketPath + '/sprocket'}}
      except Exception:
        raise Exception('Sprocket should be in SPROCKET_PATH variable')
    try:
      driverPath = os.environ['DRIVER_PATH']
    except Exception:
      raise Exception('Chromedriver should be in DRIVER_PATH variable')
    self._driver = webdriver.Chrome(executable_path=driverPath + '/chromedriver',
                                    desired_capabilities=capabilities)

  def driver(self):
    # This will be private
    # only use for dev purposes
    return self._driver

  def action(self):
    from selenium import webdriver
    return webdriver.ActionChains(self._driver)

  def touch(self):
    from selenium import webdriver
    return webdriver.TouchActions(self._driver)

  def goto(self, url = "http://www.google.com"):
    self._driver.get(url)

  def title(self):
    return self._driver.title

  def name(self):
    return self._driver.name

  def close(self):
    self._driver.close()

  def closeWindow(self, name):
    self._driver.close_window(name)

  def currentWindowHandle(self):
    return self._driver.current_window_handle

  def windowHandles(self):
    return self._driver.window_handles

  def switchToWindow(self, handle):
    self._driver.switch_to_window(handle)

  def quit(self):
    self._driver.quit()

  def back(self):
    self._driver.back()

  def forward(self):
    self._driver.forward()

  def currentUrl(self):
    return self._driver.current_url

  def findAllElements(self):
    return self._driver.find_elements_by_css_selector("*")

  def selectBody(self):
    return self._driver.find_element_by_tag_name("body")

  def findElementsByTagName(self, name):
    return self._driver.find_elements_by_tag_name(name)

  def findElementsByXPath(self, name):
    return self._driver.find_elements_by_xpath(name)

  def executeScript(self, script, *args):
    self._driver.execute_script(script, args)
