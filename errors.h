#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>



#ifndef _ERRORS_H
#define _ERRORS_H 1


/*
* Macro de verificare a erorilor
*/

#define DIE(assertion, call_description)				\
    if (assertion) {					\
        fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);		\
        perror(call_description);			\
        exit(EXIT_FAILURE);				\
    }

	
/* Dimensiunea calupului de date */
#define BUFLEN 256
#define KILOBYTE 1024	

/* Numarul maxim de clienti conectati */
#define MAX_CLIENTS 100


/* Functii */
void clientarguments_error(int argc);
void socketop_error(int sockfd);
void socketconn_error(int s);
void binding_error(int bd);
void select_error(int s);
void socketwr_error(int n);
void socketacc_error(int n);
void socketrecv_error(int n);
void recv_error(int n);
void listen_error(int x);
void serverarguments_error(int argc);
void close_all (fd_set *fds, int fdmax);
void send_server (int sock, char* msg, int l, int fdmax);
void closed_connection(int n);
void port_taken(char *buffer, int serv_sockfd);
void name_taken(char *buffer, int serv_sockfd);
void socketr_error(int n);


#endif
