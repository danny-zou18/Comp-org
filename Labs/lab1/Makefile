SRCS = main.c alloc.c print.c read.c mult.c

OBJS =  ${SRCS:.c=.o}

CC = gcc

INCLUDE_PATHS = -I./include

COMPILER_FLAGS = -Wall -Wextra

EXECUTABLE = main

all : $(EXECUTABLE) 

$(EXECUTABLE) : $(OBJS) 
	$(info test)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(INCLUDE_PATHS) -o $(EXECUTABLE)


%.o: %.c
	$(CC) $(INCLUDE_PATHS) $(COMPILER_FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXECUTABLE)

.PHONY : all clean
