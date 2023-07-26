#
# src/Makefile
#


ifeq (, $(shell which sst-config))
 $(error "No sst-config in $(PATH), add `sst-config` to your PATH")
endif

CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)
CPPFLAGS=-I./
OPTIMIZE_FLAGS=-O2

COMPONENT_LIB=libeventConverter.so

EVENTCONVERTER_SOURCES := $(wildcard *.cc)

EVENTCONVERTER_HEADERS := $(wildcard *.h)

EVENTCONVERTER_OBJS := $(patsubst %.cc,%.o,$(wildcard *.cc))

all: $(COMPONENT_LIB)

debug: CXXFLAGS += -DDEBUG -g
debug: $(COMPONENT_LIB)

$(COMPONENT_LIB): $(EVENTCONVERTER_OBJS)
	$(CXX) $(OPTIMIZE_FLAGS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ *.o
%.o:%.cc $(SIMPLEEXAMPLE_HEADERS)
	$(CXX) $(OPTIMIZE_FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $<
install: $(COMPONENT_LIB)
	sst-register eventConverter eventConverter_LIBDIR=$(CURDIR)
clean:
	rm -Rf *.o $(COMPONENT_LIB)
