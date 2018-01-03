#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define INT_DIGITS 19
#define BACKLOG 30
#define BUFF_SIZE 8192
#define USER "USER"
#define PASS "PASS"
#define LOUT "LOUT"
#define SIGU "SIGU"
#define SIGP "SIGP"
#define SIGC "SIGC"
#define HELP "HELP"
#define ANSW "ANSW"
#define REDY "REDY"
#define JOIN "JOIN"
#define STAR "STAR"
#define STOP "STOP"
#define CRRM "CRRM"
#define LEAV "LEAV" 

#define NIL "NIL" 
#define C_LOG_SEQ_WRONG "888" 
#define C_NOT_ROOM_MASTER "777" 

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
#define C_LEAV_ROOM_SUC "36"
#define C_LEAV_ROOM_FAI "37"
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
#define C_YOU_ARE_KEY "101"
#define C_WAIT "102"
#define C_ALL_ROOM_INCORRECT_K "999"
#define C_ALL_ROOM_INCORRECT_M "998"

#define NOT_IDENTIFIED_USER 1
#define NOT_AUTHENTICATED 2
#define AUTHENTICATED 3
#define START_SIGNUP 4
#define USERNAME_CREATED 5
#define PASSWORD_CREATED 6
#define SIGNUP_SUCCESSFUL 7
#define WAIT_QUICH_QUES 8
#define PLAYING_QUICK_QUES 9
#define PLAYING 10
#define WATCHING 11
#define READY 12
#define LOSE 13
#define HELP_ADVISORY 14

#define BLOCKED 0
#define ACTIVE 1
#define WAIT 0
#define PLAY 1
#define MAX_NUMBER_LOGIN 10
#define MAX_USER 10
#define MAX_ROOM 10
#define MAX_QUESTION 15
#define MAX_SESSION 100
#define FILE_NAME "account.txt"

#define QUES_FILENAME "question.txt"
#define QUES_IN_LEVER 30
#define QUES_NUMBER 120

struct User
{
	char id[30];
	char password[30];
	int userStatus;
	int count;
};

struct User users[MAX_USER];
struct User userSigns[MAX_USER];

typedef struct question
{
	int id;
	int level;
	char content[400];
	char choiceA[50];
	char choiceB[50];
	char choiceC[50];
	char choiceD[50];
	char answer[5];
	int quesStatus;
} Question;

struct Room
{
	int id;
	struct User users[MAX_USER];
	int connd[MAX_USER];
	int countUser;
	Question questions[MAX_QUESTION];
	int countQues;
	int roomStatus;
	int countHelp;
	int help5050count;
	int helpAdvisoryCount;
	char helpAnsw[MAX_USER][2];
};

struct Room rooms[MAX_ROOM];
int countRoom = 0;

struct Session
{
	struct User user;
	int sessStatus;
	int countLogin;
	int connd;
	char capcha[6];
	struct sockaddr_in cliAddr;
	struct Room room;
};

Question questionList[QUES_NUMBER];
Question quickQues;
int easyList[QUES_IN_LEVER];
int mediumList[QUES_IN_LEVER];
int hardList[QUES_IN_LEVER];
int easyIndex = 0, mediumIndex = 0, hardIndex = 0;

char answer5050[2];
char resultFromHelp5050[100];
char content[600];

char moneyReward[20]; 

struct Session sess[MAX_SESSION];

struct Session sessSignup[MAX_SESSION];

int sessCount = 0;
int userCount = 0;
int playerCount = 0;
int sessSignCount = 0;
int posCapchar;

//user constructor
struct User newUser(char id[], char password[], int userStatus)
{
	struct User user;
	strcpy(user.id, id);
	strcpy(user.password, password);
	user.userStatus = userStatus;
	user.count = 0;
	return user;
}

//session constructor
struct Session newSession(struct User user, int sessStatus, struct sockaddr_in cliAddr, int connd)
{
	struct Session session;
	session.user = user;
	session.sessStatus = sessStatus;
	session.cliAddr = cliAddr;
	session.countLogin = 0;
	session.connd = connd;
	return session;
}

//room constructor
struct Room newRoom(int id, int connd, struct User user)
{
	int i;
	struct Room room;
	for (i = 0; i < countRoom; ++i)
	{
		if (rooms[i].id == id)
		{
			return rooms[i];
		}
	}
	rooms[countRoom].id = id;
	rooms[countRoom].users[0] = user;
	rooms[countRoom].countUser = 1;
	rooms[countRoom].connd[0] = connd;
	rooms[countRoom].countQues = 0;
	rooms[countRoom].roomStatus = WAIT;
	rooms[countRoom].countHelp = 0;
	rooms[countRoom].help5050count = 0;
	rooms[countRoom].helpAdvisoryCount = 0;
	countRoom++;
	return rooms[countRoom-1];
}

//
int addLeverList(int id, int lv){
	switch(lv){
		case 1 :
			easyList[easyIndex] = id;
			easyIndex++;
			break;
		case 2 :
			mediumList[mediumIndex] = id;
			mediumIndex++;
			break;
		case 3 :
			hardList[hardIndex] = id;
			hardIndex++;
			break;
		default :
			break;
	}
	return 1;
}

