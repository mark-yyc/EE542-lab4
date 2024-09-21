# EE542-lab4

Lab4 for EE542: Fast, Reliable File Transfer

Teammates:

- Tianyu: [Github](https://github.com/tianyu0923)
- Yang Jiao: [Github](https://github.com/Young884)

## Design document
See in [Design.md](./Design.md)

## Run:
Compile and run:
```
make
./fileCompare.o
./server.o [port]
./client.o [ip] [port]
```

Check md5:
```
./md5check.sh ./file.txt ./output.txt 
```