# Makefile
CC := clang
CFLAGS := -Wall -Wextra -Werror -pedantic 
INCLUDE_DIR := include 
SOURCE_DIR := src
BUILD_DIR := build
INCLUDES := -I$(INCLUDE_DIR)
SOURCES := $(shell find $(SOURCE_DIR) -type f -name '*.c')
TARGET := panel

_TARGET = $(addprefix $(BUILD_DIR)/, $(TARGET))
_OBJS = $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

all: $(_TARGET)

$(_TARGET): $(_OBJS)
	$(CC) -o $@ $^ $(INCLUDES) $(CFLAGS) 

$(_OBJS): $(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

clean:
	rm -rf build/*

setup: 
	[ -d $(BUILD_DIR) ] || mkdir -p $(BUILD_DIR)
	[ -d $(SOURCE_DIR) ] || mkdir -p $(SOURCE_DIR)
	[ -d $(INCLUDE_DIR) ] || mkdir -p $(INCLUDE_DIR)

dev:
	bear -- make all

.PHONY: clean all test dev 