//read question from file
void readQues(){
	FILE *fptr;
	int id, lv;
	char content[1000], a[1000], b[1000], c[1000], d[1000];
	char answ[5];
	int i = 0;
	fptr = fopen(QUES_FILENAME, "r");
	if (fptr == NULL) {
		printf("Can't open file !\n");
		return;
	}
	
	while(fscanf(fptr, "%d | %d | %[^|] | %[^|] | %[^|] | %[^|] | %[^|] | %s", &id, &lv, content, a, b, c, d, answ) != EOF){
		questionList[i].id = id;
		questionList[i].level = lv;
		bzero(questionList[i].content,400);
		strcpy(questionList[i].content, content);
		strcpy(questionList[i].choiceA, a);
		strcpy(questionList[i].choiceB, b);
		strcpy(questionList[i].choiceC, c);
		strcpy(questionList[i].choiceD, d);
		strcpy(questionList[i].answer, answ);
		addLeverList(id, lv);
		i++;
	}
	fclose(fptr);
}

//random index of question
int randomQuestion(int levelList[]){
	int result;
	srand(time(NULL));
	int random = rand() % QUES_IN_LEVER ;
	return random;
}

//check message from client is valid?
int isValidMessage(char message[], char messCode[], char messAcgument[])
{
	int i = 0, j = 0;
	for (i = 0; i < 4; i++ ){
		messCode[i] = message[i];
	}
	messCode[i] = '\0';
	if ( ((strcmp(messCode, STAR) == 0 || strcmp(messCode, STOP) == 0 || strcmp(messCode, LEAV) == 0)) && (message[i] == '\n' || message[i] == ' '))  return 1;
	if (message[i] != ' ') return 0;
	while (message[++i] != '\n'){
		messAcgument[j++] = message[i];
		if (message[i] == ' ') return 0;
	}
	messAcgument[j] = '\0';
	return 1;
}
 
int receive(int conn_sock, char message[])
{
	int bytes_received = recv(conn_sock, message, BUFF_SIZE -1, 0);
	if (bytes_received > 0)
	{
		message[bytes_received] = '\0';
		return 1;
	} else return 0;
}

int respond(int conn_sock, char respond[])
{
	if (send(conn_sock, respond, strlen(respond), 0) > 0)
	{
		return 1;
	} else return 0;
}

//read file and save to struct User
void readFileUser(char filename[])
{
	FILE *f = fopen(filename,"r");
	userCount = 0;
	int i = 0;
	char id[30], password[30], userStatus[2];
	struct User user;
	if (f == NULL)
	{
		printf("Can't open file %s!\n", filename);
		return; 
	}else {
		while (!feof(f))
		{
			fscanf(f, "%s %s %s\n", id, password, userStatus);
			user = newUser(id, password, atoi(userStatus));
			users[i] = user;
			users[i].count = i + 1;
			userCount++;
			i++;
		}
	}
	fclose(f);
}

//show user detail
void showUser()
{
	int i;
	printf("List user information: \n");
	for (i = 0; users[i].count != 0; i++ )
	{
		printf("----------------------------------------------\n");
		printf("Id : %s\n",users[i].id);
		printf("Password : %s\n",users[i].password);
		printf("Status : %d\n",users[i].userStatus);
	}
	printf("-----------------------------------------------------\n");
}

//write user to file
void writeUserToFile(char filename[])
{
	FILE *f = fopen(filename,"w+");
	int i = 0;
	struct User user;
	if (f == NULL)
	{
		printf("Can't open file %s!\n",filename);
		return; 
	}else {
		showUser();
		for (i = 0; users[i].count != 0; i++ )
		{
			fprintf(f, "%s %s %d\n", users[i].id, users[i].password, users[i].userStatus);
		}
	}
	fclose(f);
}


//find user by userID
int findUserById(char messAcgument[])
{
	int i = 0;
	for (i = 0; users[i].count != 0; i++ ){
		if (strcmp(users[i].id, messAcgument) == 0)
		{
			return i;
		} 
	}
	return -1;
}

//add a new session
int addUser(struct User user)
{
	if (userCount > MAX_USER) return 0;
	users[userCount++] = user;
	return 1;
}

//add a new session
int addSession(struct Session session)
{
	sess[sessCount++] = session;
}

//add a new session signup
int addSessionSignup(struct Session session)
{
	sessSignup[sessSignCount++] = session;
}

//remove session
int removeSession(int k)
{
	int i;
	for (i = k; i < sessCount -1; ++i)
	{
		sess[k] = sess[k+1];
	}
}

//find session by cliAddr, return session position
int findSessByAddr(struct sockaddr_in cliAddr, int connd)
{
	int i = 0;
	for (i = 0; i < sessCount; i++ ){
	 	if (memcmp(&(sess[i].cliAddr), &cliAddr, sizeof(struct sockaddr_in)) == 0 && sess[i].connd == connd)
		{
			return i;
		}
	}
	return -1;
}

//find room by id, return room position
int findRoomById(int id)
{
	int i;
	for (i = 0; i < countRoom; i++ ){
	 	if (rooms[i].id == id)
		{
			return i;
		}
	}
	return -1;
}

//remove session
void removeRoom(int k)
{
	int i;
	for (i = k; i < countRoom ; ++i)
	{
		rooms[k] = rooms[k+1];
	}
	countRoom--;
}

void checkListRoom()
{
	for (int i = 0; i < countRoom; ++i)
	{
		if (rooms[i].countUser <= 0)
		{
			printf("remove room:%d\n", i);
			removeRoom(i);
		}
	}
}

int checkPass(char pass[])
{
	int i;
	if (strlen(pass) < 5) return 0; //Password has min_length is 5
	else return 1;
}

//create capcha code, include 6 random character
char *makeCapcha(){
	char *capcha = (char*) malloc(6*sizeof(char));
	int i;
	srand(time(NULL));
	for(i = 0; i < 6; ++i){
    		capcha[i] = '0' + rand()%72;
	}
	return capcha;
}

