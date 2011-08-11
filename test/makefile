# Copyright (c) 2011 Andy Jost
# Please see the file LICENSE.txt in this distribution for license terms.

SOURCES := pbrtest.cpp
OBJECTS := $(SOURCES:.cpp=.o)
SHARED_LIBS := pbrtest.so

# == TESTS ==================================================
# Run the tests found in test.py. 
test: pbrtest.so
	@LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}:$(BOOST_PYTHON_LIB_PATH)" python test.py

include ../make.include

