// Copyright (c) 2011 Andy Jost
// Please see the file LICENSE.txt in this distribution for license terms.

#include "pbr.hpp"
#include "pbr_adaptors.hpp"
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/heap_algorithm.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_product.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <iostream>
#include <string>
#include <algorithm>

#define foreach BOOST_FOREACH

using namespace boost::python;
#if 0
void mytest(object seq)
{
  using pbr::adaptors::map_keys;

  // pbr::mapping_range<str, object> my_map(seq);

  // list lst;
  // pbr::random_access_range<pbr::object_item> my_range(lst);
  // push_back(my_range, my_map | map_keys);
  // sort(my_range);

  // foreach(str key, my_map | map_keys)
  // {
  //   std::cout << extract<std::string>(key)() << std::endl;
  // }
}
#endif

#if 0
void foo(object seq, object func)
{
  list scratch(seq);
  str x(scratch);
  std::cout << extract<std::string>(x)() << std::endl;

  pbr::random_access_range3<pbr::object_item> my_range(scratch);

  // make_heap(my_range);
  std::make_heap(my_range.begin(), my_range.end());
  x = str(scratch);
  std::cout << extract<std::string>(x)() << std::endl;

  int i =0;
  while(!empty(my_range))
  {
    std::cout << "size: " << my_range.size() << " begin: " << my_range.begin().m_loc << " end: " << my_range.end().m_loc << std::endl;
    object arg = my_range.front();
    func(arg);
    pop_heap(my_range);
    if(i++ == 15) break;
  }
}
#endif

// Count the items in the sequence.  Just tests that we can iterate over the
// sequence, casting each element to the expected type.
template<typename Range>
int count(object seq)
{
  int num = 0;
  typename Range::value_type v;
  foreach(v, Range(seq)) { ++num; }
  return num;
}

// Modify a sequence by applying the expression "item+=1" to each item.
void increment_sequence(object seq)
{
  using namespace pbr;
  foreach(object_item item, random_access_range<object_item>(seq))
  {
    item += 1;
  }
}

// Modify a sequence by applying the expression "item*=2" to each item.
void double_sequence(object seq)
{
  using namespace pbr;
  foreach(object_item item, random_access_range<object_item>(seq))
  {
    item *= 2;
  }
}

// Shuffle the items in a sequence "randomly".
void shuffle_sequence(object seq)
{
  using namespace pbr;
  random_access_range<object_item> range(seq);
  std::random_shuffle(range.begin(), range.end());
}

BOOST_PYTHON_MODULE(pbrtest)
{
  // Make versions of the test functions for each of these types.
  #define PBR_py_types (object)(int)(str)(tuple)(list)

  // const and incrementable
  // Make a function count_incrementable_<type> for each type in the above list.
  #define PBR_def(r, data, tp) \
    def(                     \
        "count_incrementable_" BOOST_PP_STRINGIZE(tp)  \
      , count<pbr::incrementable_range<tp> >           \
      , ""                   \
      );
  BOOST_PP_SEQ_FOR_EACH(PBR_def,,PBR_py_types)
  #undef PBR_def

  // const and random_access
  // Make a function count_random_access_<type> for each type in the above list.
  #define PBR_def(r, data, tp) \
      def(                   \
          "count_random_access_" BOOST_PP_STRINGIZE(tp) \
        , count<pbr::random_access_range<tp> >          \
        , ""                 \
        );
  BOOST_PP_SEQ_FOR_EACH(PBR_def,,PBR_py_types)
  #undef PBR_def

  // const and random_access mapping
  // Make a function count_mapping_<type0>_<type1> for each pair of types in
  // the cross product of the above list with itself.
  #define PBR_def(tp0, tp1) \
      def(                \
          "count_mapping_" BOOST_PP_STRINGIZE(tp0) "_" BOOST_PP_STRINGIZE(tp1) \
        , count<pbr::mapping_range<tp0, tp1> >                                 \
        , ""              \
        );
  #define PBR_def2(r, product) PBR_def(BOOST_PP_SEQ_ELEM(0, product), BOOST_PP_SEQ_ELEM(1, product))
  BOOST_PP_SEQ_FOR_EACH_PRODUCT(PBR_def2, (PBR_py_types)(PBR_py_types))
  #undef PBR_def
  #undef PBR_def2

  def("increment_sequence", increment_sequence, "");
  def("double_sequence", double_sequence, "");
  def("shuffle_sequence", shuffle_sequence, "");
  // def("mytest", mytest, "");
  // def("foo", foo, "");
}

