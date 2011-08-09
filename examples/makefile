SOURCES := example1.cpp
OBJECTS := $(SOURCES:.cpp=.o)
SHARED_LIBS := example1.so

example1: example1.so
	$(shell                                                             \
      LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}:$(BOOST_PYTHON_LIB_PATH)" \
      python example1.py                                              \
    )

include ../make.include
