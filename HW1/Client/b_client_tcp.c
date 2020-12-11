#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>


#define BUFSIZE 30
void error_handling(char* message);

int main(int argc, char** argv){
	int sock;
	char message[BUFSIZE];
	int str_len, addr_size, i;

	char MSG1[] = "Good ";
	char MSG2[] = "Evening ";
	char MSG3[] = "Everybody!";

	struct sockaddr_in serv_addr;

	if(argc != 3){
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

//	printf("lets send messages");

	if( connect( sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	char MSG_M[] = "";
	write(sock, MSG_M, 0);

//	write(sock, MSG1, strlen(MSG1) );
//	write(sock, MSG2, strlen(MSG2) );
//	write(sock, MSG3, strlen(MSG3) );

//	printf("I sent messages");

	for(i=0; i<3; i++)
	{
		str_len = read(sock, message, BUFSIZE-1);
		message[str_len] = 0;
		printf("%dth message from the server : %s \n", i, message);
	}

//	sleep(4);

	close(sock);
	return 0;

}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
