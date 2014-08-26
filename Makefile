CXX = clang++
CXXFLAGS = $(shell llvm-config --cppflags) -std=c++11 -fPIC -O3 -Wall -Wextra -gdwarf-3 -D_FORTIFY_SOURCE=2 -fsanitize=address -fno-omit-frame-pointer
LDFLAGS = $(shell llvm-config --ldflags) -fsanitize=address

HEADERS = $(wildcard src/*.hpp)
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
LIBS = $(shell llvm-config --libs core)

TARGET = kaleidoscope

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

$(OBJECTS): $(HEADERS)

clean:
	rm -f src/*.o kaleidoscope
