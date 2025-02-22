#Tool Def
CC ?= gcc
CXX ?= g++

#Settings
SRC_DIR = ./src
SRC_MAIN := ./main_service
SRC_SPI := ./spi_service

INC_DIR = ./inc
GLOBAL_INC_DIR = ./usr/include

TEST_DIR = ./tests

MAIN_BUILD_DIR = ./build/main_build
SPI_BUILD_DIR = ./build/spi_build

#Remove compiled object files
.PHONY: clean
clean:
	rm -f $(MAIN_BUILD_DIR)/*
	rm -f $(SPI_BUILD_DIR)/*

#Search for all local Header Files 
CFLAGS += -I$(INC_DIR)
CFLAGS += -I$(SRC_MAIN)
CFLAGS += -I$(SRC_SPI)

#Search for all Global Header Files
CFLAGS += -I$(GLOBAL_INC_DIR)

#Compiler Flags
CFLAGS += -Wall

#Source Files
CSOURCES += $(wildcard $(SRC_DIR)/*.c)
MAIN_CSOURCES += $(wildcard $(SRC_MAIN)/*.c)
SPI_CSOURCES += $(wildcard $(SRC_SPI)/*.c)

#Linker Flags
LDFLAGS += -lgpiod
LDFLAGS += -lssl
LDFLAGS += -lcrypto

# Debug target (compiled with debug symbols)
debug_one: CFLAGS += -g -O0   # Add debugging flags to CFLAGS (Disable Optimi)
debug_one: one_service

debug_two: CFLAGS += -g -O0   # Add debugging flags to CFLAGS (Disable Optimi)
debug_two: two_service


### Create Executables ###
# Create combined SPI + Main Service
.PHONY: one_service
one_service: MAIN_CFLAGS = $(CFLAGS)
one_service: MAIN_CFLAGS += -DONE_SERVICE
one_service: clean main_service

# Create separate SPI Service and Main Service
.PHONY: two_service
two_service: MAIN_CFLAGS = $(CFLAGS)
two_service: MAIN_CFLAGS += -DTWO_SERVICE
two_service: clean main_service spi_service


### Generate names for output object files (*.o) ###
#	MAIN - Object Files for Main Theia Service
#	SPI  - Object Files for Dedicated SPI Service (Optional)
MAIN_COBJECTS = $(patsubst $(SRC_MAIN)/%.c, $(MAIN_BUILD_DIR)/%.o, $(MAIN_CSOURCES))
MAIN_COBJECTS += $(patsubst $(SRC_DIR)/%.c, $(MAIN_BUILD_DIR)/%.o, $(CSOURCES))

SPI_COBJECTS = $(patsubst $(SRC_SPI)/%.c, $(SPI_BUILD_DIR)/%.o, $(SPI_CSOURCES))
SPI_COBJECTS += $(patsubst $(SRC_DIR)/%.c, $(SPI_BUILD_DIR)/%.o, $(CSOURCES))


### Build Components ###
# Main - Build the Object Files for Main Service
$(MAIN_BUILD_DIR)/%.o: $(SRC_MAIN)/%.c | $(MAIN_BUILD_DIR)
	$(CC) $(MAIN_CFLAGS) -c $< -o $@

$(MAIN_BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(MAIN_BUILD_DIR)
	$(CC) $(MAIN_CFLAGS) -c $< -o $@

# SPI - Build the Object Files for SPI Service
$(SPI_BUILD_DIR)/%.o: $(SRC_SPI)/%.c | $(SPI_BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SPI_BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(SPI_BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Creates Main Service Executable
.PHONY: main_service
main_service: $(MAIN_COBJECTS)
				$(CC) $(MAIN_COBJECTS) -o $(MAIN_BUILD_DIR)/main $(LDFLAGS)

# Creates SPI Service Executable
.PHONY: spi_service
spi_service: $(SPI_COBJECTS)
				$(CC) $(SPI_COBJECTS) -o $(SPI_BUILD_DIR)/spi $(LDFLAGS)