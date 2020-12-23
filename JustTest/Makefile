COMPILER=g++
CFLAGS=-std=c++17 -lstdc++ -lprotobuf -lpthread -lm -lX11
SFMLFLAGS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
BUILDFILES=Source.cpp PlayerStat.pb.cc

build:
	$(COMPILER) $(BUILDFILES) $(CFLAGS) $(SFMLFLAGS)