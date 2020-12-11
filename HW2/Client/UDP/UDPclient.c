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
	int sock;
	char message[BUFSIZE];
	int str_len, addr_size, i;

	struct sockaddr_in serv_addr;
	struct sockaddr_in from_addr;

	if(argc != 4){
		printf("Usage : %s<IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_DGRAM,0);
	if(sock == -1)
		error_handling("UDP socket generation error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));


	struct timeval optVal = {5,0} ;
	int optLen = sizeof(optVal) ;

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &optVal, optLen) ; // set timer of 5 sec

	while(1){ //send title info and recieve a confirm

		sendto(sock, argv[3], strlen(argv[3]), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

		str_len = recvfrom(sock, message, BUFSIZE, 0, (struct sockaddr*)&from_addr, &addr_size ) ;

		if(str_len == -1){
			printf("no ack received from the server. Resent title\n") ;
			continue ;
		}
	
		break;
	}
	printf("Successfully sent the title as: %s\n", argv[3]);

	//open file & iteratively send data
	FILE* fp = fopen(argv[3], "rb") ;

	int read_size;
	while(feof(fp) == 0){
		read_size = fread(message, sizeof(char), BUFSIZE, fp) ;
		sendto(sock, message, read_size, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) ;
//		printf("sent to server length: %d\n", read_size) ;
	}

	
	printf("All work done\n");

	fclose(fp);
	close(sock);

	return 0;

}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
