# Copyright (c) 2011 Andy Jost
# Please see the file LICENSE.txt in this distribution for license terms.

# == SETUP ==================================================
CC := g++
CFLAGS := -Wall -O -fPIC
LDFLAGS := -shared

# This variable points to your Python installation.
PYTHON_INCLUDE := /usr/include/python2.6

# This variable points to your Boost installation. PBR was tested with Boost
# version 1.46.
BOOST_INCLUDE := $(HOME)/libs/boost/

PBR_INCLUDE := ../include
INCLUDES := -I $(PYTHON_INCLUDE) -I $(BOOST_INCLUDE) -I $(PBR_INCLUDE)

# This variable points to the Boost library location.  PBR requires the Boost
# python library.
BOOST_PYTHON_LIB_PATH := $(HOME)/libs/boost/lib
LINK_LIBS := $(BOOST_PYTHON_LIB_PATH)/libboost_python-g.so


# == TARGETS ================================================
SOURCES := pbrtest.cpp
OBJECTS = $(SOURCES:.cpp=.o)
PLUGIN := pbrtest.so


# == RULES ==================================================
all: pbrtest.so

.cpp.o:
	$(CC) -c $< $(CFLAGS) $(INCLUDES)

$(PLUGIN): $(OBJECTS)
	$(CC) $(LDFLAGS) $< $(LINK_LIBS) -o $@

clean:
	rm -rf *.o *.so


# == TESTS ==================================================
# Run the tests found in test.py. 
test: pbrtest.so
	@NULL="$(shell                                                      \
	    LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}:$(BOOST_PYTHON_LIB_PATH)" \
			python test.py                                                  \
			)"
