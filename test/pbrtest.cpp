// Copyright (c) 2011 Andy Jost
// Please see the file LICENSE.txt in this distribution for license terms.

#include "pbr.hpp"
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <iostream>
#include <string>

#define foreach BOOST_FOREACH

using namespace boost::python;

// Count the items in the sequence.  Just tests that we can iterate over the
// sequence, casting each element to the correct type.
template<template <typename> class Range, typename Value>
int count_const(object seq)
{
  int num = 0;
  Value v;
  foreach(v, Range<Value>(seq)) { ++num; }
  return num;
}

template<typename MutableRange, typename Value>
int count_mutable(object seq)
{
  int num = 0;
  foreach(pbr::object_item v, MutableRange(seq))
  {
    Value v = extract<Value>(object(v));
    ++num;
  }
  return num;
}

BOOST_PYTHON_MODULE(pbrtest)
{
  // const and incrementable
  def("count_incrementable_object", count_const<pbr::incrementable_range, object>, "");
  def("count_incrementable_int", count_const<pbr::incrementable_range, int>, "");
  def("count_incrementable_str", count_const<pbr::incrementable_range, str>, "");

  // const and random_access
  def("count_random_access_object", count_const<pbr::random_access_range, object>, "");
  def("count_random_access_int", count_const<pbr::random_access_range, int>, "");
  def("count_random_access_str", count_const<pbr::random_access_range, str>, "");

  #if 0
  // mutable and incrementable
  def(
      "count_mutable_incrementable_object"
    , count_mutable<pbr::mutable_incrementable_range, pbr::object_item>
    , ""
    );
  #endif

  // mutable and random_access
  def(
      "count_mutable_random_object"
    , count_mutable<pbr::mutable_random_access_range, object>
    , ""
    );
  def(
      "count_mutable_random_int"
    , count_mutable<pbr::mutable_random_access_range, int>
    , ""
    );
  def(
      "count_mutable_random_str"
    , count_mutable<pbr::mutable_random_access_range, str>
    , ""
    );
}

