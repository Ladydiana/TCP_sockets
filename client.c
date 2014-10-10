#include "func.cpp"



int main(int argc, char *argv[]){
	
	clientarguments_error(argc);
	strcpy(fn, strdup(argv[1]));
	strcat(fn, ".txt");
	f= fopen(fn,"w+");
	portno= htons(atoi(argv[2]));
	
	serv_sockfd = socket(AF_INET,SOCK_STREAM,0);
	socketop_error(serv_sockfd);
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[4]));
	inet_aton(argv[3],&serv_addr.sin_addr);

	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
	
	//Verificam daca se poate conecta la server
	socketconn_error(connect(serv_sockfd,(struct sockaddr*) &serv_addr,sizeof(struct sockaddr)));

	
	// Trimitem numele clientului si asteptam raspuns	
	memset(buffer,0,BUFLEN);
	send_server(serv_sockfd,strdup(argv[1]),strlen(argv[1]),0);
	m = recv(serv_sockfd,buffer,sizeof(buffer),0);
	closed_connection(m);
	recv_error(m);
	printf("%s", buffer);
	name_taken(buffer, serv_sockfd);
	memset(buffer,0,BUFLEN);
	
	// Trimitem portul clientului si asteptam raspuns
	send(serv_sockfd,&portno,sizeof(int),0);
	m = recv(serv_sockfd,buffer,sizeof(buffer),0); 
	closed_connection(m);
	recv_error(m);
	printf("%s", buffer);
	port_taken(buffer, serv_sockfd);
	
	srand(time(NULL));
	my_sockfd = socket(AF_INET,SOCK_STREAM,0);
	socketop_error(my_sockfd);
	
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(portno);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	
	binding_error(bind(my_sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)));
	
	listen(my_sockfd,MAX_CLIENTS);
	
	FD_SET(serv_sockfd, &read_fds);
	FD_SET(my_sockfd, &read_fds);
	FD_SET(0,&read_fds);
	
	if (serv_sockfd > my_sockfd) 
		fdmax = serv_sockfd;
	else 
		fdmax = my_sockfd;
	
	while(1){
		tmp_fds = read_fds;
		select_error(select(fdmax + 1, &tmp_fds, NULL, NULL, NULL));
		
		for(i = 0; i <= fdmax; i++){
			if (FD_ISSET(i,&tmp_fds)){
				if (i == 0){	//citesc de la tastatura o comanda
					memset(buffer,0,BUFLEN);
					fgets(buffer,BUFLEN - 1,stdin);
					buffer[strlen(buffer) - 1] = '\0';
					
					if (!strcmp(buffer,"quit")){
						n = send(serv_sockfd,buffer,strlen(buffer),0);
						socketwr_error(n);
						
						for (j = 0; j <= fdmax; j++)
							if (FD_ISSET(j,&read_fds)){
								FD_CLR(j,&read_fds);
								close(j);
							}
						return 0;
					}
					if(!strcmp(buffer,"history")){
						printf("Printing history.\n");
					       	fseek(f,0,SEEK_END);
					       	string_size = ftell (f);
					       	rewind(f);
					       	bfr = (char*) malloc (sizeof(char) * (string_size + 1) );
					       	read_size = fread(bfr,sizeof(char*),string_size,f);
					       	bfr[string_size+1] = '\0';
						printf("%s\n", bfr);
					}					
	
					n = send(serv_sockfd,buffer,strlen(buffer),0);
					socketwr_error(n);
				}
				else if (i == serv_sockfd){	//primesc mesaj de la server
					memset(buffer,0,BUFLEN);
					m = recv(serv_sockfd,buffer,sizeof(buffer),0);
					if (m < 0){
						FD_CLR(serv_sockfd,&read_fds);
						close(serv_sockfd);
						socketr_error(m);
					}
					else{
						memset(buff,0,BUFLEN);
						strcpy(buff,buffer);	//salvam ce am primit de la server
						token = strtok(buffer, " ");
						
						if (!strcmp(token,"message")){	//am primit informatii necesare pt message
							memset(infos,0,sizeof(infos));
							m = recv(serv_sockfd,infos,sizeof(infos),0);
							if (m < 0){
								FD_CLR(serv_sockfd,&read_fds);
								close(serv_sockfd);
								socketr_error(m);
							}
							else{
								fprintf(f, "%s\n", infos);
								token = strtok(infos, " ");
								strcpy(adr,token);
								if (strcmp(adr,void_adr)){
									token = strtok(NULL, " ");
									strcpy(port_str,token);
									token = strtok(NULL, " ");
									strcpy(family_str,token);
									token = strtok(NULL, "\n");
									strcpy(message,token);
									port = atoi(port_str);
									family = atoi(family_str);
									if (strcmp(message,"none") && port != portno){
										inet_aton(adr,&other_addr.sin_addr);
										other_addr.sin_port = htons(port);
										other_addr.sin_family = family;
										
										other_sockfd = socket(other_addr.sin_family,SOCK_STREAM,0);
										c= connect(other_sockfd,(struct sockaddr*) &other_addr,sizeof(struct sockaddr));
										if ( c< 0)
											socketconn_error(c);
										else{
											FD_SET(other_sockfd,&read_fds);
											memset(piece,0,sizeof(piece));
											sprintf(piece,"%s",msg_msg);
											strcat(piece,message);
											n = send(other_sockfd,piece,strlen(piece),0);
											socketwr_error(n);
											memset(buffer,0,BUFLEN);
											m = recv(other_sockfd,buffer,sizeof(buffer),0);
											if (m < 0){
												FD_CLR(other_sockfd,&read_fds);
												close(other_sockfd);
												socketr_error(m);
											}
											
											else{
												//Inchidem conexiunea dupa ce am trimis mesajul
												printf("%s", buffer);
												FD_CLR(other_sockfd,&read_fds);
												close(other_sockfd);
											}
										}
									}
								}
								else{
									printf("%s", client_err);
								}
							}
						}//end informatii pt message

						else if(!strstr(buffer,"kick")==0){
							printf("Stop kicking me! :(\n");
							close(serv_sockfd);
							return 0;
						}
						else if(strcmp(buffer,"quit")==0){
							printf("Connection closed by server.\n");
							close(serv_sockfd);
							return 0;
						}
						
						else	//am primit raspuns la comenzile specifice pt server
							fprintf(f, "%s\n", buff);
							printf("%s \n", buff);
						
					}
				}
				else if (i == my_sockfd){	//un alt client doreste o conexiune directa
					len = sizeof(new_addr);
					if ((newsockfd = accept(my_sockfd, (struct sockaddr *)&new_addr, &len)) == -1)
						socketacc_error(newsockfd);
					else{	//adaugam noul socket in lista de file descriptori
						FD_SET(newsockfd,&read_fds);
						if (newsockfd > fdmax) fdmax = newsockfd;
					}
				}
				
				else{	//pt ceilalti socketi apartinand clientilor care s-au conectat
					memset(piece,0,sizeof(piece));
					m = recv(i,piece,sizeof(piece),0);
					if (m < 0){
						FD_CLR(i,&read_fds);
						close(i);
						socketr_error(m);
					}
					else if (!strncmp(piece,msg_msg,strlen(msg_msg))){
						printf("%s\n", piece + strlen(msg_msg));
						socketwr_error(n);
						socketwr_error(n);
						//inchidem conexiunea cu clientul
						FD_CLR(i,&read_fds);
						close(i);
					}
				}
			}
		}
	}
	fclose(f);
	return 0;
}
