CC=g++
CFLAGS=-Wall -g

default: debug

debug: util.cpp working.cpp invoker.cpp
	g++ -o main util.cpp working.cpp invoker.cpp `pkg-config gtkmm-3.0 --cflags --libs` $(CFLAGS)
