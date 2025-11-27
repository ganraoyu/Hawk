# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -Iinclude -Itests -Wall -Wextra

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
SHARED_SRCS := $(filter-out $(SRC_DIR)/main.cpp $(SRC_DIR)/timer_test.cpp, $(SRCS))
SHARED_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SHARED_SRCS))

# Main programs
MAINS = main timer_test
MAIN_OBJS := $(patsubst %,$(BUILD_DIR)/%.o,$(MAINS))
MAIN_SRCS := $(patsubst %,$(SRC_DIR)/%.cpp,$(MAINS))

# Tests
TEST_SRCS := $(shell find $(TEST_DIR) -name '*.cpp')
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SRCS))

# Default target: build everything
all: $(BUILD_DIR) $(MAIN_OBJS) $(SHARED_OBJS) $(MAIN_OBJS:%=$(BUILD_DIR)/%) $(TEST_OBJS:%=$(BUILD_DIR)/%)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile shared .cpp files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile main programs
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test .cpp files recursively
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link main executables
$(BUILD_DIR)/%: $(BUILD_DIR)/%.o $(SHARED_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Link test executable (all tests into one)
$(BUILD_DIR)/run_tests: $(TEST_OBJS) $(SHARED_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Clean build
clean:
	rm -rf $(BUILD_DIR)

# Run main
run: $(BUILD_DIR)/main
	./$(BUILD_DIR)/main

# Run all tests
test: $(BUILD_DIR)/run_tests
	./$(BUILD_DIR)/run_tests