//find sessionSignup by cliAddr, return session position
int findSessSignByAddr(struct sockaddr_in cliAddr, int connd)
{
	int i = 0;
	for (i = 0; i < sessSignCount; i++ ){
	 	if (memcmp(&(sessSignup[i].cliAddr), &cliAddr, sizeof(struct sockaddr_in)) == 0 && sessSignup[i].connd == connd)
		{
			return i;
		}
	}
	return -1;
}

void setStatusSessSign(int pos, int status)
{
	sessSignup[pos].sessStatus = status;
}

int sessSignStatusIs(int pos, int status)
{
	return sessSignup[pos].sessStatus == status;
}

void setStatusSess(int pos, int status)
{
	sess[pos].sessStatus = status;
}

void setSessRoom(int pos, struct Room room)
{
	sess[pos].room = room;
}

int roomStatusIs(int pos, int status)
{
	return rooms[pos].roomStatus == status;
}

void setStatusRoom(int pos, int status)
{
	rooms[pos].roomStatus = status;
}

int sessStatusIs(int pos, int status)
{
	return sess[pos].sessStatus == status;
}

//process while Code is USER
char *userCodeProcess(struct sockaddr_in cliAddr, int connd, int pos, int i)
{
	struct Session session;
	if (i == -1) return C_NOT_FOUND_ID; //if not found user
	if (users[i].userStatus == BLOCKED)	return C_BLOCK; //if found user but user blocked
	if (pos == -1) //if not found session
	{
		session = newSession(users[i], NOT_AUTHENTICATED, cliAddr, connd);// create new session
		addSession(session);
		return C_FOUND_ID;
	}
	//found session
	else if (sessStatusIs(pos, NOT_AUTHENTICATED) || sessStatusIs(pos, NOT_IDENTIFIED_USER)) {//found user != user of session
		setStatusSess(pos, NOT_AUTHENTICATED);
		memcpy(&(sess[pos].user), &users[i], sizeof(struct User));
		return C_FOUND_ID;
	}
	return C_LOG_SEQ_WRONG;
}

//Process while Code is PASS
char *passCodeProcess(char messAcgument[], int pos)
{
	int i;
	if (sess[pos].user.userStatus == BLOCKED)
		return C_BLOCK;
	//if PASS ok
	if (strcmp (sess[pos].user.password, messAcgument) == 0) 
	{
		setStatusSess(pos, AUTHENTICATED);
		sess[pos].countLogin = 0;			 // reset count login
		return C_FOUND_PASSWORD;
	}
	//PASS error 
	else 
	{                                                
		sess[pos].countLogin++;    			//countLogin + 1
		if (sess[pos].countLogin >= MAX_NUMBER_LOGIN){  //check count login is > max?
			sess[pos].user.userStatus = BLOCKED;	//block user 
			i = findUserById (sess[pos].user.id);
			users[i].userStatus = BLOCKED;
			writeUserToFile(FILE_NAME);			//save to file
			return C_BLOCK;
		} else
			return C_NOT_FOUND_PASSWORD;
	}
}

//Process while Code is CRRM, create a new room
char *crrmCodeProcess(char messAcgument[], int pos)
{
	struct Room room;
	int i = findRoomById(atoi(messAcgument));
	if (i >= 0) {
		return C_CRE_ROOM_FAI; //room exist
	} else {
		room = newRoom(atoi(messAcgument), sess[pos].connd, sess[pos].user);
		setStatusSess(pos, WAIT_QUICH_QUES);
		setSessRoom(pos, room);
		return C_CRE_ROOM_SUC;
	}
}

//add User to room, i - position of rooms
void addUserToRoom(int i, int connd,struct User user)
{
	int count = rooms[i].countUser;
	if (count <= MAX_USER)
	{
		rooms[i].users[count] = user;
		rooms[i].connd[count] = connd;
		rooms[i].countUser ++;
	}else {
		printf("room full slot\n");
	}
}

//Process while Code is ROOM
char *joinCodeProcess(char messAcgument[], int pos, int connd)
{
	int i = findRoomById(atoi(messAcgument));
	if (i < 0 ){ //room not exist
		return C_OUT_ROOM;
	} else if (roomStatusIs(i, PLAY)){ //room is playing now
		return C_OUT_ROOM;
	}else {
		addUserToRoom(i, connd, sess[pos].user);
		setStatusSess(pos, WAIT_QUICH_QUES);
		setSessRoom(pos, rooms[i]);
		return C_IN_ROOM;
	}
}

char *itoa(i)
     int i;
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}


//make full question (question + choose answer)
char *makeFullQues(Question question, int countQues)
{
	bzero(content,600);
	char str[1];
	if (countQues == 0){
		strcpy(content, "Quick question");
	} else {
		strcpy(content, "Question");
	}
	strcat(content, ": ");
	strcat(content,question.content);
	strcat(content, "\n");
	strcat(content, question.choiceA);
	strcat(content, "\n");
	strcat(content, question.choiceB);
	strcat(content, "\n");
	strcat(content, question.choiceC);
	strcat(content, "\n");
	strcat(content, question.choiceD);
	strcat(content, "\n>ENTER YOUR ANSWER :");
	return content;
}

//Process while Code is STAR
char *starCodeProcess(int pos, struct sockaddr_in cliAddr)
{
	int posRoom, posSess, random;
	//if is room master
	if (memcmp(&(sess[pos].user), &(sess[pos].room.users[0]), sizeof(struct User))  == 0)
	{
		//send question to all users of room
		int i;
		posRoom = findRoomById(sess[pos].room.id);
		setStatusRoom(posRoom, PLAY);
		random = randomQuestion(easyList); //random quick question
		quickQues = questionList[random];
		quickQues.quesStatus = 0; //There are currently no correct players
		char* res = makeFullQues(quickQues, rooms[posRoom].countQues);

		for (i = 0; i <= rooms[posRoom].countUser; ++i)
		{
			posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[i]);
			respond(rooms[posRoom].connd[i],res);
			setStatusSess(posSess, PLAYING_QUICK_QUES);
			setSessRoom(posSess, rooms[posRoom]);
		}
		return NIL;
	}else {
		return C_NOT_ROOM_MASTER;
	}
}

