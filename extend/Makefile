# Makefile
CC := clang
CFLAGS := -Wall -Wextra -Werror -pedantic -std=c23 \
	-Wshadow -Wconversion -Wnull-dereference -Wformat=2 -Wundef
LDFLAGS :=
DEBUG_CFLAGS := -g -fsanitize=address,undefined,leak -O0
DEBUG_LDFLAGS := -fsanitize=address,undefined,leak
RELEASE_CFLAGS := -O3
RELEASE_LDFLAGS := -flto

TARGET := extend
LIBS :=
INCLUDE_DIR := include
SOURCE_DIR := src
BUILD_DIR := build
TEST_DIR := tests

# do not modify
INCLUDES := -I$(INCLUDE_DIR)
SOURCES := $(shell find $(SOURCE_DIR) -type f -name '*.c')
OBJS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
DEPS := $(OBJS:.o=.d)
-include $(DEPS)

.PHONY: all
all: release

.PHONY: release
release: CFLAGS += $(RELEASE_CFLAGS)
release: LDFLAGS += $(RELEASE_LDFLAGS)
release: $(TARGET)

.PHONY: debug
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: LDFLAGS += $(DEBUG_LDFLAGS)
debug: $(TARGET)

# for static libs use: ar rcs $@ $^ $(LIBS)
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES) -MMD -MP

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

setup:
	@mkdir -p $(SOURCE_DIR) $(INCLUDE_DIR) $(TEST_DIR)

dev:
	bear -- $(MAKE) debug

format:
	@echo "Formatting code..."
	@fd -ec -eh . -x clang-format -i {}

watch-%:
	@echo "Watching for changes..."
	@fd -ec -eh . | entr -cd $(MAKE) -s $*

.SECONDEXPANSION:
test-%: CFLAGS += $(DEBUG_CFLAGS)
test-%: LDFLAGS += $(DEBUG_LDFLAGS)
test-%: $(TEST_DIR)/%.c $(SOURCE_DIR)/%.c $$(TEST_DEPS_$$*) | $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/test_$* $^ $(CFLAGS) $(INCLUDES) $(LDFLAGS)
	@./$(BUILD_DIR)/test_$*

.PHONY: clean setup dev format test-% watch-%
