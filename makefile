###############################################################################
#                                                                             #
# MAKEFILE for wctl2                                                          #
#                                                                             #
# (c) Guy Wilson 2023                                                         #
#                                                                             #
###############################################################################

# Version number for WCTL
MAJOR_VERSION = 0
MINOR_VERSION = 1

# Directories
SOURCE = src
BUILD = build
DEP = dep

# What is our target
TARGET = ccalc

# Tools
C = gcc
LINKER = gcc

# postcompile step
PRECOMPILE = @ mkdir -p $(BUILD) $(DEP)
# postcompile step
POSTCOMPILE = @ mv -f $(DEP)/$*.Td $(DEP)/$*.d

CFLAGS = -c -O2 -Wall -pedantic
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP)/$*.Td
INCLUDEFLAGS = -I /usr/local/MacGPG2/include -I /opt/homebrew/include -I ${HOME}/Library/include
LIBFLAGS = -L /opt/homebrew/lib -L ${HOME}/Library/lib

# Libraries
STDLIBS = -lstrutils
EXTLIBS = -lreadline -lgmp -lmpfr

COMPILE.c = $(C) $(CFLAGS) $(DEPFLAGS) $(INCLUDEFLAGS) -o $@
LINK.o = $(LINKER) $(LIBFLAGS) $(STDLIBS) -o $@

CSRCFILES = $(wildcard $(SOURCE)/*.c)
OBJFILES = $(patsubst $(SOURCE)/%.c, $(BUILD)/%.o, $(CSRCFILES))
DEPFILES = $(patsubst $(SOURCE)/%.c, $(DEP)/%.d, $(CSRCFILES))

all: $(TARGET)

# Compile C/C++ source files
#
$(TARGET): $(OBJFILES)
	$(LINK.o) $^ $(EXTLIBS)

$(BUILD)/%.o: $(SOURCE)/%.c
$(BUILD)/%.o: $(SOURCE)/%.c $(DEP)/%.d
	$(PRECOMPILE)
	$(COMPILE.c) $<
	$(POSTCOMPILE)

.PRECIOUS = $(DEP)/%.d
$(DEP)/%.d: ;

-include $(DEPFILES)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin

clean:
	rm -r $(BUILD)
	rm -r $(DEP)
	rm $(TARGET)
