PROJECT = volcatool
IDIR = include
CXX=clang++
CXXFLAGS +=-I./include -Wc++11-extensions -std=c++11
CC=clang
CFLAGS=-Wall

LDFLAGS += -framework CoreAudio -framework CoreFoundation -framework Accelerate -lSDL2 -lpthread -stdlib=libc++ -ldl 

OUTPUTDIR = ./build/
MKDIR = mkdir -p $(OUTPUTDIR)

EXEC = run
CXXSOURCES += $(wildcard src/*.cpp src/*/*.cpp)
CSOURCES += $(wildcard src/*.c src/*/*.c)
OBJECTS = $(SOURCES:.cpp=.o .c=o)

all: $(PROJECT)

$(PROJECT):
	$(MKDIR)
	$(CC) $(CSOURCES) $(CFLAGS) -c
	$(CXX) -o $(OUTPUTDIR)/$(PROJECT) $(CXXFLAGS) $(LDFLAGS) *.o $(CXXSOURCES) 

.PHONY : clean
clean:
	rm -rf $(OUTPUTDIR) *.o

