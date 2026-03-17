CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -pedantic -Iinclude -D_GNU_SOURCE -g -O0 -DDEBUG
LDFLAGS =

SRCDIR = src
BUILDDIR = build
BINDIR = bin

TARGET = csq-x86
SRCS = $(shell find $(SRCDIR) -name '*.c')
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
