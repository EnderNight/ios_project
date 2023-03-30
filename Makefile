
CC ?= gcc
SRCDIR ?= src
BUILDDIR ?= obj
INCDIR ?= include
TARGET ?= bin/run



SOURCES ?= $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS ?= $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.c=.o))
HEADERS ?= $(shell find $(INCDIR) -type f -name *.h)

CFLAGS += -I$(INCDIR) -Wall -Wextra -Wpedantic -std=c17 -g -fsanitize=address
LIBS = -lm

$(TARGET): $(OBJECTS) $(HEADERS)
	@mkdir -p bin
	$(CC) $^ -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $< -save-temps

clean:
	$(RM) -r $(BUILDDIR) $(TARGET) bin

.PHONY: clean
