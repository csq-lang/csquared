include config.mk

.PHONY: all clean directories

# Default target
all: directories $(TARGET)

directories:
	@mkdir -p $(BUILDDIR) $(BINDIR) $(shell find $(SRCDIR) -type d | sed 's/$(SRCDIR)/$(BUILDDIR)/')

# Target executable
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)"
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Object files from C source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts"
	@rm -rf $(BUILDDIR) $(BINDIR)

# Print Makefile information
info:
	@echo "C² Build Configuration:"
	@echo "  Compiler:   $(CC)"
	@echo "  CFLAGS:     $(CFLAGS)"
	@echo "  LDFLAGS:    $(LDFLAGS)"
	@echo "  Source dir: $(SRCDIR)"
	@echo "  Build dir:  $(BUILDDIR)"
	@echo "  Binary dir: $(BINDIR)"
	@echo "  Target:     $(TARGET)"
	@echo "  Sources:    $(SRCS)"
	@echo "  Objects:    $(OBJS)"
