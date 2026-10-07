SYSTEMC_HOME := /usr/local/systemc-2.3.1a
SYSTEMC_LIBDIR := $(SYSTEMC_HOME)/lib-linux64

CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wall -Wextra -I$(SYSTEMC_HOME)/include
LDFLAGS := -L$(SYSTEMC_LIBDIR) -lsystemc -lm -Wl,-rpath,$(SYSTEMC_LIBDIR)

CPP_TARGETS := version1_formula1 version2_formula2 version3_formula22 compare_y
SYSTEMC_TARGET := version4_systemc_fixed

all: $(CPP_TARGETS) $(SYSTEMC_TARGET)

version1_formula1: version1_formula1.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

version2_formula2: version2_formula2.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

version3_formula22: version3_formula22.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

compare_y: compare_y.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

version4_systemc_fixed: version4_systemc_fixed.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(CPP_TARGETS) $(SYSTEMC_TARGET)