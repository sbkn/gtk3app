CC=g++
CFLAGS=-Wall -g

default: program

program: util.cpp working.cpp main.cpp
	g++ -o main util.cpp working.cpp main.cpp `pkg-config gtkmm-3.0 --cflags --libs` $(CFLAGS)
