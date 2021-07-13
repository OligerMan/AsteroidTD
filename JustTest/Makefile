COMPILER=g++
CFLAGS=-std=c++17 -lstdc++ -lprotobuf -lpthread -lm -lX11 -fsanitize=address -o
SFMLFLAGS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
BUILDFILES=Source.cpp PlayerStat.pb.cc
OUT=asteroid
CURDIR=${PWD}
PROTOC_SUFFIX=*.proto
PROTOC_OPTS=--cpp_out=$(CURDIR) --proto_path=$(CURDIR)

all: protoc
	$(COMPILER) $(BUILDFILES) $(CFLAGS) $(OUT) $(SFMLFLAGS)


protoc:
	protoc $(PROTOC_OPTS) $(PROTOC_SUFFIX)


osxapp:
	mkdir -p ../AsteroidTD.app/Contents/MacOS
	mkdir -p ../AsteroidTD.app/Contents/Resources
	cp -f ../macosx/Info.plist ../AsteroidTD.app/Contents/Info.plist
	cp -f ../macosx/installscript_osx.vdf ../AsteroidTD.app/installscript_osx.vdf
	cp -rf $(CURDIR)/* ../AsteroidTD.app/Contents/Resources/
	cp -f $(CURDIR)/asteroid ../AsteroidTD.app/Contents/MacOS/asteroid