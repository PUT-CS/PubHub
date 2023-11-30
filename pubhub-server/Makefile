# Compiler and compiler flags
CC = g++
CCFLAGS = -Wall -Wextra -std=c++20

# Directories
SRC_DIR = src

OBJ_DIR = obj
BIN_DIR = bin

# Source files found in subdirectories
SOURCES = $(shell find  $(SRC_DIR) -type f -name '*.cpp')

# Object files in a single directory
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Executable name
EXECUTABLE = $(BIN_DIR)/pubhub
LIB_DIR = $(SRC_DIR)/libpubhub
NLOHMANN_JSON_GCH = $(LIB_DIR)/json.hpp.gch


all: $(NLOHAMNN_JSON_GCH) $(EXECUTABLE)

$(NLOHMANN_JSON_GCH):
	echo "COMPILING JSON"
	$(CC) $(CCFLAGS) $(SRC_DIR)/libpubhub/json.hpp
	echo "DONE COMPILING JSON"

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CCFLAGS) -c $< -o $@

# Rule to link object files into the executable
$(EXECUTABLE): $(OBJECTS)
	mkdir $(BIN_DIR) 2> /dev/null || true
	$(CC) $(CCFLAGS) $^ -o $@

run: $(NLOHMANN_JSON_GCH) $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run clean
