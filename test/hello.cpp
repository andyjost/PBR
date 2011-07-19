#include "pbr.hpp"
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <iostream>
#include <string>

#define foreach BOOST_FOREACH

using namespace boost::python;

void printseq(object seq)
{
  foreach(object obj, pbr::incrementable_range<object>(seq))
  {
    str const tmp0(obj);
    std::string const tmp1 = extract<std::string>(tmp0);
    std::cout << tmp1 << " ";
  }
  std::cout << std::endl;
}

void dumb(object seq)
{
  pbr::random_access_range<object> range(seq);
  std::cout << (boost::end(range) - boost::begin(range)) << std::endl;
}

void shuffle(object seq)
{
  pbr::mutable_random_access_range<object> range(seq);
  std::random_shuffle(boost::begin(range), boost::end(range));
}

BOOST_PYTHON_MODULE(hello)
{
  def("printseq", printseq, "");
  def("dumb", dumb, "");
  def("shuffle", shuffle, "");
}