void view(Question q){
	printf("%s\n%s\n%s\n%s\n%s\n%s\n", q.content, q.choiceA, q.choiceB, q.choiceC, q.choiceD, q.answer);
}

//get question random
Question getQuestion(int pos)
{
	// Question question;
	int random;
	switch(sess[pos].room.countQues) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			random = randomQuestion(easyList) + QUES_IN_LEVER;
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			random = randomQuestion(mediumList) + QUES_IN_LEVER * 2;
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
			random = randomQuestion(hardList) + QUES_IN_LEVER * 3;
			break;
		case 15:
			break;
		default:
			break;		
	}
	view(questionList[random]);
	return questionList[random];
}

char *checkStatusSess(int pos, struct sockaddr_in cliAddr)
{
	int count = 0,posRoom = findRoomById(sess[pos].room.id);
	for (int i = 0; i < rooms[posRoom].countUser; ++i)
	{
		int posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[i]);
		if (sessStatusIs(posSess, WATCHING)) count++; 
	}
	if (count == rooms[posRoom].countUser )
	{
		for (int i = 0; i < rooms[posRoom].countUser; ++i)
		{
			int posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[i]);
			if (i == 0 )
			{
				respond(rooms[posRoom].connd[i], C_ALL_ROOM_INCORRECT_K);
			}else {
				respond(rooms[posRoom].connd[i], C_ALL_ROOM_INCORRECT_M);
			}
			setStatusSess(posSess, WAIT_QUICH_QUES);
		}
		return NIL;
	}
	else return C_A_QQ_INCORRECT;
}

//Process while Code is ANSW -quick question
char *answQuickCodeProcess(char messAcgument[], int pos, struct sockaddr_in cliAddr)
{
	Question question;
	int posRoom = findRoomById(sess[pos].room.id);
	if(strcmp(messAcgument, quickQues.answer) ==0 && quickQues.quesStatus == 0){
		//correct
		setStatusSess(pos, PLAYING);
		quickQues.quesStatus = 1;
		question = getQuestion(pos);
		rooms[posRoom].questions[0] = question;
		rooms[posRoom].countQues++;
		setSessRoom(pos, rooms[posRoom]);
		return makeFullQues(question, rooms[posRoom].countQues);
	}else{
		setStatusSess(pos, WATCHING);
		return checkStatusSess(pos, cliAddr);
		// return C_A_QQ_INCORRECT;
	}
}

char *getCodeBonus(int bonus_level)
{
	switch(bonus_level){
		case 0:
			return C_YOU_LOSE_1;
		case 1:
			return C_YOU_LOSE_2;
		case 2:
			return C_YOU_LOSE_3;
		default:
			return NIL;
	}
}

int findUserInRoom(int posRoom, int pos)
{
	for (int i = 0; i <= rooms[posRoom].countUser; ++i)
	{
		if (memcmp(&rooms[posRoom].users[i], &sess[pos].user, sizeof(struct User)) == 0)
		{
			return i;
		}
	}
}

void kickUser(int posRoom, int posUserInRoom)
{
	for (int i = posUserInRoom; i < rooms[posRoom].countUser; ++i)
	{
		rooms[posRoom].users[i] = rooms[posRoom].users[i+1];
		rooms[posRoom].connd[i] = rooms[posRoom].connd[i+1];
	}
	rooms[posRoom].countUser--;
}

//Process while Code is ANSW --main
char *answCodeProcess(char messAcgument[], int pos, struct sockaddr_in cliAddr)
{
	Question question;
	int posSess;
	int posRoom = findRoomById(sess[pos].room.id);
	int countQues = rooms[posRoom].countQues;
	printf("countQues: %d\n",countQues );
	if(strlen(messAcgument) == 1 && strcmp(messAcgument, rooms[posRoom].questions[countQues-1].answer) == 0){
		//return question to main && save question to list question of session && tang so cau tra loi dung len +1
		if (rooms[posRoom].countQues >= MAX_QUESTION)
		{
			//todo set status all member of room to wait_QQ
			int reachQues = rooms[posRoom].countQues;
		 	setStatusRoom(posRoom, WAIT);
			rooms[posRoom].countQues = 0;
			setStatusSess(pos, AUTHENTICATED);
			int posUserInRoom = findUserInRoom(posRoom, pos);
			kickUser(posRoom, posUserInRoom); //kick user khoi room vi thua cuoc
			for (int i = 0; i < rooms[posRoom].countUser; ++i)
			{
				int posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[i]); //find session of user in room
				setStatusSess(posSess, WAIT_QUICH_QUES);
				if (i == 0)
				{
					respond(rooms[posRoom].connd[i], C_YOU_ARE_KEY);// thong bao tro thanh chu phong
				}else {
					respond(rooms[posRoom].connd[i], C_WAIT);//thong bao doi hieu lenh bat dau
				}
				setSessRoom(posSess, rooms[posRoom]);
			}
			return C_YOU_WIN;
		}
		question = getQuestion(pos);
		rooms[posRoom].questions[countQues] = question;
		rooms[posRoom].countQues++;
		setSessRoom(pos, rooms[posRoom]);
		char* s = makeFullQues(question, rooms[posRoom].countQues);
		return s;
	} else {
		//incorrect
		int bonus_level = countQues/5;
		int posUserInRoom;
		int i;
		setStatusRoom(posRoom, WAIT);
		rooms[posRoom].countQues = 0;
		setStatusSess(pos, AUTHENTICATED);
		// tim user trong room
		posUserInRoom = findUserInRoom(posRoom, pos);
		kickUser(posRoom, posUserInRoom); //kick user khoi room vi thua cuoc
		for (i = 0; i < rooms[posRoom].countUser; ++i)
		{
			posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[i]); //find session of user in room
			// sess[posSess].sessStatus = WAIT_QUICH_QUES;
			setStatusSess(posSess, WAIT_QUICH_QUES);
			if (i == 0)
			{
				respond(rooms[posRoom].connd[i], C_YOU_ARE_KEY);// thong bao tro thanh chu phong
			}else {
				respond(rooms[posRoom].connd[i], C_WAIT);//thong bao doi hieu lenh bat dau
			}
			setSessRoom(posSess, rooms[posRoom]);
		}
		return getCodeBonus(bonus_level);//thong bao ban da ra ve voi so tien ...
	} 
	
}


