make: server.o client.o
	gcc server.o -o server
	gcc client.o calendar.o -o client
server.o: server.c server.h
	gcc server.c -c
client.o: client.c client.h
	gcc client.c -c
calendar.o: calendar.c calendar.h
	gcc calendar.c -c
runc: client
	./client
runs: server
	./server
clean:
	rm client
	rm server
	rm client.o
	rm server.o
