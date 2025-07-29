# Compiler and flags
CXX = g++
CXXFLAGS = -Iinclude
DEBUGFLAGS = -g -Iinclude   # Flags for debugging

# Directories for build output
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/objects
RELEASE_DIR = $(BUILD_DIR)/release
DEBUG_DIR = $(BUILD_DIR)/debug

# Object files and target names
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/basic_calculator.o $(OBJ_DIR)/math_utils.o
TARGET = $(RELEASE_DIR)/calculator
DEBUGTARGET = $(DEBUG_DIR)/calculator

# Default target to build the program (release)
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Rule for main.o (for both release and debug)
$(OBJ_DIR)/main.o: src/calculator/main.cpp include/calculator/basic_calculator.hpp
	$(CXX) -c $(CXXFLAGS) src/calculator/main.cpp -o $(OBJ_DIR)/main.o

# Rule for basic_calculator.o (for both release and debug)
$(OBJ_DIR)/basic_calculator.o: src/calculator/basic_calculator.cpp include/calculator/basic_calculator.hpp
	$(CXX) -c $(CXXFLAGS) src/calculator/basic_calculator.cpp -o $(OBJ_DIR)/basic_calculator.o

# Rule for math_utils.o (for both release and debug)
$(OBJ_DIR)/math_utils.o: src/utils/math_utils.cpp include/utils/math_utils.hpp
	$(CXX) -c $(CXXFLAGS) src/utils/math_utils.cpp -o $(OBJ_DIR)/math_utils.o

# Run target to execute the program after build
run: $(TARGET)
	./$(TARGET)

# Clean rule to remove object files and target
clean:
	rm -f $(OBJ) $(TARGET) $(DEBUGTARGET)

# Debug rule to build with debug flags
debug: CXXFLAGS += $(DEBUGFLAGS)  # Add debug flags
debug: $(DEBUGTARGET)  # Ensure the debug executable is built

# Create debug target
$(DEBUGTARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(DEBUGTARGET) $(DEBUGFLAGS)  

# Debug run target
debug_run: debug
	./$(DEBUGTARGET)

# Create necessary directories if they don't exist
$(OBJ_DIR) $(RELEASE_DIR) $(DEBUG_DIR):
	mkdir -p $@
