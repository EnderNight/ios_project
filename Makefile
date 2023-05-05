# Makefile
# Used to compile the project


# Project structure variables
SRCDIR ?= src
BUILDDIR ?= obj
INCDIR ?= include
BINDIR ?= bin
MAIN_TARGET ?= main
HEADERS ?= $(shell find $(INCDIR) -type f -name *.h)


# Compiler variables
CC ?= gcc
CFLAGS += -I$(INCDIR) -Wall -Wextra -Wshadow -Wdouble-promotion -Wformat=2 -Wformat-overflow -Wformat-truncation -Wundef -Wconversion -Wpedantic -g3 -fsanitize=address -std=gnu11
LIBS = 



# Targets variables
WHERE_AM_I_TAR = where_am_i
INVENTORY_TAR = inventory
LS_TAR = ls
CD_TAR = cd
MAN_TAR = man
SHELL_TAR = shell
UTILS_TAR = utils
ITEM_TAR = item
INPUT_TAR = input
TOKEN_TAR = token
STACK_TAR = stack
PARSER_TAR = parser
AST_TAR = ast

EXECUTABLES ?= $(MAIN_TARGET) $(WHERE_AM_I_TAR) $(INVENTORY_TAR) $(LS_TAR) $(MAN_TAR)





all: $(EXECUTABLES)
########## MAIN EXECUTABLE ########
MAIN_TARGET_SRC = $(MAIN_TARGET).c $(SHELL_TAR).c $(UTILS_TAR).c $(CD_TAR).c $(INPUT_TAR).c $(TOKEN_TAR).c $(PARSER_TAR).c $(STACK_TAR).c $(AST_TAR).c
MAIN_TARGET_OBJ = $(patsubst %, $(BUILDDIR)/%, $(MAIN_TARGET_SRC:.c=.o))

$(MAIN_TARGET): $(MAIN_TARGET_OBJ)
	$(CC) $^ -o $(MAIN_TARGET) $(LIBS) $(CFLAGS)




###### COMMANDS EXECUTABLES ##########
WHERE_AM_I_SRC = $(WHERE_AM_I_TAR).c $(UTILS_TAR).c
WHERE_AM_I_OBJ = $(patsubst %, $(BUILDDIR)/%, $(WHERE_AM_I_SRC:.c=.o))

$(WHERE_AM_I_TAR): $(WHERE_AM_I_OBJ) $(MAIN_TARGET)
	@mkdir -p bin
	$(CC) $(WHERE_AM_I_OBJ) -o bin/$(WHERE_AM_I_TAR) $(LIBS) $(CFLAGS)



INVENTORY_SRC = $(INVENTORY_TAR).c $(ITEM_TAR).c $(UTILS_TAR).c
INVENTORY_OBJ = $(patsubst %, $(BUILDDIR)/%, $(INVENTORY_SRC:.c=.o))

$(INVENTORY_TAR): $(INVENTORY_OBJ) $(MAIN_TARGET)
	@mkdir -p bin
	$(CC) $(INVENTORY_OBJ) -o bin/$(INVENTORY_TAR) $(LIBS) $(CFLAGS)



LS_SRC = $(LS_TAR).c $(UTILS_TAR).c
LS_OBJ = $(patsubst %, $(BUILDDIR)/%, $(LS_SRC:.c=.o))
 
$(LS_TAR): $(LS_OBJ) $(MAIN_TARGET) 
	@mkdir -p bin
	$(CC) $(LS_OBJ) -o bin/$(LS_TAR) $(LIBS) $(CFLAGS) 



MAN_SRC = $(MAN_TAR).c $(UTILS_TAR).c
MAN_OBJ = $(patsubst %, $(BUILDDIR)/%, $(MAN_SRC:.c=.o))
 
$(MAN_TAR): $(MAN_OBJ) $(MAIN_TARGET) 
	@mkdir -p bin
	$(CC) $(MAN_OBJ) -o bin/$(MAN_TAR) $(LIBS) $(CFLAGS) 

######



####### OTHER ########

# Objects rule
$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean rule
clean:
	$(RM) -r $(BUILDDIR) $(MAIN_TARGET) bin

.PHONY: clean all
