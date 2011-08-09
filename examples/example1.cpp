// Copyright (c) 2011 Andy Jost
// Please see the file LICENSE.txt in this distribution for license terms.

#include "pbr.hpp"
#include <iostream>

using namespace boost::python;

void py_main()
{
  std::cout << "main() called" << std::endl;
}

BOOST_PYTHON_MODULE(example1)
{
  def("main", py_main, "");
}

