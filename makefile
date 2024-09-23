all: client server generateFile
client: client
	gcc -pthread client.c -o client.o
server: server.c
	gcc -pthread server.c -o server.o
generateFile: generateFile.c
	gcc generateFile.c -o generateFile.o
clean:
	rm client.o server.o generateFile.o