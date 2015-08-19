# Copyright (c) 2015 University of Szeged.
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from random import randint
from random import choice

class Generator:
  _charsLower = 'abcdefghijklmnopqrstuvwxyz'
  _charsUpper = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
  _numbers = '0123456789'
  _hex = _numbers + 'abcdef'
  # this comes from: http://stackoverflow.com/questions/31841467/generate-random-strings-of-chinese-characters-in-python
  _charsChinese = map(unichr, range(0x3400, 0x4e00) + range(0x4e00, 0xa000))

  def __init__(self):
    pass

  def randomInt(self, minInt=0, maxInt=255):
    return randint(minInt, maxInt)

  def select(self, elements):
    return choice(elements)

  def selectWithPriority(self, elements):
    totalPrioritySum = 0
    for priority in elements.values():
      totalPrioritySum += priority
    selected = randint(1, totalPrioritySum)
    prioritySum = 0
    for element in elements.keys():
      prioritySum += elements[element]
      if selected <= prioritySum:
        return element

  def charsLower(self):
    return choice(self._charsLower)

  def charsUpper(self):
    return choice(self._charsUpper)

  def chars(self):
    return choice(self._charsLower + self._charsUpper + self._numbers)

  def numbers(self):
    return choice(self._numbers)

  def hex(self):
    return choice(self._hex)

  def charsChinese(self):
    return choice(self._charsChinese)
