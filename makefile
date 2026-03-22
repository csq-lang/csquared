# =============================
# Makefile for C² Compiler
# =============================

include config.mk

.PHONY: all clean rebuild info test directories

# -----------------------------
# Default target
# -----------------------------
all: $(BINDIR)/$(TARGET)

rebuild: clean all

# -----------------------------
# Linking
# -----------------------------
$(BINDIR)/$(TARGET): $(OBJS)
	@echo "Linking $@"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BINDIR)/$(TEST_TARGET): $(TEST_OBJS)
	@echo "Linking test binary $@"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# -----------------------------
# Compilation
# -----------------------------
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR)/%_test.o: $(SRCDIR)/%.c
	@echo "Compiling test $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCSQ_RUN_TESTS -MMD -MP -c $< -o $@

# -----------------------------
# Test target
# -----------------------------
test: $(BINDIR)/$(TEST_TARGET)
	@echo "Running tests..."
	@./$(BINDIR)/$(TEST_TARGET)

# -----------------------------
# Cleaning
# -----------------------------
clean:
	@echo "Cleaning build artifacts"
	@rm -rf $(BUILDDIR) $(BINDIR)

# -----------------------------
# Info
# -----------------------------
info:
	@echo "Build mode: $(BUILD)"
	@echo "Compiler:   $(CC)"
	@echo "CFLAGS:     $(CFLAGS)"
	@echo "Sources:    $(SRCS)"

# -----------------------------
# Dependency inclusion
# -----------------------------
-include $(DEPS)
-include $(TEST_DEPS)
