#include "errors.h"


/* Variables */

int my_sockfd, serv_sockfd, other_sockfd, newsockfd, portno, n,m,fdmax,i,j,len, c, port, family, filesize, size;
int string_size, read_size;
fd_set read_fds;
fd_set tmp_fds;
struct sockaddr_in serv_addr, my_addr, other_addr, new_addr;
struct hostent *server;
char msg_msg[] = "Aveti un mesaj.\n";
char get_msg[] = "Aveti un request pt un fisier.\n";
char no_file[] = "ERROR 404: File not found.\n";
char client_err[] = "ERROR: Client not found.\n";
char void_adr[] = "0.0.0.0";
char buff[BUFLEN], buffer[BUFLEN], current_file[50], client[30], message[BUFLEN];
char adr[20], port_str[5], family_str[5], infos[BUFLEN], filename[50];
char kb[2000], piece[2000];
char *token;
char fn[50];
char *bfr;
long int bytesize;
struct stat st;
FILE *f;


/* Error handling functions */

void clientarguments_error(int argc){
	DIE(argc < 5, "ERROR. Usage: nume_client port_client ip_server port_server\n");
}

void socketop_error(int sockfd){
	DIE(sockfd < 0, "ERROR opening socket.\n");
}

void socketconn_error(int s){
	DIE(s < 0, "ERROR connecting.\n");
}

void binding_error(int bd){
	DIE(bd < 0, "ERROR on binding client.\n");
}

void select_error(int s){
	DIE(s==-1, "ERROR in select.\n");
}

void socketwr_error(int n){
	DIE(n<0, "ERROR writing to socket.\n");
}

void socketacc_error(int n){
	DIE(n==-1, "ERROR in accept.\n");
}

void socketrecv_error(int n){
	DIE(n<0, "ERROR in recv.\n");
}

void recv_error(int n){
	DIE(n<0, "ERROR in recv.\n");
}

void serverarguments_error(int argc){
	DIE(argc<2, "Sintaxa server gresita.\n")
}

void listen_error(int x){
	DIE(x<0, "ERROR on listen.\n")
}

void closed_connection(int n){
	DIE(n=0, "Serverul s-a inchis!\n")
}

void socketr_error(int n){
	DIE(n<0, "ERROR reading from socket!\n");
}

/* Other functions */

void close_all (fd_set *fds, int fdmax) {
    int i;
    for (i=0; i <= fdmax; i++) 
        if (FD_ISSET(i, fds)) 
            close(i);
}


void send_server (int sock, char* msg, int l, int fdmax) {
    int x = send(sock, msg, l, 0);
    if (x <= 0) {
	socketwr_error(x);
        printf("Nu am putut trimite mesaj la serverul central. Eroare comunicare.\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
}

void port_taken(char *buffer, int serv_sockfd){
	if (!strcmp(buffer,"Portul specificat este deja luat.\n")){
		close(serv_sockfd);
		exit(EXIT_FAILURE);
	}
}

void name_taken(char *buffer, int serv_sockfd){
	if (!strcmp(buffer,"Numele deja exista!\n")){
		close(serv_sockfd);
		exit(EXIT_FAILURE);
	}
}





