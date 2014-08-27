CXX = clang++

LANGUAGE_OPTIONS = -std=c++11
WARNING_OPTIONS = -Wall -Wextra -Woverloaded-virtual -Weffc++
OPTIMIZATION_OPTIONS = -O3 -fno-omit-frame-pointer
CODE_GENERATION_OPTIONS = -fPIC
PREPROCESSOR_OPTIONS = -MMD -MP
LLVM_OPTIONS = $(shell llvm-config --cppflags | sed -e 's/-DNDEBUG //')
DEBUGGING_OPTIONS = -gdwarf-3 -fsanitize=address
CXXFLAGS = $(LANGUAGE_OPTIONS) $(WARNING_OPTIONS) $(OPTIMIZATION_OPTIONS) $(CODE_GENERATION_OPTIONS) $(PREPROCESSOR_OPTIONS) $(LLVM_OPTIONS) $(DEBUGGING_OPTIONS)

LDFLAGS = $(shell llvm-config --ldflags) -fsanitize=address
LIBS = $(shell llvm-config --libs core)

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst src/%, obj/%, $(patsubst %.cpp, %.o, $(SOURCES)))
DEPENDS = $(patsubst %.o, %.d, $(OBJECTS))

TARGET = kaleidoscope

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

obj/%.o: src/%.cpp
	@mkdir -p obj/
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf obj/ $(TARGET)

-include $(DEPENDS)
