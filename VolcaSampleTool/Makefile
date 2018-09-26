IDIR = include
CXX=clang++
CXXFLAGS +=-Iinclude -Wc++11-extensions -std=c++11
CC=clang
CFLAGS=-Wall

LDFLAGS += -framework CoreAudio -framework CoreFoundation -framework Accelerate -lSDL2 -lpthread -stdlib=libc++ -ldl 

OUTPUTDIR = ./build/
MKDIR = mkdir -p $(OUTPUTDIR)

EXEC = run
CXXSOURCES += $(wildcard src/*.cpp src/*/*.cpp)
CSOURCES += $(wildcard src/*.c src/*/*.c)
OBJECTS = $(SOURCES:.cpp=.o .c=o)

all: volcatool

volcatool:
	$(MKDIR)
	$(CC) $(CSOURCES) $(CFLAGS) -c
	$(CXX) -o $(OUTPUTDIR)/VolcaTool $(CXXFLAGS) $(LDFLAGS) *.o $(CXXSOURCES) 

clean:
	rm -rf $(OUTPUTDIR) *.o

