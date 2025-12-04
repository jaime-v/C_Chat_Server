# Set CC and CFLAGS Variables
CC = gcc-14
CFLAGS = -Wall -Wextra -Werror -Wpedantic -Wconversion -Wsign-conversion -Wshadow -std=c2x -pthread -fsanitize=address,undefined -g
INCLUDES = -Iinclude

# Directories
SRC_DIR := src
BUILD_DIR := build

# Common Source Files
COMMON_SRC := $(wildcard $(SRC_DIR)/common/*.c)
COMMON_OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(COMMON_SRC))

# Server Source Files
SERVER_SRC := $(wildcard $(SRC_DIR)/server/*.c)
SERVER_OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SERVER_SRC))

# Common Source Files
CLIENT_SRC := $(wildcard $(SRC_DIR)/client/*.c)
CLIENT_OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CLIENT_SRC))

# Binaries
SERVER_BIN := $(BUILD_DIR)/server/server
CLIENT_BIN := $(BUILD_DIR)/client/client

# Default Target
all: $(SERVER_BIN) $(CLIENT_BIN)

# Link Targets
$(SERVER_BIN): $(COMMON_OBJ) $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(CLIENT_BIN): $(COMMON_OBJ) $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Create object files in build dir
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

# we can just run 
# make && ./build/server/server
#
# And if we ever want the binaries in the root directory, we just
# SERVER_BIN := server
# CLIENT_BIN := client
# clean:
#	rm -rf $(BUILD_DIR) $(SERVER_BIN) $(CLIENT_BIN)
