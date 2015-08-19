# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

class Action(object):

  _type = None
  _browser = None

  def __init__(self, browser):
    self._browser = browser

  def perform(self):
    raise NotImplementedError

  def debugLog(self, message):
    print self._type, "=>" , message, " at ", self._browser.currentUrl()