//Process while Code is LOUT
char *loutCodeProcess(char messAcgument[], int pos)
{
	if (strcmp(sess[pos].user.id, messAcgument) == 0) //check userId is valid?
		{
			setStatusSess(pos, NOT_IDENTIFIED_USER);
			return C_LOGOUT_OK;
		} else 
			return C_LOGOUT_FAILS;
}

//process while code is SIGU
char *siguCodeProcess(char messAcgument[],struct sockaddr_in cliAddr, int connd, int pos, int i)
{
	struct Session session;
	struct User user;
	if (i != -1) return C_SAME_USER; //if not found user
	user = newUser(messAcgument, "", ACTIVE);
	if (pos == -1) //if not found session
	{
		session = newSession(user, USERNAME_CREATED, cliAddr, connd);// create new session
		addSessionSignup(session);                                   // add session
		return C_NEW_USER;
	}
	//found session
	else if (sessSignStatusIs(pos, START_SIGNUP) || sessSignStatusIs(pos, USERNAME_CREATED)) {//found user != user of session
		// sessSignup[pos].sessStatus = USERNAME_CREATED;
		setStatusSessSign(pos, USERNAME_CREATED);
		memcpy(&(sessSignup[pos].user), &user, sizeof(struct User));
		return C_NEW_USER;
	}
	return C_LOG_SEQ_WRONG;
}


//Process while Code is SIGP
char *sigpCodeProcess(char messAcgument[], int posSign)
{
	int i;
	if (checkPass(messAcgument)){
		strcpy(sessSignup[posSign].user.password, messAcgument);
		strcpy(sessSignup[posSign].capcha, makeCapcha());
		posCapchar = posSign;
		setStatusSessSign(posSign, PASSWORD_CREATED);
		return C_CORRECT_PASS;
	}
	else return C_INCORRECT_PASS;
}

//Process while Code is SIGC
char *sigcCodeProcess(char messAcgument[], int pos)
{
	if (strcmp(sessSignup[pos].capcha, messAcgument) == 0) //check capcha
	{
		setStatusSessSign(pos, SIGNUP_SUCCESSFUL);
		sessSignup[pos].user.count = userCount;
		if (addUser(sessSignup[pos].user))
		{
			writeUserToFile(FILE_NAME);
			return C_CORRECT_CODE;
		}
	} else 
		return C_INCORRECT_CODE;
}

//Make right answer and random answer arranged
void arrangeAnswer(){
  	char temp;
 	if (answer5050[0] > answer5050[1])
  	{
   		temp = answer5050[1];
   		answer5050[1] = answer5050[0];
   		answer5050[0] = temp;
  	}
}

//Random question from 3 choices
char getRandom(char choice1, char choice2, char choice3)
{
	srand(time(NULL));
	int random = rand() % 3 + 1;
	printf("random : %d\n", random);
	switch(random)
	{
		case 1: return choice1;
				break;
		case 2: return choice2;
				break;
		case 3: return choice3;
				break;
	}
}

//Help feature is 50-50
void help5050Process(Question question)
{
	int i = 0;
	switch(question.answer[0])
	{
		case 'A':
			answer5050[0] = 'A';
			answer5050[1] = getRandom('B','C','D');
			arrangeAnswer();
			break;
		case 'B':
			answer5050[0] = 'B';
			answer5050[1] = getRandom('A','C','D');
			arrangeAnswer();
			break;
		case 'C':
			answer5050[0] = 'C';
			answer5050[1] = getRandom('A','B','D');
			arrangeAnswer();
			break;
		case 'D':
			answer5050[0] = 'D';
			answer5050[1] = getRandom('A','B','C');
			arrangeAnswer();
			break;
		default:
			break;
	}
	while(i != 2)
	{
		if (i == 1) strcat(resultFromHelp5050, "\n");
		switch(answer5050[i])
		{
			case 'A':
				strcat(resultFromHelp5050,question.choiceA);
				break;
			case 'B':
				strcat(resultFromHelp5050,question.choiceB);
				break;
			case 'C':
				strcat(resultFromHelp5050,question.choiceC);
				break;
			case 'D':
				strcat(resultFromHelp5050,question.choiceD);
				break;
			default :
				break;
		}
	i++;
	}
}

