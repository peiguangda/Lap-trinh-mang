#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024
#define C_FOUND_ID "00"
#define C_NOT_FOUND_ID "01"
#define C_FOUND_PASSWORD "11"
#define C_NOT_FOUND_PASSWORD "01"
#define C_LOGOUT_OK "20"
#define C_LOGOUT_FAILS "21"
#define C_BLOCK "31"
#define C_NOT_SAME_USER "40"
#define C_SAME_USER "41"
#define C_CORRECT_PASS "50"
#define C_INCORRECT_PASS "51"
#define C_CORRECT_CODE "60"
#define C_INCORRECT_CODE "61"

#define NOT_IDENTIFIED_USER 1
#define NOT_AUTHENTICATED 2
#define AUTHENTICATED 3
#define START_SIGNUP 4
#define USERNAME_CREATED 5
#define PASSWORD_CREATED 6
#define CHECK_CODE_SUCCESSFUL 7
#define SIGNUP_SUCCESSFUL 8

#define BLOCKED 0
#define ACTIVE 1
#define MAX 100

int request(int client_sock, char message[])
{
	if (send(client_sock, message, strlen(message), 0) > 0)
	{
		return 1;
	} else return 0;
}

int receive(int client_sock, char respond[])
{
	int bytes_received = recv(client_sock, respond, BUFF_SIZE -1, 0);
	if (bytes_received > 0)
	{
		respond[bytes_received] = '\0';
		return 1;
	} else return 0;
}


int main(int argc, char const *argv[])
{
	int SERVER_PORT;
	char SERVER_ADDR[MAX];
	int client_sock;
	char buff[BUFF_SIZE + 1],respond[BUFF_SIZE];
	struct sockaddr_in server_addr; 
	int msg_len, bytes_sent, bytes_received, check;
	
	if (argc != 3) exit(1);
	SERVER_PORT = atoi(argv[2]);
	strcpy(SERVER_ADDR, argv[1]);

	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	
	while(1){
		strcpy(buff,"");
		printf("\nEnter your request:");
		memset(buff,'\0',(strlen(buff)+1));
		fgets(buff, BUFF_SIZE, stdin);		
		msg_len = strlen(buff);
		if (msg_len == 1) break;
		
		//send message
		if (!request(client_sock, buff)){
			printf("message send fails\n");
		} else{
			printf("\n-----------------------------------------------\n");
		}
		
		if (!receive(client_sock, respond)){
			printf("message receive fails\n");
		} else { 
			printf("\nRespond from server:%s.\n", respond);
			printf("\n-----------------------------------------------\n");
			while (strcmp(respond, "70") == 0 || strcmp(respond, "81") == 0 || strcmp(respond, "44") == 0)
			{
				if (!receive(client_sock, respond)){
					printf("message receive fails\n");
				} else { 
					printf("\nRespond from server:%s.\n", respond);
					printf("\n-----------------------------------------------\n");
				}
			}
		}
	}
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}