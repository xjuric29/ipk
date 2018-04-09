#CFLAGS=-std=gnu99
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

all: ipk-dhcpstarve

ipk-dhcpstarve: ipk-dhcpstarve.c
	gcc ${CFLAGS} ipk-dhcpstarve.c -o ipk-dhcpstarve

clean:
	rm -f *.o *.out *.gch ipk-dhcpstarve