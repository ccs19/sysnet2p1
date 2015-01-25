CC = gcc
CFLAGS = -g -Wall
OBJECTS = *.c
NAME = TCPServer
TODELETE = $(NAME) *.o
LIBS = -pthread

$(NAME) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME) $(LIBS)

.PHONY: clean
clean:
	rm -f $(TODELETE)