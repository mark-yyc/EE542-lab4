all: client server generateFile
client: client
	gcc client.c -o client.o
server: server.c
	gcc server.c -o server.o
generateFile: generateFile
	gcc generateFile.c -o generateFile.o
clean:
	rm client.o server.o generateFile.o