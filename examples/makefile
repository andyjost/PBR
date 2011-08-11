# Copyright (c) 2011 Andy Jost
# Please see the file LICENSE.txt in this distribution for license terms.

SOURCES := example1.cpp
OBJECTS := $(SOURCES:.cpp=.o)
SHARED_LIBS := example1.so

example1: example1.so
	@LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}:$(BOOST_PYTHON_LIB_PATH)" python example1.py

include ../make.include
