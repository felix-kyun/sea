# Makefile
CC := clang
CFLAGS := -Wall -Wextra -Werror -pedantic -pthread -std=c11
TARGET_FLAGS := -rdynamic -ldl
MODULE_FLAGS := -shared -fPIC
INCLUDE_DIR := include 
SOURCE_DIR := src
MODULES_DIR := modules
BUILD_DIR := build
INCLUDES := -I$(INCLUDE_DIR)
SOURCES := $(shell find $(SOURCE_DIR) -type f -name '*.c')
MODULES := $(shell find  $(MODULES_DIR) -type f -name '*.c')
TARGET := sea_panel_main

_TARGET = $(addprefix $(BUILD_DIR)/, $(TARGET))
_OBJS = $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
_MODULES = $(patsubst $(MODULES_DIR)/%.c, $(BUILD_DIR)/modules/%.so, $(MODULES))

all: $(_TARGET) launch

$(_TARGET): $(_OBJS) $(_MODULES)
	$(CC) -o $@ $^ $(INCLUDES) $(CFLAGS) $(TARGET_FLAGS)

$(_MODULES): $(BUILD_DIR)/modules/%.so: $(MODULES_DIR)/%.c
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(CC) $(MODULE_FLAGS) -o $@ $< $(CFLAGS) $(INCLUDES)

$(_OBJS): $(BUILD_DIR)/%.o: src/%.c
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

launch:
	$(CC) -o $(BUILD_DIR)/sea_panel_launcher launch.c $(CFLAGS)

clean:
	rm -rf build/*

debug: CFLAGS += -g -fsanitize=address,undefined,leak -O0
debug: all

release: CFLAGS += -O2 -DNDEBUG
release: all

dev:
	bear -- make all

watch-log:
	tail -f panel.log 2>&1 | awk '/tail: .* file truncated/ { system("clear"); next } {print}'

test-string:
	$(CC) -o $(BUILD_DIR)/test_string_utf8 tests/string.test.c src/string-utf8.c  $(CFLAGS) $(INCLUDES)
	@$(BUILD_DIR)/test_string_utf8 && echo -e "\033[1;4;32mString Tests Passed!\033[0m" || echo -e "\033[1;4;31mString Tests Failed!\033[0m"
	@echo -e "\n\033[1;4;34mChecking For Memory Leaks...\033[0m"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 ./$(BUILD_DIR)/test_string_utf8 && echo -e "\033[1;4;32mNo Memory Leaks Detected!\033[0m" || echo -e "\033[1;4;31mMemory Leaks Detected!\033[0m"

test-args:
	$(CC) -o $(BUILD_DIR)/test_args tests/args.test.c src/args.c  $(CFLAGS) $(INCLUDES)
	@$(BUILD_DIR)/test_args && echo -e "\033[1;4;32mString Tests Passed!\033[0m" || echo -e "\033[1;4;31mString Tests Failed!\033[0m"
	@echo -e "\n\033[1;4;34mChecking For Memory Leaks...\033[0m"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 ./$(BUILD_DIR)/test_args && echo -e "\033[1;4;32mNo Memory Leaks Detected!\033[0m" || echo -e "\033[1;4;31mMemory Leaks Detected!\033[0m"

.PHONY: clean all test dev debug test-string release watch-log launch
