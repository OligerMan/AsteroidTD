COMPILER=g++
CFLAGS=-std=c++17 -lstdc++ -lprotobuf -lpthread -lm -lX11 -o
SFMLFLAGS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
BUILDFILES=Source.cpp PlayerStat.pb.cc
OUT=asteroid

build:
	$(COMPILER) $(BUILDFILES) $(CFLAGS) $(OUT) $(SFMLFLAGS)