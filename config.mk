# Configuration for C² project
CC := gcc
CFLAGS := -std=c23 -Wall -Wextra -pedantic -Iinclude -D_GNU_SOURCE
LDFLAGS :=

SRCDIR = src
BUILDDIR = build
BINDIR = bin

TARGET = csq-x86
SRCS = $(shell find $(SRCDIR) -name '*.c')
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
