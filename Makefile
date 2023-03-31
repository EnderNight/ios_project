
CC ?= gcc
SRCDIR ?= src
BUILDDIR ?= obj
INCDIR ?= include
TARGET ?= ios_project



SOURCES ?= $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS ?= $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.c=.o))
HEADERS ?= $(shell find $(INCDIR) -type f -name *.h)

CFLAGS += -I$(INCDIR) -Wall -Wextra -Wpedantic -g -std=c11 -fsanitize=address
LIBS = 

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $(TARGET) $(LIBS) $(CFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $< -save-temps

clean:
	$(RM) -r $(BUILDDIR) $(TARGET) bin

.PHONY: clean
