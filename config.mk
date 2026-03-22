# =============================
# Configuration for C² Compiler
# =============================

CC := gcc

# Build mode: debug | release
BUILD ?= debug

# Directories
SRCDIR := src
BUILDDIR := build
BINDIR := bin

TARGET := csq-x86
TEST_TARGET := csq-x86-test

# ---------------------------------
# Flags
# ---------------------------------

BASE_CFLAGS := -std=c2x -Wall -Wextra -pedantic -Iinclude -D_GNU_SOURCE
DEBUG_CFLAGS := -O0 -g
RELEASE_CFLAGS := -O3 -DNDEBUG

ifeq ($(BUILD),release)
	CFLAGS := $(BASE_CFLAGS) $(RELEASE_CFLAGS)
else
	CFLAGS := $(BASE_CFLAGS) $(DEBUG_CFLAGS)
endif

LDFLAGS :=

# ---------------------------------
# Recursive wildcard (portable)
# ---------------------------------

rwildcard = $(foreach d,$(wildcard $(1)/*),$(call rwildcard,$d,$2)) \
            $(filter $(subst *,%,$2),$(wildcard $(1)/$2))

SRCS := $(sort $(call rwildcard,$(SRCDIR),*.c))

# Object + dependency files
OBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

TEST_OBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%_test.o,$(SRCS))
TEST_DEPS := $(TEST_OBJS:.o=.d)
