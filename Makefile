# Makefile for LOB Simulator
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = lob_simulator.exe
SOURCES = main.cpp lob.cpp order_queue.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Rebuild everything
rebuild: clean all

# Dependencies
main.o: main.cpp lob.h order.h order_queue.h
lob.o: lob.cpp lob.h order.h order_queue.h
order_queue.o: order_queue.cpp order_queue.h order.h

.PHONY: all clean rebuild