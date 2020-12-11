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

	char http_header[BUFSIZE] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
	char http_404[BUFSIZE] = "HTTP/1.1 404 Not Found" ;
	char http_message[BUFSIZE] ;

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

        char* method_name = NULL; // the string to receive the length from the message.
	char* request_file = NULL ;
	char* post_message = NULL ;


        if( listen(serv_sock, 5) == -1)
                error_handling("listen() error");

        while(1){ // start iterative server

                clnt_addr_size = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
                if(clnt_sock == -1)
                        error_handling("accept() error");

	
		str_len = read( clnt_sock, message, BUFSIZE) ;
       	        
		if(str_len == 0){
			close(clnt_sock) ;
			continue ;
		}
			
       	        message[str_len] = 0;

		//printf("Recieved request!!\n\n");

	        // extract the http message type
       	        char* tmp = message;
       		method_name = strtok( tmp, " ") ; // method name stored
	      
		//printf("HTTP Method is %s\n\n", method_name) ;			

		//if the request is GET
		if(!strcmp(method_name, "GET") ){
			request_file = strtok( NULL, " ");
			
			if(strlen(request_file) == 1)
				sprintf(request_file, "/index.html") ;
	
			sscanf(request_file, "/%s", request_file) ;
			//printf("Requested file is: %s\n\n", request_file) ;

			//if the requested file is invalid
			if(strcmp(request_file, "index.html") && strcmp(request_file, "query.html")){
				write(clnt_sock, http_404, strlen(http_404)) ;
				//printf("Requested wrong file! \n\n");
				close(clnt_sock) ;
				continue ;
			}
			
                	//open file
       	        	FILE* fp = fopen(request_file, "r");
               		str_len = fread(message, sizeof(char), BUFSIZE, fp);
			message[str_len] = 0 ;

			//send content to client
			sprintf(http_message, "%s%s\r\n\r\n", http_header, message) ;
			write(clnt_sock, http_message, strlen(http_message)) ;				

			fclose( fp );
			//printf("Sent reply\n\n") ;
				
			close(clnt_sock) ;
			//printf("closed with client\n") ;
			continue ;
	

		}
		else if(!strcmp(method_name, "POST") ){
			
			tmp = message ;
			//printf("first token: %s\n", tmp);
			
			int i = 19;
			while(i--){
				post_message = strtok( NULL, "\n");
			}
			
				
			sprintf(message, "<!DOCTYPE html><html><head><h2>%s</h2></head><body></body></html>", post_message) ;
			sprintf(http_message, "%s%s\r\n\r\n", http_header, message) ;



			write(clnt_sock, http_message, strlen(http_message)) ;				
			//printf("Post message is: %s\n\n", http_message) ;

			close(clnt_sock) ;
			//printf("closed with client\n") ;
			continue ;
								

		}
		else{
			close(clnt_sock) ;
			//printf("closed with client with something else\n" ) ;
			continue ;
		}

                

        }// finish iterative server


        return 0;
}

void error_handling(char* message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}

