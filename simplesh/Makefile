

all: simplesh

clean:
	rm -f simplesh.o simplesh

simplesh.o: simplesh.cpp
	g++ -std=c++11 -c simplesh.cpp

simplesh: simplesh.o
	g++ -std=c++11 simplesh.o -o simplesh
