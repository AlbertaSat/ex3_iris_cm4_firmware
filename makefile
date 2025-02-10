#Tool Def
CC ?= gcc
CXX ?= g++

#Settings
SRC_DIR = ./src
INC_DIR = ./inc
GLOBAL_INC_DIR = /usr/include
TEST_DIR = ./tests
BUILD_DIR = ./build
NAME = main

#Remove compiled object files
.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*
	
#Search for all local Header Files 
CFLAGS += -I$(INC_DIR)

#Search for all Global Header Files
CFLAGS += -I$(GLOBAL_INC_DIR)

#Source Files
CSOURCES += $(wildcard $(SRC_DIR)/*.c)

#Compiler Flags
CFLAGS += -Wall

#Linker Flags
LDFLAGS += -lgpiod

# Debug target (compiled with debug symbols)
debug: CFLAGS += -g -O0   # Add debugging flags to CFLAGS (Disable Optimi)
debug: clean $(NAME)             # Use the same build process for the debug target

#Generate names for output object files (*.o)
COBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CSOURCES))

# Ensure build directory exists
$(NAME): | $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

#Default Rule: Build Application
.PHONY: all
all: clean $(NAME)

# Build Components
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o  $@

# Build the target Application
.PHONY: $(NAME)
$(NAME): $(COBJECTS)
	$(CC) $(COBJECTS) -o $(BUILD_DIR)/$(NAME) $(LDFLAGS)

