HEADER_DIR=/usr/local/include/RF24/..
LIB_DIR=/usr/local/lib/
LIBS=-lrf24 -lpthread

all: Debug

Debug: main.cpp
	g++ -I$(HEADER_DIR) -L$(LIB_DIR) $(LIBS) -fpermissive -o program main.cpp radio.cpp -std=c++11

Release: main.cpp
	g++ -I$(HEADER_DIR) -L$(LIB_DIR) $(LIBS) -fpermissive -o program main.cpp radio.cpp -std=c++11

clean:
	rm -f program