char *helpAdvisoryProcess(Question question, int pos, struct sockaddr_in cliAddr)
{
	int i, posSess;
	int posRoom = findRoomById(sess[pos].room.id);
	int countQues = rooms[posRoom].countQues;
	char* res = makeFullQues(question, countQues);

	for (i = 0; i <= rooms[posRoom].countUser; ++i)
	{
		posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[i]); //find session of user in room
		if (sessStatusIs(posSess, WATCHING))
		{
			respond(rooms[posRoom].connd[i],res);
			setStatusSess(posSess, HELP_ADVISORY);
		}
	}
	return NIL;
}



//Process while Code is HELP
char *helpCodeProcess(char messAcgument[], int pos, struct sockaddr_in cliAddr)
{
	int count = sess[pos].room.countQues;
	int posRoom = findRoomById(sess[pos].room.id);
	Question question = sess[pos].room.questions[count -1];	
	printf("help5050count1 :::::::::::::::::::: %d\n",rooms[posRoom].help5050count );
	printf("help5050count1 id :::::::::::::::::::: %d\n",rooms[posRoom].id);
	if (strcmp(messAcgument, "1") == 0)
	{
		if (rooms[posRoom].help5050count != 0) return C_HELP_NOT_OK;
		else {
			help5050Process(question);
			rooms[posRoom].help5050count++;
			printf("help5050count2 :::::::::::::::::::: %d\n",rooms[posRoom].help5050count);
			printf("help5050count2 id :::::::::::::::::::: %d\n",rooms[posRoom].id);
			return C_HELP_50_OK;
		}
	}
	else if (strcmp(messAcgument, "2") == 0)
	{
		if (rooms[posRoom].helpAdvisoryCount != 0) return C_HELP_NOT_OK;
		if (sess[pos].room.countUser <= 1) return C_HELP_NOT_OK;
		else {
			helpAdvisoryProcess(question, pos, cliAddr);
			rooms[posRoom].helpAdvisoryCount ++;
			return C_HELP_ADVISORY_OK;
		}
	}
	else return C_HELP_NOT_OK;
}

char *getHelpAdvise(int posRoom)
{
	int countHelp = rooms[posRoom].countHelp;
	int a=0,b=0,c=0,d=0;
	int perA, perB, perC, perD;
	char str[3];
	bzero(content,600);
	for (int i = 0; i < countHelp; ++i)
	{
		if(strcmp(rooms[posRoom].helpAnsw[i], "A" ) == 0){
			a++;
		}
		if(strcmp(rooms[posRoom].helpAnsw[i], "B" ) == 0){
			b++;
		}
		if(strcmp(rooms[posRoom].helpAnsw[i], "C" ) == 0){
			c++;
		}
		if(strcmp(rooms[posRoom].helpAnsw[i], "D" ) == 0){
			d++;
		}
	}

	perA = 100*a/(a+b+c+d);
	perB = 100*b/(a+b+c+d);
	perC = 100*c/(a+b+c+d);
	perD = 100*d/(a+b+c+d);
	strcpy(content, "SUGGESTIONS FROM OTHER PLAYERS FOR YOU:");
	strcat(content, "\nA:");
	strcpy(str, itoa(perA));
	strcat(content, str);
	strcat(content, "%\nB:");
	strcpy(str, itoa(perB));
	strcat(content, str);
	strcat(content, "%\nC:");
	strcpy(str, itoa(perC));
	strcat(content, str);
	strcat(content, "%\nD:");
	strcpy(str, itoa(perD));
	strcat(content, str);
	strcat(content, "%\n");
	strcat(content, ">ENTER YOUR ANSWER :");
	return content;
}

char *answHelpCodeProcess(char messAcgument[],int pos, struct sockaddr_in cliAddr)
{
	int i, posSess, posRoom = findRoomById(sess[pos].room.id);
	int countHelp = rooms[posRoom].countHelp;
	setStatusSess(pos, WATCHING);
	strcpy(rooms[posRoom].helpAnsw[countHelp], messAcgument) ;
	rooms[posRoom].countHelp++;
	if (rooms[posRoom].countHelp == rooms[posRoom].countUser-1)
	{
		for (i = 0; i < rooms[posRoom].countUser; ++i)
		{
			posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[i]); //find session of user in room
			if (sessStatusIs(posSess, PLAYING))
			{
				char *res = getHelpAdvise(posRoom);
				respond(rooms[posRoom].connd[i], res);// thong bao tro thanh chu phong
			}
			setSessRoom(posSess, rooms[posRoom]);
		}
	}
	return NIL;
}

//Get money reward from number of question answered
char *getRewardFromQuestion(int reachQues)
{
	switch(reachQues)
	{
		case 1 :
			return C_YOU_STOP_1;
			break;
		case 2 :
			return C_YOU_STOP_2;
			break;
		case 3 :
			return C_YOU_STOP_3;
			break;
		case 4 :
			return C_YOU_STOP_4;
			break;
		case 5 :
			return C_YOU_STOP_5;
			break;
		case 6 :
			return C_YOU_STOP_6;
			break;
		case 7 :
			return C_YOU_STOP_7;
			break;
		case 8 :
			return C_YOU_STOP_8;
			break;
		case 9 :
			return C_YOU_STOP_9;
			break;
		case 10 :
			return C_YOU_STOP_10;
			break;
		case 11 :
			return C_YOU_STOP_11;
			break;
		case 12 :
			return C_YOU_STOP_12;
			break;
		case 13 :
			return C_YOU_STOP_13;
			break;
		case 14 :
			return C_YOU_STOP_14;
			break;
		case 15 :
			return C_YOU_STOP_15;
			break;
		default :
			break;
	}
}

