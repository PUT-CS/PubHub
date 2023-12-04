.DEFAULT_GOAL := all

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

NLOHMANN_JSON = $(LIB_DIR)/json.hpp
NLOHMANN_JSON_PCH = $(NLOHMANN_JSON).pch

# Compiler and compiler flags
CC = clang++
CC_COMMON_FLAGS = -Wall -Wextra -std=c++20 -pipe
CC_CODE_FLAGS =  $(CC_COMMON_FLAGS) -include-pch $(NLOHMANN_JSON_PCH) -ftime-trace
CC_JSON_FLAGS = -o $(NLOHMANN_JSON_PCH) $(CC_COMMON_FLAGS)

# Source files found in subdirectories
SOURCES = $(shell find  $(SRC_DIR) -type f -name '*.cpp')

# Object files in a single directory
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Executable name
EXECUTABLE = $(BIN_DIR)/pubhub
LIB_DIR = $(SRC_DIR)/libpubhub


all: json $(EXECUTABLE)

#$(NLOHMANN_JSON_PCH):
json:
	$(CC) $(CC_JSON_FLAGS) $(NLOHMANN_JSON)

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CC_CODE_FLAGS) -c $< -o $@

# Rule to link object files into the executable
$(EXECUTABLE): $(OBJECTS)
	mkdir $(BIN_DIR) 2> /dev/null || true
	$(CC) $(CC_CODE_FLAGS) $^ -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run clean
