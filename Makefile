CC ?= gcc
CFLAGS := -Wall -Wextra -Werror -pedantic -std=c99 -Isrc -O2

BUILD_DIR := build
TEST_DIR := tests
TEST_BUILD_DIR := $(TEST_DIR)/build

TARGET := $(BUILD_DIR)/main

SRCS := $(shell find src -name '*.c')
OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_BINS := $(patsubst $(TEST_DIR)/%.c,$(TEST_BUILD_DIR)/%,$(TEST_SRCS))

.PHONY: all clean debug rebuild test
all: $(TARGET)
rebuild: clean $(TARGET)

debug: CFLAGS := $(filter-out -O%,$(CFLAGS))
debug: CFLAGS += -O0 -g3 -DDEBUG
debug: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test: CFLAGS += -O0 -g3
test: $(TEST_BINS)
	@for test in $(TEST_BINS); do \
		echo "\nFile: $$test"; \
		$$test || exit $$?; \
	done

$(TEST_BUILD_DIR)/%: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TEST_BUILD_DIR)

