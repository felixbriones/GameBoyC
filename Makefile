# Compiler and Linker
CC = gcc

# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Executable name
EXEC = $(BIN_DIR)/felixGB

# Compiler flags
CFLAGS = -I$(INC_DIR) -Wall -Wextra -g `sdl2-config --cflags`
# Linker flags
LDFLAGS = `sdl2-config --libs`

# Default target
all: $(EXEC)

# Link the object files to create the executable
$(EXEC): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)  # Create obj directory if it doesn't exist
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove generated files
clean:
	rm -rf $(OBJ_DIR)/*.o $(EXEC)

.PHONY: all clean
