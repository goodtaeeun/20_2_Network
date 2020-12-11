#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define BUFSIZE 5
void error_handling(char* message);

int main(int argc, char** argv){

//	printf("hello?");

	int serv_sock;
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
//	serv_addr.sin_port = htons("55558");
	serv_addr.sin_port = htons(atoi(argv[1]));

//	printf("dafe here?");


	if( bind( serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

//	printf("bind complete");	


//	sleep(5);

//	printf("finish sleeping");


	while(1){
		clnt_addr_size = sizeof(clnt_addr);
		sleep(1);
		str_len = recvfrom(serv_sock, message, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		printf("recvfrom returned %d\n", str_len);
		message[str_len] = 0;
//		printf("received number: %d \n", num++);
//		printf("received message from client : %s \n ", message);
//		sendto( serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
	}



	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
} 
