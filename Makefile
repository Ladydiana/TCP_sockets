all:	clean
	gcc server.c -o server
	gcc client.c -o client
clean:
	rm -rf server
	rm -rf client
