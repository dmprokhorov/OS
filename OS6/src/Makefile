CXX = g++
FLAGS = -lzmq -o

all: child_node server

child_node: calculation_node.cpp
	${CXX} calculation_node.cpp ${FLAGS} child_node -w

server: control_node.cpp
	${CXX} control_node.cpp ${FLAGS} server -w

clean:
	rm -r child_node server
