CC=gcc
CFLAGS=-I. -Wall -std=c99
OBJ = server.o 
OBJ2 = client.o
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
all:	server client
server: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
client: $(OBJ2)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o
