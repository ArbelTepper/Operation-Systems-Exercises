# Compiler and compiler flags
CXX = g++ # Using g++ as the compiler
CXXFLAGS = -Wall -std=c++11 # Common flags, plus C++11 standard

# Project files
SOURCES = main.cpp Producer.cpp Dispatcher.cpp CoEditor.cpp ScreenManager.cpp BoundBuffer.cpp UnboundedBuffer.cpp
HEADERS = Producer.h Dispatcher.h CoEditor.h ScreenManager.h BoundBuffer.h UnboundedBuffer.h
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = myApplication

# The first rule is the one executed when no parameters are fed to the Makefile
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

# To obtain object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Phony targets
.PHONY: all clean
