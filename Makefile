# Makefile stolen from: https://stackoverflow.com/a/1484873
TARGET = cgol
LIBS = -lc -lm
CC = gcc

# NOTE: -D_POSIX_C_SOURCE=200809L is needed for strnlen(),
# which is only defined in -std=gnu99 and not in -std=c99
# See the sntrnlen man page: man 3 strnlen
CFLAGS=-I. -Wall -Werror -std=c99 -D_POSIX_C_SOURCE=200809L -O2

.PHONY: default all debug clean

default: clean $(TARGET)
all: default
debug: CFLAGS += -DDEBUG
debug: clean $(TARGET)

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o *.gch *~
	-rm -f $(TARGET)
