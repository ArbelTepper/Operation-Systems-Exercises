# Compiler and compiler flags
CXX = clang # Using g++ as the compiler
CXXFLAGS = -Wall -std=c++20 # Common flags, plus C++11 standard

# Project files
SOURCES = main.cpp Producer.cpp Dispatcher.cpp CoEditor.cpp ScreenManager.cpp BoundedBuffer.cpp UnboundedBuffer.cpp
HEADERS = Producer.h Dispatcher.h CoEditor.h ScreenManager.h BoundedBuffer.h UnboundedBuffer.h
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = myApplication

# The first rule is the one executed when no parameters are fed to the Makefile
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -lstdc++ $(OBJECTS) -o $@

# To obtain object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Phony targets
.PHONY: all clean
