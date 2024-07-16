# Compiler
CXX=g++

# Target executable name
TARGET=app

# Source files
SRC=main.cpp

# Compiler flags, e.g., -std=c++11 for C++11 support
CXXFLAGS=-std=c++20

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean compiled files
clean:
	rm -f $(TARGET)
