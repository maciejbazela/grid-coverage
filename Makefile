CC ?= gcc
CFLAGS := -Wall -Wextra -Werror -pedantic -std=c99 -Isrc

BUILD_DIR := build
TARGET := $(BUILD_DIR)/main

SRCS := $(shell find src -name '*.c')
OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

