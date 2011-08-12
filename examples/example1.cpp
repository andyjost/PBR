// Copyright (c) 2011 Andy Jost
// Please see the file LICENSE.txt in this distribution for license terms.

// This file contains the examples from the wiki Introduction page.
// https://github.com/andyjost/PBR/wiki/Introduction

#include "pbr.hpp"
#include "pbr_adaptors.hpp"
#include <boost/foreach.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <iostream>
#include <string>

using namespace pbr;
using namespace boost::python;

#define foreach BOOST_FOREACH

// A dummy function that just prints an item.
void fn(object item)
{
  str s(item);
  std::cout << extract<std::string>(s)() << std::endl;
}

void foreach_(object py_iterable_object)
{
  incrementable_range<object> my_range(py_iterable_object);
  foreach(object item, my_range) { fn(item); }
}

void dictKeys(object py_mapping_object)
{
  using namespace pbr::adaptors;
  mapping_range<str, object> my_range(py_mapping_object);
  for_each(my_range | map_keys, fn);
}

void modifySeq(object py_seq_object)
{
  random_access_range<object_item> my_range(py_seq_object);
  foreach(object_item item, my_range) { item *= item; }
  boost::sort(my_range);
}
int as_int(object obj)
{
  long_ lng(obj);
  int const i = extract<int>(lng);
  return i;
}

BOOST_PYTHON_MODULE(example1)
{
  def("foreach", foreach_, "foreach example");
  def("dictKeys", dictKeys, "iterate over the keys in a dict");
  def("modifySeq", modifySeq, "modify a sequence in place");
  def("as_int", as_int, "simulates int(x)");
}

