#CFLAGS=-std=gnu99
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

all: server client

server: ipk-server.c string.o
	gcc ${CFLAGS} ipk-server.c string.o -o ipk-server

client: ipk-client.c string.o
	gcc ${CFLAGS} ipk-client.c string.o -o ipk-client

string.o: string.c
	 gcc ${CFLAGS} string.c -c

clean:
	rm -f *.o *.out *.gch