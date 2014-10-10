#include "server.h"
#include "serv.cpp"

int main(int argc, char *argv[]){
	
	arguments_error(argc);
	
	pclient new_client = (pclient)malloc(sizeof(client));
	client_no = 0;
	int index;
	char *str;
	char *adr2;
	
	 
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
	
	serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	socketop_error(serv_sockfd);
	
	portno = atoi(argv[1]);
	
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
	serv_addr.sin_port = htons(portno);
	
	binding_error(bind(serv_sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)));
	
	listen(serv_sockfd, MAX_CLIENTS);
	
	FD_SET(serv_sockfd, &read_fds);
	fdmax = serv_sockfd;
	FD_SET(0,&read_fds);
	
	while (1){
		tmp_fds = read_fds;
		select_error(select(fdmax + 1, &tmp_fds, NULL, NULL, NULL));
		
		for(i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &tmp_fds)){
				if(i == 0){	//instructiuni primite de la tastatura
					fflush(stdin);
					memset(buff,0,BUFLEN);
     					fgets(buff,BUFLEN - 1,stdin);
					
					if (strcmp(buff,"status\n") == 0){	//status - afisam datele despre clienti
						for (j = 0; j < client_no; j++)
							printf("Numele: %s; adresa: %s; portul: %d\n", client_list[j]->name, inet_ntoa(client_list[j]->addr.sin_addr), ntohs(client_list[j]->addr.sin_port));
						
					}
					else if (strncmp(buff, "quit", 4) == 0){
						tmp_fds= read_fds;
						printf("Quitting...\n");
						for (j = 0; j < client_no; j++){
							socketwr_error(send(client_list[j]->fd,"quit",4,0));
							client_no = remove_client(client_list,j,client_no);
						}
							
						
						for (j = 0; j <= fdmax; j++)
							if (FD_ISSET(j,&read_fds)){
								FD_CLR(j,&read_fds);
								close(j);
							}
						return 0;
					}
					else if(strncmp(buff,"kick",4)==0){
							for (j = 0; j < client_no; j++){
								if(strstr(buff, client_list[j]->name)){
									printf("Mortal Kombat Championship with client %s and server.\n",client_list[j]->name);
									printf("Client %s is KO now. Good job, server!\n", client_list[j]->name);
									socketwr_error(send(client_list[j]->fd,"kick",4,0));
									client_no = remove_client(client_list,j,client_no);
									
									kicked= true;
									break;
								}
							}
							if(kicked== false)
								printf("Client not registered. Please try again.\n");
							else
								kicked= false;
								
					}
					else
						printf("Comanda invalida!\n");
					
				}
				else if (i == serv_sockfd){
					// a venit ceva pe socketul de ascultare = o noua conexiune
					clilen = sizeof(cli_addr);
					newsockfd = accept(serv_sockfd, (struct sockaddr *)&cli_addr, &clilen);
					socketacc_error(newsockfd);
					//adaug noul client in lista de clienti
					FD_SET(newsockfd, &read_fds);
					memset(buffer,0,BUFLEN);
					if ((n = recv(newsockfd,buffer,sizeof(buffer),0)) <= 0){
						recv_error(n);
						close(newsockfd); 
						FD_CLR(newsockfd, &read_fds);	//scoatem socketul din multime
					}
					else{
						//Verificam daca numele deja exista
						index = -1;
						for (j = 0; j < client_no; j++)
							if (!strcmp(buffer,client_list[j]->name)){
								socketwr_error(send(newsockfd,name_used,strlen(name_used),0));							
								//Inchidem conexiunea cu clientul
								close(newsockfd);
								FD_CLR(newsockfd, &read_fds);
								index = j;
								break;
							}
						
						if (index == -1){	//numele nu-i luat
							memset(buff,0,BUFLEN);
							strcpy(buff,buffer);
							socketwr_error(send(newsockfd,name_ok,strlen(name_ok),0));
							memset(buffer,0,BUFLEN);
							if ((n = recv(newsockfd,&portno,sizeof(int),0)) <= 0){
								close(newsockfd); 
								FD_CLR(newsockfd, &read_fds);	//scoatem socketul din multime
							}
							else{
								index = -1;
								for (j = 0; j < client_no; j++)
									if (portno == ntohs(client_list[j]->addr.sin_port)){
										socketwr_error(send(newsockfd,port_used,strlen(port_used),0));
										//Inchidem conexiunea cu clientul
										close(newsockfd);
										FD_CLR(newsockfd, &read_fds);
										index = j;
										break;
									}
						
								if (index == -1){	//putem sa adaugam noul client
									socketwr_error(send(newsockfd,port_ok,strlen(port_ok),0));
									
									if (newsockfd > fdmax)
										fdmax = newsockfd;
										
									client_list[client_no] = (pclient)malloc(sizeof(client));
										
									client_list[client_no]->name = strdup(buff);
									client_list[client_no]->addr = cli_addr;
									client_list[client_no]->addr.sin_port = htons(portno);
									client_list[client_no]->time = time(NULL);
									client_list[client_no]->fd = newsockfd;
										
									printf("A aparut un nou client : %s %d\n ", client_list[client_no]->name, ntohs(client_list[client_no]->addr.sin_port));
									client_no++;
								}
							}
						}
					}
					
				}
				else{	// am primit date pe unul din socketii cu care vorbesc cu clientii
					memset(buffer, 0, BUFLEN);
					if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0){
						for (j = 0; j < client_no; j++)
							if (i == client_list[j]->fd){
								//Scoatem clientul cu numarul index din lista
								client_no = remove_client(client_list,j,client_no);
								break;
							}
						
						if (n == 0) //conexiunea s-a inchis
							printf("Clientul cu socketul %d s-a deconectat\n", i);
						
						close(i);
						FD_CLR(i, &read_fds);
					}
					else{ 
						//aici interpretam comenzile preluate de la clienti
						token = strtok(buffer, " \n");
						strcpy(command,token);
						
						if (!strcmp(command,"listclients")){
							memset(buffer,0,BUFLEN);
							sprintf(buffer,"Nr. clienti : %d\n", client_no);
							for (j = 0; j < client_no; j++){
								strcat(buffer,client_list[j]->name);
								strcat(buffer,"\n");
							}
							socketwr_error(send(i,buffer,strlen(buffer),0));
						}
						else if (!strcmp(command,"infoclient")){
							token = strtok(NULL, " \n");
							index = -1;
							for (j = 0; j < client_no; j++)
								if (!strcmp(token,client_list[j]->name)){
									index = j;
									break;
								}
							
							memset(buffer,0,BUFLEN);
							if (index == -1)
								sprintf(buffer,"%s",no_client);
							else
								sprintf(buffer,"Nume: %s\tPort: %d\tOnline de: %.02lf secunde\n",client_list[index]->name, ntohs(client_list[index]->addr.sin_port), (float)(difftime(time(NULL),client_list[index]->time)));
							socketwr_error(send(i,buffer,strlen(buffer),0));
						}
						else if (!strcmp(command,"message")){
							memset(buff,0,BUFLEN);
							token = strtok(NULL, " \n");
							if (token){
								strcpy(buff,token);	//retinem numele
								token = strtok(NULL, "\0");
								if (token){
									strcpy(aux,token);	//retinem mesajul
									memset(buffer,0,BUFLEN);
									sprintf(buffer,"message");
									socketwr_error(send(i,buffer,sizeof(buffer),0));
									inet_aton(void_adr,&aux_addr.sin_addr);
									for (j = 0; j < client_no; j++)
										if (!strcmp(buff,client_list[j]->name)){
											aux_addr = client_list[j]->addr;
											break;
										}
								
									sprintf(adr,"%s",inet_ntoa(aux_addr.sin_addr));
									if (strcmp(adr,void_adr)){
										sprintf(infos,"%s ", adr);
										sprintf(port_str,"%d ",ntohs(aux_addr.sin_port));
										strcat(infos,port_str);
										sprintf(family_str,"%d ", aux_addr.sin_family);
										strcat(infos,family_str);
										
										if (!aux) 
											strcpy(aux,"none");
										strcat(infos,aux);
										strcat(infos,"\n");
										socketwr_error(send(i,infos,strlen(infos),0));
									}
									else
										socketwr_error(send(i,void_adr,strlen(void_adr),0));
								}
								else
									socketwr_error(send(i,no_msg,strlen(no_msg),0));
							}
							else{
								memset(buffer,0,BUFLEN);
								sprintf(buffer,"message");
								socketwr_error(send(i,buffer,sizeof(buffer),0));
								socketwr_error(send(i,void_adr,strlen(void_adr),0));
							}
						}
						
						else if (!strcmp(command,"quit")){
							for (j = 0; j < client_no; j++)
								if (i == client_list[j]->fd){
									client_no = remove_client(client_list,j,client_no);
									break;
								}
						}
						else if(!strncmp(buffer, "broadcast", 9)){
							memset(buff,0,BUFLEN);
							token = strtok(NULL, " \n");
							if(token){
								sprintf(buff, "Broadcast: %s\n", token);
								printf("Broadcasting.\n");
								for (j = 0; j < client_no; j++)
									socketwr_error(send(client_list[j]->fd, buff, strlen(buff), 0));
							}
						}
					}
				}
			}
		}
	}
	
	close(serv_sockfd);
	return 0; 
}
