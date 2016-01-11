make: server.o client.o
	gcc server.o -o server
	gcc client.o -o client
server.o: server.c server.h
	gcc server.c -c
client.o: client.c client.h
	gcc client.c -c
runc: client
	./client
runs: server
	./server
