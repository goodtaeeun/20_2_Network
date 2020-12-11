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



	int serv_sock, clnt_sock ;
	char message[BUFSIZE];
	int str_len, num = 0;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;

	if(argc != 2){
		printf("Usage : %s <port> \n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("TCP socket generation error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	serv_addr.sin_port = htons(atoi(argv[1]));




	if( bind( serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

	int title_length = 0; // length of the actual title
	int length_length = 0; // length of the title length in the string form
	char* length_str = NULL; // the string to receive the length from the message.


	if( listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	while(1){ // start iterative server
		
		char* title;
	
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
			error_handling("accept() error");

		//read first message containg the title lentgh and the title
		str_len = read( clnt_sock, message, BUFSIZE);
		message[str_len] = 0;

	
		
		// extract the title size from the first message & prepare title to be stored
		char* tmp = message;
		length_length = strlen( (length_str = strtok( tmp, " ")) ) ;
		title_length = atoi(length_str) ; //one more for the last null
		title = (char*)malloc(sizeof(char)*title_length);

		strcpy(message, message+length_length+1); //left shift the message





		//iterate until title is all received
		int remain_title_length = title_length + length_length + 1 ;
		while(1){
			if(remain_title_length <= str_len){
				strcat( title, message) ;
				title[title_length] = '\0' ;
				strcpy(message, message+remain_title_length) ; //left shift the message
				str_len -= remain_title_length ; //remaining message size
				break;
			}
			else{
				strncat( title, message, strlen(message) );
				remain_title_length -= str_len ;
				str_len = read(clnt_sock, message, BUFSIZE);
				message[str_len] = 0;
			}
		}

		printf("title recieved from client: %s\n", title);
 		
		//open file
		FILE* fp = fopen(title, "wb");
		if( str_len > 0){ // check if initial message is left in the last title message
			fwrite(message, sizeof(char), str_len, fp);
		}



		//now start receiving the remaining file
		while( (str_len = read( clnt_sock, message, BUFSIZE)) != 0){

			fwrite(message, sizeof(char), str_len, fp);

		}

		printf("connection closed with one client\n") ;
		fclose(fp) ;

		close(socket);

	}// finish iterative server


	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
} 
