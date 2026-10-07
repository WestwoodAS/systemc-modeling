LIB_DIR=-L/usr/local/systemc-2.3.1a/lib-linux64
INC_DIR=-I/usr/local/systemc-2.3.1a/include
LIB=-lsystemc
RPATH=-Wl,-rpath=/usr/local/systemc-2.3.1a/lib-linux64

APP=exercise2

all:
	g++ -o $(APP) $(APP).cpp $(LIB_DIR) $(INC_DIR) $(LIB) $(RPATH)

clean:
	rm -rf $(APP)
