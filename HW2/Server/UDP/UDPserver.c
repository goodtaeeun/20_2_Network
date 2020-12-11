#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define BUFSIZE 1024
void error_handling(char* message);

int main(int argc, char** argv){



	int serv_sock ;
	char message[BUFSIZE];
	int str_len, num = 0;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;

	if(argc != 2){
		printf("Usage : %s <port> \n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock == -1)
		error_handling("UDP socket generation error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));


	if( bind( serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");



	struct timeval optVal = {5,0} ;
	int optLen = sizeof(optVal);


	while(1){ // start iterative server
	
		clnt_addr_size = sizeof(clnt_addr);
		
		char* title;

		optVal.tv_sec = 0;
		setsockopt(serv_sock, SOL_SOCKET, SO_RCVTIMEO, &optVal, optLen) ; // set the timer off
		
		//read first message containg the title
		str_len = recvfrom( serv_sock, message, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

		//save title
		title = (char*)malloc(sizeof(char)*str_len);
		strncpy(title, message, str_len);
		title[str_len] = 0 ;
		
		printf("title recieved from client: %s\n", title);

		optVal.tv_sec = 5;
		setsockopt(serv_sock, SOL_SOCKET, SO_RCVTIMEO, &optVal, optLen) ; // now set the timer to 5 sec
		
		while(1){ // loop to send ack before receiving data
			sendto(serv_sock, title, strlen(title), 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr)) ;
	
			//wait for first message
			str_len = recvfrom( serv_sock, message, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
			if(str_len == -1){
				printf("Timeout waiting on data. Resend ack\n") ;
				continue ;
			}

			//check if recieved data is same as the title
			char* newMSG = (char*)malloc(sizeof(char)*str_len);
			strncpy(newMSG, message, str_len);
			newMSG[str_len] = 0;
			if (strcmp(newMSG, title) == 0){
				printf("Title recieved again!! Resend ack\n");
				continue ;
			}

			break ;
		}

		 		
		//open file
		FILE* fp = fopen(title, "wb");
		fwrite(message, sizeof(char), str_len, fp);

		//set timer to 60 sec
		optVal.tv_sec = 60 ;
		setsockopt(serv_sock, SOL_SOCKET, SO_RCVTIMEO, &optVal, optLen) ; 

		//Now start receiving the remaining file
		while(1) {
			str_len = recvfrom( serv_sock, message, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size) ;
			if(str_len == -1 )
				break ;

			fwrite(message, sizeof(char), str_len, fp);

//			printf("received %d length string\n", str_len);

		}
		



		fclose(fp) ;
		printf("finish one client\n") ;

	}// finish iterative server



	close(socket);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
} 
