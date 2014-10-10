#include "server.h"
#include "errors.h"


/* Variables */
int serv_sockfd, newsockfd, portno, clilen, dest, n, i, j, m, k,  index_f, port, family,  client_no, fdmax;
char buffer[BUFLEN],buff[BUFLEN], command[20], aux[BUFLEN], adr[20], port_str[5], family_str[5], infos[BUFLEN], *token;
struct sockaddr_in serv_addr, cli_addr, aux_addr;
pclient client_list[MAX_CLIENTS];
fd_set read_fds, tmp_fds;
char name_used[] = "Eroare : Numele deja exista.\n";
char name_ok[] = "Succes : Numele este ok.\n";
char port_used[] = "Eroare: Portul este deja utilizat.\n";
char port_ok[] = "Succes : Portul este ok.\n";
char no_client[] = "Eroare : Clientul nu exista.\n";
char any_client[] = "Eroare : Nu a fost precizat nici un client.\n";
char no_msg[] = "Eroare : Nu ati scris nici un mesaj.\n";
char void_adr[] = "0.0.0.0";
bool kicked= false;

/* Error handling functions */
void arguments_error(int argc){
	DIE(argc < 2, "ERROR. Usage: port\n");
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


/*
 *	Functions
 */
pclient copy(pclient c){
	int i;
	pclient new_client = (pclient)malloc(sizeof(client));
	new_client->fd = c->fd;
	new_client->name = strdup(c->name);
	new_client->addr = c->addr;
	new_client->time = c->time;
	return new_client;
}

int remove_client(pclient client_list[], int i, int client_no){
	int j;
	for (j = i; j < client_no - 1; j++){
		free(client_list[j]);
		client_list[j] = copy(client_list[j + 1]);
	}
	free(client_list[client_no - 1]);
	client_no--;
	return client_no;
}
