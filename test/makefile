# Copyright (c) 2011 Andy Jost
# Please see the file LICENSE.txt in this distribution for license terms.

# Check the entries marked with >>>>>.  If you don't keep Boost or Python in
# the expected locations, update the associated variables.

# == SETUP ==================================================
CC := g++
CFLAGS := -Wall -O0 -fPIC -g
LDFLAGS := -shared

# >>>>> This variable points to your Python installation.
PYTHON_INCLUDE := /usr/include/python2.6

# >>>>> This variable points to your Boost installation. PBR was tested with
# >>>>> Boost version 1.46.
BOOST_INCLUDE := $(HOME)/libs/boost/

PBR_INCLUDE := ../include
INCLUDE_DEPENDS := $(PBR_INCLUDE)/pbr.hpp
INCLUDES := -I $(PYTHON_INCLUDE) -I $(BOOST_INCLUDE) -I $(PBR_INCLUDE)

# >>>>> This variable points to the Boost library location.  PBR requires the
# >>>>> Boost python runtime library.
BOOST_PYTHON_LIB_PATH := $(HOME)/libs/boost/lib
LINK_LIBS := $(BOOST_PYTHON_LIB_PATH)/libboost_python.so

SOURCES := pbrtest.cpp
OBJECTS = $(SOURCES:.cpp=.o)
SHARED_LIBS := pbrtest.so


# == RULES ==================================================
all: $(SHARED_LIBS)

.cpp.o: $(INCLUDE_DEPENDS)
	$(CC) -c $< $(CFLAGS) $(INCLUDES)

$(SHARED_LIBS): $(OBJECTS) $(INCLUDE_DEPENDS)
	$(CC) $(LDFLAGS) $< $(LINK_LIBS) -o $@

clean:
	rm -rf *.o *.so *.log

.PHONY phony:

# == TESTS ==================================================
# Run the tests found in test.py. 
test: pbrtest.so
	@NULL="$(shell                                                      \
      LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}:$(BOOST_PYTHON_LIB_PATH)" \
      python test.py                                                  \
    | tee test.log                                                    \
    )"
