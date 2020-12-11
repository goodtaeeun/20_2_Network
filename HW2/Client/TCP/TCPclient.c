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
	int read_size = 0 ;

	struct sockaddr_in serv_addr;

	if(argc != 4){
		printf("Usage : %s<IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM,0);
	if(sock == -1)
		error_handling("TCP socket generation error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));


	if( connect( sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");
	
	//prepare and send title info
	char length[10];
	sprintf(length, "%d", strlen(argv[3]) ) ;
	char* send_title = (char*)malloc(sizeof(char)*( strlen(length) + strlen(argv[3]) + 1) ) ;
	
	strcpy(send_title, length);
	strcat(send_title, " ");
	strcat(send_title, argv[3]);
	write(sock, send_title, strlen(length)+ strlen(argv[3]) + 1) ;
	
	
	printf("I sent the title as: %s\n", send_title);

	//open file & iteratively send data
	FILE* fp = fopen(argv[3], "rb") ;

	while(feof(fp) == 0){
		read_size = fread(message, sizeof(char), BUFSIZE, fp) ;
		write(sock, message, read_size);
	}
	sleep(1);
	free(send_title) ;
	fclose(fp);

	printf("All file sent successfully\n");

	close(sock);
	return 0;

}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
