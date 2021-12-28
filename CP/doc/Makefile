CXX = g++
FLAGS = -lzmq -o

all: client server

client: client.cpp
	${CXX} client.cpp ${FLAGS} client -w

server: server.cpp
	${CXX} server.cpp ${FLAGS} server -w

clean:
	rm -r client server
