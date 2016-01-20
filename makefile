make: server.o client.o calendar.o
	gcc server.o -o server
	gcc client.o -o client
	gcc calendar.o -o calendar
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
	rm calendar
	rm calendar.o
