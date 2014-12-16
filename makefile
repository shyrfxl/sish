OBJS = sish.o type_prompt.o read_command.o parse.o builtin.o
CC = gcc
DEBUG = -g
LFLAGS = -Wall $(DEBUG)
CFLAGS = -c -Wall -Werror $(DEBUG) 
LIBS = -lreadline
# executables
all: sish

sish: $(OBJS) 
	$(CC) $(LFLAGS) $(OBJS) -o sish $(LIBS)

# object files
sish.o: sish.c sish.h 
	$(CC) $(CFLAGS) sish.c

type_prompt.o: type_prompt.c sish.h 
	$(CC) $(CFLAGS) type_prompt.c

read_command.o: read_command.c sish.h
	$(CC) $(CFLAGS) read_command.c

parse.o: parse.c sish.h
	$(CC) $(CFLAGS) parse.c

builtiin.o: builtin.c sish.h 
	$(CC) $(CFLAGS) builtin.c

# remove files
clean:
	rm *.o 


