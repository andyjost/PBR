#!/usr/local/bin/python

# Copyright (c) 2011 Andy Jost
# Please see the file LICENSE.txt in this distribution for license terms.

import pbrtest
import unittest

class PbrTest(unittest.TestCase):
  def setUp(self):
    # Build a collection of sequences, keyed by (sequence_type, content_type)
    SEQUENCES = {}
    SEQUENCES[(list,object)] = ['a', 5, {}, []]
    SEQUENCES[(list,int)] = list(range(10))
    SEQUENCES[(list,str)] = ['a', 'b', 'c', 'd', 'e']
    SEQUENCES[(tuple,object)] = ('a', 5, {}, [])
    SEQUENCES[(tuple,int)] = tuple(range(10))
    SEQUENCES[(tuple,str)] = ('a', 'b', 'c', 'd', 'e')
    # SEQUENCES[(str,str)] = "What's up, Chuck?"
    self.SEQUENCES = SEQUENCES

    MAPS = {}
    MAPS[(str,int)] = {'a':1, 'b':2, 'c':3}
    MAPS[(int,list)] = {1:[], 2:[1,2,3]}
    MAPS[(tuple,str)] = {(1,1):'11', (1,2):'12', (2,1):'21', (2,2):'22'}
    self.MAPS = MAPS

  def test_basic(self):
    """
    These basic tests simply check whether the C++ code can iterate over the
    sequence and return its length.  Since the C++ code is typed, we expect the
    iteration to fail when the passed elements don't match what C++ expects.
    """

    # incrementable
    self._check(pbrtest.count_incrementable_object, self.SEQUENCES, object, object)
    self._check(pbrtest.count_incrementable_int, self.SEQUENCES, object, int)
    self._check(pbrtest.count_incrementable_str, self.SEQUENCES, object, str)

    # random access
    self._check(pbrtest.count_random_access_object, self.SEQUENCES, (list,tuple), object)
    self._check(pbrtest.count_random_access_int, self.SEQUENCES, (list,tuple), int)
    self._check(pbrtest.count_random_access_str, self.SEQUENCES, (list,tuple), str)

    # mapping
    self._check(pbrtest.count_mapping_object_object, self.MAPS, object, object)
    self._check(pbrtest.count_mapping_str_object, self.MAPS, str, object)
    self._check(pbrtest.count_mapping_int_object, self.MAPS, int, object)
    self._check(pbrtest.count_mapping_tuple_object, self.MAPS, tuple, object)
    self._check(pbrtest.count_mapping_list_object, self.MAPS, list, object)
    self._check(pbrtest.count_mapping_object_str, self.MAPS, object, str)
    self._check(pbrtest.count_mapping_object_int, self.MAPS, object, int)
    self._check(pbrtest.count_mapping_object_tuple, self.MAPS, object, tuple)
    self._check(pbrtest.count_mapping_object_list, self.MAPS, object, list)
    self._check(pbrtest.count_mapping_str_int, self.MAPS, str, int)
    self._check(pbrtest.count_mapping_int_list, self.MAPS, int, list)
    self._check(pbrtest.count_mapping_tuple_str, self.MAPS, tuple, str)

  # Check each sequence in SEQUENCES.  For ones whose element type matches "type,"
  # call the function and confirm that it correctly iterates over the sequence.
  # For others, confirm that the iteration fails.
  #
  #   func  - the pbrtest function to call
  #   collection - which sequence collection to use (SEQUENCES or MAPS)
  #   type0/1 - an object or sequence corresponding the 0th/1st type of the
  #             collection keys
  #
  # For each key (k0,k1) in the collection if the two keys are both subtypes of
  # an element in type0 and type1, respectively, then the C++ function should
  # be able to iterate the corresponding collection item.
  # 
  def _check(self, func, collection, type0, type1):
    for key in collection.iterkeys():
      item = collection[key]
      msg = '%s -> %s' % ((type0, type1), item)

      if issubclass(key[0], type0) and issubclass(key[1], type1):
        # This func should be able to iterate item
        self.assertTrue(func(item) == len(item), msg=msg)
      else:
        # This func should not be able to iterate item
        print msg
        self.assertRaises(Exception, lambda: func(item))

if __name__ == '__main__':
  unittest.main()
    

