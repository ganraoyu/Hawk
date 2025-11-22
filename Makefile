# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -Iinclude -Wall -Wextra

# Directories
SRC_DIR = src
BUILD_DIR = build

# Executable name
TARGET = main

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Object files in build/
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link object files into executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Compile .cpp to .o in build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Run program
run: $(TARGET)
	./$(TARGET)
