# Compiler and flags
CC = gcc
CFLAGS = -Wall 

# Source files
SRCS = shell.c
OBJS = $(SRCS:.c=.o)

# Executable name
EXECUTABLE = mm

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXECUTABLE)

.PHONY: all clean