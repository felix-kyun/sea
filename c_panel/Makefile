# Makefile
CC := clang
CFLAGS := -Wall -Wextra -Werror -pedantic -pthread
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
	[[ -d $(dir $@) ]] || mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

clean:
	rm -rf build/*

debug: CFLAGS += -g
debug: all

dev:
	bear -- make all

.PHONY: clean all test dev debug
