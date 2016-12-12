all: main.cpp
	g++ main.cpp -std=c++11 `pkg-config --cflags --libs igraph` -ligraph -lglpk -lpthread -o main

