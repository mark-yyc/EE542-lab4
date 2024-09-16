all: client server
client: client
	gcc client.c -o client.o
server: server.c
	gcc server.c -o server.o
clean:
	rm client.o server.o