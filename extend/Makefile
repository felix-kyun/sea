# ============================================================================
# Makefile - v0.1.0 - Build configuration for project
# Author:   Praise Jacob <iampraisejacob@gmail.com>
# Repo:     https://github.com/felix-kyun/sea
# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Praise Jacob
# ============================================================================

CC := clang
CFLAGS := -Wall -Wextra -Werror -pedantic -std=c23 \
	-Wshadow -Wconversion -Wnull-dereference -Wformat=2 -Wundef
LDFLAGS :=
DEBUG_CFLAGS := -g -fsanitize=address,undefined,leak -O0
DEBUG_LDFLAGS := -fsanitize=address,undefined,leak
RELEASE_CFLAGS := -O3
RELEASE_LDFLAGS := -flto
MAKEFLAGS += -j$(shell nproc)

TARGET := extend
LIBS :=
PKG_CONFIG_PACKAGES := glib-2.0 gio-2.0 gstreamer-1.0
INCLUDE_DIR := include
SOURCE_DIR := src
BUILD_DIR := build
TEST_DIR := tests
.DEFAULT_GOAL := help

# ======= Don't edit below this line unless you know what you're doing =======

ifneq ($(PKG_CONFIG_PACKAGES),)
	PKG_CONFIG_CFLAGS := $(shell pkg-config --cflags $(PKG_CONFIG_PACKAGES))
	PKG_CONFIG_LIBS := $(shell pkg-config --libs $(PKG_CONFIG_PACKAGES))
	CFLAGS += $(subst -I,-isystem ,$(PKG_CONFIG_CFLAGS))
	LDFLAGS += $(PKG_CONFIG_LIBS)
endif

BUILD_TARGET := $(BUILD_DIR)/$(TARGET)
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
release: $(BUILD_TARGET)

.PHONY: debug
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: LDFLAGS += $(DEBUG_LDFLAGS)
debug: $(BUILD_TARGET)

$(BUILD_TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $(BUILD_TARGET) $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES) -MMD -MP

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: help
help:
		@echo "Targets:"
		@echo "all/release			Release build"
		@echo "debug				Debug build"
		@echo "clean				Clean build artifacts"
		@echo "format				Format whole project"
		@echo "test-<name> 			Run specific test"
		@echo "watch-test-<name>	Watch specific test"

clean:
	rm -r $(BUILD_DIR)

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