//Process while Code is STOP
char *stopCodeProcess(int pos, struct sockaddr_in cliAddr)
{
 	int posRoom = findRoomById(sess[pos].room.id);
 	int reachQues = rooms[posRoom].countQues; //the number of question when this user say stop 
 	setStatusRoom(posRoom, WAIT);
	rooms[posRoom].countQues = 0;
	setStatusSess(pos, AUTHENTICATED);
	int posUserInRoom = findUserInRoom(posRoom, pos);
	kickUser(posRoom, posUserInRoom); //kick user khoi room vi thua cuoc
	for (int i = 0; i < rooms[posRoom].countUser; ++i)
	{
		int posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[i]); //find session of user in room
		setStatusSess(posSess, WAIT_QUICH_QUES);
		if (i == 0)
		{
			respond(rooms[posRoom].connd[i], C_YOU_ARE_KEY);// thong bao tro thanh chu phong
		}else {
			respond(rooms[posRoom].connd[i], C_WAIT);//thong bao doi hieu lenh bat dau
		}
		setSessRoom(posSess, rooms[posRoom]);
	}
	return getRewardFromQuestion(reachQues);
}

//Process while Code is LEAV
char *leavCodeProcess(int pos, struct sockaddr_in cliAddr)
{
	int posRoom = findRoomById(sess[pos].room.id);
	int posUserInRoom = findUserInRoom(posRoom, pos);
	kickUser(posRoom, posUserInRoom); //kick user 
	setStatusSess(pos, AUTHENTICATED);
	return C_LEAV_ROOM_SUC;
}

//process request
char *process(char messCode[], char messAcgument[], struct sockaddr_in cliAddr, int connd )
{
	int pos,  posSign, i;
	pos = findSessByAddr(cliAddr, connd);
	posSign = findSessSignByAddr(cliAddr, connd);
	checkListRoom();

	/***********messcode is USER***********/
	if (strcmp(messCode, USER) == 0 ){
		i = findUserById (messAcgument);
		return userCodeProcess(cliAddr,connd, pos, i);
	}

	/********messcode is PASS**********/
	if (strcmp(messCode, PASS) == 0 && pos != -1 && sessStatusIs(pos, NOT_AUTHENTICATED))
	{
		return passCodeProcess(messAcgument, pos);
	}

	/********messcode is CRRM**********/
	if (strcmp(messCode, CRRM) == 0 && pos != -1 && sessStatusIs(pos, AUTHENTICATED) )
	{
		return crrmCodeProcess(messAcgument, pos);
	}

	/********messcode is JOIN**********/
	if (strcmp(messCode, JOIN) == 0 && pos != -1 && sessStatusIs(pos, AUTHENTICATED) )
	{
		return joinCodeProcess(messAcgument, pos, connd);
	}

	/********messcode is STAR**********/
	if (strcmp(messCode, STAR) == 0 && pos != -1 && sessStatusIs(pos, WAIT_QUICH_QUES) )
	{
		return starCodeProcess(pos, cliAddr);
	}

	/********messcode is ANSW**********/
	if (strcmp(messCode, ANSW) == 0 && pos != -1 && sessStatusIs(pos, PLAYING_QUICK_QUES) )
	{
		return answQuickCodeProcess(messAcgument, pos, cliAddr);
	}

	if (strcmp(messCode, ANSW) == 0 && pos != -1 && sessStatusIs(pos, HELP_ADVISORY) )
	{
		return answHelpCodeProcess(messAcgument, pos, cliAddr);
	}

	/********messcode is LEAV**********/
	if (strcmp(messCode, LEAV) == 0 && pos != -1 && (sessStatusIs(pos, WAIT_QUICH_QUES)||sessStatusIs(pos, PLAYING_QUICK_QUES)))
	{
		return leavCodeProcess(pos, cliAddr);
	}

	/********messcode is ANSW**********/
	if (strcmp(messCode, ANSW) == 0 && pos != -1 && sessStatusIs(pos, PLAYING) )
	{
		return answCodeProcess(messAcgument, pos, cliAddr);
	}

	/********messcode is LOUT*********/
	if (strcmp(messCode, LOUT) == 0 && pos != -1 && sess[pos].sessStatus == AUTHENTICATED)
	{
		return loutCodeProcess(messAcgument, pos);
	}

	/********messcode is ANSW**********/
	if (strcmp(messCode, HELP) == 0 && pos != -1 && sessStatusIs(pos, PLAYING) )
	{
		return helpCodeProcess(messAcgument, pos, cliAddr);
	}

	/********messcode is STOP**********/
	if (strcmp(messCode, STOP) == 0 && pos != -1 && sessStatusIs(pos, PLAYING) )
	{
		return stopCodeProcess(pos, cliAddr);
	}
	
	/********messcode is SIGU*********/
	if (strcmp(messCode, SIGU) == 0)
	{
		i = findUserById (messAcgument); 
		return siguCodeProcess(messAcgument, cliAddr,connd, posSign, i);
	}

	/********messcode is SIGP*********/
	if (strcmp(messCode, SIGP) == 0 && posSign != -1 && sessSignStatusIs(posSign, USERNAME_CREATED))
	{
		return sigpCodeProcess(messAcgument, posSign);
	}

	/********messcode is SIGC*********/
	if (strcmp(messCode, SIGC) == 0 && posSign != -1 && sessSignStatusIs(posSign, PASSWORD_CREATED))
	{
		return sigcCodeProcess(messAcgument, posSign);
	}

	else
	{
		return C_LOG_SEQ_WRONG;
	}
}

