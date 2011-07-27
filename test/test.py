#!/usr/local/bin/python

# Copyright (c) 2011 Andy Jost
# Please see the file LICENSE.txt in this distribution for license terms.

import pbrtest
import unittest

class PbrTest(unittest.TestCase):
  def setUp(self):
    # Build a container of collections, keyed by (container_type, content_type)
    STUFF = {}
    STUFF[(list,object)] = ['a', 5, {}, []]
    STUFF[(list,int)] = list(range(10))
    STUFF[(list,str)] = ['a', 'b', 'c', 'd', 'e']
    STUFF[(tuple,object)] = ('a', 5, {}, [])
    STUFF[(tuple,int)] = tuple(range(10))
    STUFF[(tuple,str)] = ('a', 'b', 'c', 'd', 'e')
    # STUFF[(str,str)] = "What's up, Chuck?"
    self.STUFF = STUFF

  def test_basic(self):
    # incrementable
    self._check_sequence(pbrtest.count_incrementable_object, object, object)
    self._check_sequence(pbrtest.count_incrementable_int, object, int)
    self._check_sequence(pbrtest.count_incrementable_str, object, str)

    # random access
    self._check_sequence(pbrtest.count_random_access_object, (list,tuple), object)
    self._check_sequence(pbrtest.count_random_access_int, (list,tuple), int)
    self._check_sequence(pbrtest.count_random_access_str, (list,tuple), str)

  # Check each sequence in STUFF.  For ones whose element type matches "type,"
  # call the function and confirm that it correctly iterates over the sequence.
  # For others, confirm that the iteration fails.
  def _check_sequence(self, func, container_type, item_type):
    # Check incrementable sequences of ints.
    for key in self.STUFF.iterkeys():
      stuff = self.STUFF[key]

      # Only sequences of the correct type should be iterable with the supplied
      # function.
      if issubclass(key[0], container_type) and issubclass(key[1], item_type):
        self.assertTrue(func(stuff) == len(stuff))
      else:
        self.assertRaises(Exception, lambda: func(stuff))

if __name__ == '__main__':
  unittest.main()
    

