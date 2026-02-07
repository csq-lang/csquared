# Configuration for C² project
CC := gcc
CFLAGS := -std=gnu99 -Wall -Wextra -pedantic -Iinclude -D_GNU_SOURCE
LDFLAGS :=

# Target executable
TARGET := csq-x86

# Directories
SRCDIR := src
INCDIR := include
BUILDDIR := build
BINDIR := bin

# Build type: debug (default) or release
# Usage: make BUILD_TYPE=release
BUILD_TYPE ?= debug

ifeq ($(BUILD_TYPE),release)
    CFLAGS += -O2 -DNDEBUG
else
    CFLAGS += -g -O0 -DDEBUG
endif

# Source files (automatically find all .c files in src directory)
SRCS := $(shell find $(SRCDIR) -name "*.c")
OBJS := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

# Target binary path
TARGET := $(BINDIR)/$(TARGET)