void addListRoomTo(char message[])
{
	if (countRoom > 0){
		strcat(message, "LIST ROOM \n");
		strcat(message," ------------------------------\n");
		strcat(message,"|    ID   |    PEOPLE          |\n");
		strcat(message," ------------------------------\n");
		for (int i = 0; i < countRoom; ++i)
		{
			strcat(message,"|    ");
			char str[10];
			strcpy(str,itoa(rooms[i].id));
			strcat(message, str);
			strcat(message,"    |      ");
			strcpy(str,itoa(rooms[i].countUser));
			strcat(message, str);
			strcat(message,"             |\n");
			strcat(message," ------------------------------\n");
		}
		strcat(message, "\n>JOIN or CREATE_NEW?(JOIN ID-CRRM ID)\n");
		strcat(message, ">ENTER YOUR CHOOSE: ");
	} else {
		strcat(message, "LET'S CREATE ROOM NOW: ");
	}
}

//convert to full message
void changeFull(char message[])
{
	if (strcmp(message, C_FOUND_PASSWORD) == 0)
	{
		strcpy(message,"");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_WIN) == 0)
	{
		strcpy(message,"CONGRATULATION, YOU ARE THE MILLIONAIRE! YOUR REWARD IS 120.000.000 VND\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_LOSE) == 0)
	{
		strcpy(message,"REGRETTABLY, YOU HAVE TO STOP THE GAME HERE TO GIVE WAY TO OTHER PLAYERS!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_LOSE_1) == 0)
	{
		strcpy(message,"REGRETTABLY, YOU HAVE TO LEAVE WITH A BONUS OF 0 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_LOSE_2) == 0)
	{
		strcpy(message,"REGRETTABLY, YOU HAVE TO LEAVE WITH A BONUS OF 5.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_LOSE_3) == 0)
	{
		strcpy(message,"REGRETTABLY, YOU HAVE TO LEAVE WITH A BONUS OF 20.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_LEAV_ROOM_SUC) == 0)
	{
		strcpy(message,"LEAVE ROOM SUCCESSFUL!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_1) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 0 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_2) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 1.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_3) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 2.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_4) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 3.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_5) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 4.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_6) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 5.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_7) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 7.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_8) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 10.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_9) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 12.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_10) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 15.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_11) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 20.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_12) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 30.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_13) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 50.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_14) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 65.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_YOU_STOP_15) == 0)
	{
		strcpy(message,"YOU HAD STOP THIS GAME. YOUR REWARD IS 80.000.000 VND!\n");
		addListRoomTo(message);
	}
	if (strcmp(message, C_CORRECT_PASS) == 0)
	{
		char capcha[6];
		strcpy(message,"");
		strcpy(capcha, sessSignup[posCapchar].capcha);
		strcat(message, "ENTER CAPCHA CODE : ");
		strcat(message, capcha);
		strcat(message, " ");
	}
	if (strcmp(message, C_HELP_50_OK) == 0)
	{
		strcpy(message, "REMAINING :\n");
		strcat(message, resultFromHelp5050);
		memset(resultFromHelp5050,'\0',(strlen(resultFromHelp5050)+1));
		strcat(message, "\n>ENTER YOUR ANSWER : ");
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) return -1;
 	int PORT = atoi(argv[1]);
	char buff[BUFF_SIZE];
	char message[BUFF_SIZE], messCode[BUFF_SIZE], messAcgument[BUFF_SIZE];
	struct pollfd fds[BACKLOG];
	struct sockaddr_in server_addr, client_addr;
	int sin_size = sizeof(client_addr);
	int listen_sock, fdmax, newfd,nbytes,i;

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) <0) {
		perror("Error socket()");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	if (bind(listen_sock, (struct sockaddr *) &server_addr, sizeof(server_addr))<0) {
		perror("Error bind()");
		exit(1);
	}

	if (listen(listen_sock, BACKLOG) == -1) {
		perror("Error listen()");
		exit(1);
	}
	printf("...\n");
	readQues();
	fdmax = 1;
	fds[0].fd = listen_sock;
	fds[0].events = POLLIN;
	while(1) {
		if (poll(fds, fdmax, -1) == -1) {
			perror("Error poll()");
			exit(1);
		}
		for (i = 0; i < fdmax; i++) {
			if (fds[i].revents != 0) {
				if (fds[i].fd == listen_sock) {
					if ((newfd = accept(listen_sock, (struct sockaddr *) &client_addr, (socklen_t*) &sin_size)) < 0) {
						perror("Error accept()");
					} 
					else {
						if (newfd > fdmax) {
							fdmax = newfd;
						}
						printf("Connected\n");
						fds[newfd].fd = newfd;
						fds[newfd].events = POLLIN;
						fds[newfd].revents = 0;
						fdmax++;
						printf("You got a connection from %s\n", inet_ntoa(client_addr.sin_addr));
					}
				} 
				else if(fds[i].revents & POLLIN){
					readFileUser (FILE_NAME);
					//recieve data
					if ((nbytes = recv(fds[i].fd, buff,BUFF_SIZE, 0)) <= 0) {
						if (nbytes == 0)
							printf("Server: socket %d out\n", fds[i].fd);
						close(fds[i].fd);
					}

					else {
						buff[nbytes]='\0';

						if (isValidMessage (buff, messCode, messAcgument))
						{
							printf("messCode:%s\nmessAcgument:%s\n",messCode, messAcgument );
							strcpy(message, process(messCode, messAcgument, client_addr, fds[i].fd));
							changeFull(message);
						}else{
							strcpy(message, "Syntax Error!\n");
						}
						//send data
						if (strcmp(message, NIL) != 0)
						{
							respond(fds[i].fd,message);
							bzero(buff,BUFF_SIZE);
						}else{
							printf("NULL\n");
						}
					}
				}
			}
		}
	}
	return 0;
}