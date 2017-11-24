.SUFFIXES:.c .o

CC   = gcc
SRCS = thread.c exploit.c rand.c http.c debug.c

OBJS = $(SRCS:.c=.o)
EXEC = length

start: $(OBJS)
		$(CC) -o $(EXEC) $(OBJS) -g -Wall -lpthread
		@echo '--- Complie ALL File Ok ---'

.c.o:
		$(CC) -o $@ -c $< -g -Wall -lpthread

clean:
		rm -f $(OBJS)