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
TEST_CFLAGS := $(filter-out -O%,$(CFLAGS)) -O0

GEN_SRCS := $(wildcard generators/*.c)
GEN_BINS := $(patsubst generators/%.c,$(BUILD_DIR)/generators/%,$(GEN_SRCS))

.PHONY: all clean debug debug_test rebuild test generators
all: $(TARGET) $(GEN_BINS)
verbose: CFLAGS += -DDEBUG
verbose: $(TARGET) $(GEN_BINS)

generators: $(GEN_BINS)

debug: CFLAGS := $(filter-out -O%,$(CFLAGS))
debug: CFLAGS += -O0 -g3 -DDEBUG
debug: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

define run_tests
	@for test in $(TEST_BINS); do \
		echo "\nFile: $$test"; \
		$(1) $$test || exit $$?; \
	done
endef

test: override CFLAGS := $(TEST_CFLAGS)
test: $(TEST_BINS)
	$(call run_tests)

debug_test: override CFLAGS := $(TEST_CFLAGS) -g3 -DDEBUG
debug_test: $(TEST_BINS)
	$(call run_tests,valgrind --leak-check=full)

$(TEST_BUILD_DIR)/%: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/generators/%: generators/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TEST_BUILD_DIR)

