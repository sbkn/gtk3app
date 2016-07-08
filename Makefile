CC=g++
CFLAGS=-Wall -g

default: debug

debug: util.cpp working.cpp invoker.cpp
	g++ -o invoker util.cpp working.cpp invoker.cpp `pkg-config gtkmm-3.0 --cflags --libs` $(CFLAGS)

macos: util.cpp working.cpp invoker.cpp
	g++ -std=c++11 -o invoker util.cpp working.cpp invoker.cpp `pkg-config /usr/local/Cellar/gtkmm3/*/lib/pkgconfig/gtkmm-3.0.pc --cflags --libs` $(CFLAGS)
