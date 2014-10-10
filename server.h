#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

#ifndef _SERVER_H
#define _SERVER_H 1

typedef struct{
	char *name;
	struct sockaddr_in addr;
	time_t time;
	int fd;
}client;

typedef client* pclient;


void arguments_error(int argc);
#endif
