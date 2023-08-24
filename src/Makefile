#
# src/Makefile
#


ifeq (, $(shell which sst-config))
 $(error "No sst-config in $(PATH), add `sst-config` to your PATH")
endif

CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)
ELEMENT_HEADERS=$(shell sst-config SST_ELEMENT_LIBRARY SST_ELEMENT_LIBRARY_HOME)/include
CPPFLAGS= -I/scratch/shared/apps/sst/13.0.0/elements/include -I./
OPTIMIZE_FLAGS=-O2

COMPONENT_LIB=libmemoryRouter.so

MEMORYROUTER_SOURCES := $(wildcard *.cc)

MEMORYROUTER_HEADERS := $(wildcard *.h)

MEMORYROUTER_OBJS := $(patsubst %.cc,%.o,$(wildcard *.cc))

all: $(COMPONENT_LIB)

debug: CXXFLAGS += -DDEBUG -g
debug: $(COMPONENT_LIB)

$(COMPONENT_LIB): $(MEMORYROUTER_OBJS)
	$(CXX) $(OPTIMIZE_FLAGS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ *.o
%.o:%.cc $(SIMPLEEXAMPLE_HEADERS)
	$(CXX) $(OPTIMIZE_FLAGS) $(CXXFLAGS) $(CPPFLAGS) -I$(ELEMENT_HEADERS) -c $<
install: $(COMPONENT_LIB)
	sst-register memoryRouter memoryRouter_LIBDIR=$(CURDIR)
clean:
	rm -Rf *.o $(COMPONENT_LIB)
