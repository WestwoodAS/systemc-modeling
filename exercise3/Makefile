LIB_DIR=-L/usr/local/systemc-2.3.1a/lib-linux64
INC_DIR=-I/usr/local/systemc-2.3.1a/include
LIB=-lsystemc
RPATH=-Wl,-rpath=/usr/local/systemc-2.3.1a/lib-linux64
CXXFLAGS=-std=c++11

TARGET=main
SRC=main.cpp stimgen.cpp adder.cpp monitor.cpp

all:
	g++ $(CXXFLAGS) -o $(TARGET) $(SRC) $(LIB_DIR) $(INC_DIR) $(LIB) $(RPATH)

clean:
	rm -rf $(TARGET)