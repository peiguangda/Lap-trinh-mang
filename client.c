#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 8192
#define C_FOUND_ID "00"
#define C_NOT_FOUND_ID "01"
#define C_FOUND_PASSWORD "10"
#define C_NOT_FOUND_PASSWORD "11"
#define C_LOGOUT_OK "20"
#define C_LOGOUT_FAILS "21"
#define C_BLOCK "31"
#define C_NEW_USER "40"
#define C_SAME_USER "41"
#define C_CORRECT_PASS "50"
#define C_INCORRECT_PASS "51"
#define C_CORRECT_CODE "60"
#define C_INCORRECT_CODE "61"
#define C_IN_ROOM "70"
#define C_OUT_ROOM "71"
#define C_A_QQ_CORRECT "80"
#define C_A_QQ_INCORRECT "81"
#define C_READY "90"
#define C_NOT_READY "91"
#define C_A_CORRECT "04"
#define C_A_INCORRECT "05"
#define C_HELP_50_OK "14"
#define C_HELP_ADVISORY_OK "15"
#define C_HELP_NOT_OK "16"
#define C_STOP_OK "24"
#define C_STOP_NOT_OK "25"
#define C_CRE_ROOM_SUC "34"
#define C_CRE_ROOM_FAI "35"
#define C_YOU_WIN "45"
#define C_YOU_LOSE "44"
#define C_YOU_LOSE_1 "100"
#define C_YOU_LOSE_2 "200"
#define C_YOU_LOSE_3 "300"
#define C_YOU_STOP_1 "201"
#define C_YOU_STOP_2 "202"
#define C_YOU_STOP_3 "203"
#define C_YOU_STOP_4 "204"
#define C_YOU_STOP_5 "205"
#define C_YOU_STOP_6 "206"
#define C_YOU_STOP_7 "207"
#define C_YOU_STOP_8 "208"
#define C_YOU_STOP_9 "209"
#define C_YOU_STOP_10 "210"
#define C_YOU_STOP_11 "211"
#define C_YOU_STOP_12 "212"
#define C_YOU_STOP_13 "213"
#define C_YOU_STOP_14 "214"
#define C_YOU_STOP_15 "215"
#define C_YOU_IS_KEY "101"
#define C_WAIT "102"

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

char *makeFull(char respond[])
{
	if (strcmp(respond, C_IN_ROOM) == 0)
	{
		return "In room successfully, please wait until all the room signaled the start command!";
	}
	if (strcmp(respond, C_FOUND_ID) == 0)
	{
		return "Username ok, please enter the password!";
	} 
	if (strcmp(respond, C_NOT_FOUND_ID) == 0)
	{
		return "User incorrect, try again";
	}
	if (strcmp(respond, C_NOT_FOUND_PASSWORD) == 0)
	{
		return "Password incorrect , login fails, try again!";
	}
	if (strcmp(respond, C_LOGOUT_OK) == 0)
	{
		return "Logout successful!";
	}
	if (strcmp(respond, C_LOGOUT_FAILS) == 0)
	{
		return "Logout fails";
	}
	if (strcmp(respond, C_BLOCK) == 0)
	{
		return "User blocked, try again!";
	}
	if (strcmp(respond, C_NEW_USER) == 0)
	{
		return "Create new user, enter pass for new user";
	}
	if (strcmp(respond, C_SAME_USER) == 0)
	{
		return "User exits, please choose other name";
	}
	if (strcmp(respond, C_INCORRECT_PASS) == 0)
	{
		return "Pass is too short, please enter pass >= 5 character";
	}
	if (strcmp(respond, C_CORRECT_CODE) == 0)
	{
		return "Ok, user is created, Please sign in now!";
	}
	if (strcmp(respond, C_CRE_ROOM_SUC) == 0)
	{
		return "Ok, the room is created, \"STAR ID_ROOM\" to start game!";
	}
	if (strcmp(respond, C_A_QQ_INCORRECT) == 0)
	{
		return "Sorry, you answered wrong or slow to become the main player!";
	}
	if (strcmp(respond, C_YOU_WIN) == 0)
	{
		return "You are the winner, really good, answer 15 questions!";
	}
	if (strcmp(respond, C_CRE_ROOM_FAI) == 0)
	{
		return "The room id is exists! Create a room with another id!";
	}
	if (strcmp(respond, C_OUT_ROOM) == 0)
	{
		return "The room id is not exists or the room is playing!";
	}
	if (strcmp(respond, C_YOU_LOSE) == 0)
	{
		return "Regrettably, you have to stop the game here to give way to other players!";
	}
	if (strcmp(respond, C_YOU_LOSE_1) == 0)
	{
		return "Regrettably, You have to leave with a bonus of 0 VND!";
	}
	if (strcmp(respond, C_YOU_LOSE_2) == 0)
	{
		return "Regrettably, You have to leave with a bonus of 5.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_LOSE_3) == 0)
	{
		return "Regrettably, You have to leave with a bonus of 20.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_1) == 0)
	{
		return "You had stop this game. Your reward is 0 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_2) == 0)
	{
		return "You had stop this game. Your reward is 1.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_3) == 0)
	{
		return "You had stop this game. Your reward is 2.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_4) == 0)
	{
		return "You had stop this game. Your reward is 3.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_5) == 0)
	{
		return "You had stop this game. Your reward is 4.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_6) == 0)
	{
		return "You had stop this game. Your reward is 5.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_7) == 0)
	{
		return "You had stop this game. Your reward is 7.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_8) == 0)
	{
		return "You had stop this game. Your reward is 10.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_9) == 0)
	{
		return "You had stop this game. Your reward is 12.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_10) == 0)
	{
		return "You had stop this game. Your reward is 15.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_11) == 0)
	{
		return "You had stop this game. Your reward is 20.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_12) == 0)
	{
		return "You had stop this game. Your reward is 30.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_13) == 0)
	{
		return "You had stop this game. Your reward is 50.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_14) == 0)
	{
		return "You had stop this game. Your reward is 65.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_STOP_15) == 0)
	{
		return "You had stop this game. Your reward is 80.000.000 VND!";
	}
	if (strcmp(respond, C_YOU_IS_KEY) == 0)
	{
		return "The main player loses, you become the master of the room!";
	}
	if (strcmp(respond, C_WAIT) == 0)
	{
		return "Main players lose, waiting for new owners start command room!";
	}
	if (strcmp(respond, C_HELP_NOT_OK) == 0)
	{
		return "Can't use this feature";
	}
	else
		return respond;
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
			printf("\nRespond from server:\n%s\n", makeFull(respond));
			printf("\n-----------------------------------------------\n");
			while (strcmp(respond, C_IN_ROOM) == 0 || strcmp(respond, C_A_QQ_INCORRECT) == 0 || strcmp(respond, C_WAIT) == 0)
			{
				if (!receive(client_sock, respond)){
					printf("message receive fails\n");
				} else { 
					printf("\nRespond from server:\n%s\n", respond);
					printf("\n-----------------------------------------------\n");
				}
			}
		}
	}
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}