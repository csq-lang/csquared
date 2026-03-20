# =============================
# Makefile for C² Compiler
# Supports normal and test builds
# =============================

include config.mk

.PHONY: all clean rebuild info directories

# -----------------------------
# Build targets
# -----------------------------
all: directories $(TARGET)

rebuild: clean all

# Normal build
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)"
	$(CC) $(CFLAGS) $(OBJS) -o $(BINDIR)/$(TARGET) $(LDFLAGS)

# test build with tests
TEST_TARGET := csq-x86-test
TEST_OBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%_test.o,$(SRCS))

$(TEST_TARGET): directories $(TEST_OBJS)
	@echo "Linking test binary $(BINDIR)/$(TEST_TARGET)"
	$(CC) $(CFLAGS) $(TEST_OBJS) -o $(BINDIR)/$(TEST_TARGET) $(LDFLAGS)

# -----------------------------
# Compile source files
# -----------------------------
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%_test.o: $(SRCDIR)/%.c
	@echo "Compiling test $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCSQ_RUN_TESTS -c $< -o $@

# -----------------------------
# Token/Directory management
# -----------------------------
directories:
	@mkdir -p $(BUILDDIR) $(BINDIR) $(shell find $(SRCDIR) -type d | sed 's|$(SRCDIR)|$(BUILDDIR)|')

# -----------------------------
# Cleaning
# -----------------------------
clean:
	@echo "Cleaning build artifacts"
	@rm -rf $(BUILDDIR) $(BINDIR)

# -----------------------------
# test info
# -----------------------------
info:
	@echo "C² Build Configuration:"
	@echo "  Compiler:   $(CC)"
	@echo "  CFLAGS:     $(CFLAGS)"
	@echo "  LDFLAGS:    $(LDFLAGS)"
	@echo "  Source dir: $(SRCDIR)"
	@echo "  Build dir:  $(BUILDDIR)"
	@echo "  Binary dir: $(BINDIR)"
	@echo "  Target:     $(TARGET)"
	@echo "  test Target: $(TEST_TARGET)"
	@echo "  Sources:    $(SRCS)"
	@echo "  Objects:    $(OBJS)"
	@echo "  test Objects: $(TEST_OBJS)"
