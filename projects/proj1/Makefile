CFLAGS=-std=gnu99
#CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

all: server client

server: ipk-server.c
	gcc ${CFLAGS} ipk-server.c -o ipk-server

client: ipk-client.c
	gcc ${CFLAGS} ipk-client.c -o ipk-client

clean:
	rm -f *.o *.out *.gch