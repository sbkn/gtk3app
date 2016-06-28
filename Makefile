CC=g++
CFLAGS=-Wall -g

default: program

program: util.cpp test.cpp
	g++ -o test util.cpp test.cpp `pkg-config gtkmm-3.0 --cflags --libs` $(CFLAGS)
