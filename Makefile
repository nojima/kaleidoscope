CXX = clang++
CXXFLAGS = -std=c++1y -O3 -Wall -Wextra -I/home/nojima/src/libcxx/include
LDFLAGS = -stdlib=libc++ -nostdinc++ -L/home/nojima/src/libcxx/build/lib

HEADERS = $(wildcard src/*.hpp)
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

EXE=kaleidoscope

all: $(EXE)

$(EXE): $(OBJECTS)

$(OBJECTS): $(HEADERS)
