make: server.o client.o
	gcc server.o -o server
	gcc client.o -o client
	gcc calendar.o -o calendar
server.o: server.c server.h
	gcc server.c -c
client.o: client.c calendar.c client.h calendar.h
	gcc client.c calendar.c -c
runc: client
	./client
runs: server
	./server
clean:
	rm client
	rm server
	rm client.o
	rm server.o
