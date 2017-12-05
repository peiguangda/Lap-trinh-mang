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
#define CRRM "CRRM" //create room

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
#define C_HELP_50_NOT_OK "15"
#define C_STOP_OK "24"
#define C_STOP_NOT_OK "25"
#define C_CRE_ROOM_SUC "34"
#define C_CRE_ROOM_FAI "35"
#define C_YOU_WIN "45"
#define C_YOU_LOSE "44"
#define C_YOU_LOSE_KEY_ROOM "43"

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
#define QUES_IN_LEVER 7
#define QUES_NUMBER 21

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
	int level; //1 : easy, 2 : medium, 3 : hard
	char content[100];
	char choiceA[50];
	char choiceB[50];
	char choiceC[50];
	char choiceD[50];
	char answer;
	int quesStatus; //da co nguoi tra loi nhanh nhat chua?
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
	struct Room room;
	for (int i = 0; i < countRoom; ++i)
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
	countRoom++;
	return rooms[countRoom-1];
}

//print question and lever
void printListQues(){
	int i;
	printf("Easy list :\n");
	for (i = 0; i < QUES_NUMBER; i++){
		printf("%s\n%s\n%s\n%s\n%s\n%c\n", questionList[i].content, questionList[i].choiceA, questionList[i].choiceB, questionList[i].choiceC, questionList[i].choiceD, questionList[i].answer);
	}
}

//
void printRoom(struct Room room)
{
	printf("room id :%d\n",room.id );
	printf("room countUser :%d\n",room.countUser );
	printf("room countQues :%d\n",room.countQues );
	printf("room status :%d\n",room.roomStatus );
}

//
void printSession(int pos)
{
	if (pos <= 0) return;
	printf("Session status: %d\n", sess[pos].sessStatus);
	printf("Session countlogin: %d\n", sess[pos].countLogin);
	printRoom(sess[pos].room);
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
	char answ;
	int i = 0;
	fptr = fopen(QUES_FILENAME, "r");
	if (fptr == NULL) {
		printf("Can't open file !\n");
		return;
	}
	
	while(fscanf(fptr, "%d | %d | %[^|] | %[^|] | %[^|] | %[^|] | %[^|] | %c", &id, &lv, content, a, b, c, d, &answ) != EOF){
		questionList[i].id = id;
		questionList[i].level = lv;
		strcpy(questionList[i].content, content);
		strcpy(questionList[i].choiceA, a);
		strcpy(questionList[i].choiceB, b);
		strcpy(questionList[i].choiceC, c);
		strcpy(questionList[i].choiceD, d);
		questionList[i].answer = answ;
		addLeverList(id, lv);
		i++;
	}
	fclose(fptr);
}

//random index of question
int randomQuestion(int levelList[]){
	int result;
	srand(time(NULL));
	int random = rand() % QUES_IN_LEVER ; //  0 to 6
	// result = levelList[random];
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

int checkPass(char pass[]){
	//Password has min_length is 5
	int i;
	if (strlen(pass) < 5) return 0;
	else return 1;
}

//create capcha code, include 6 random character
char *makeCapcha(){
	char *capcha = (char*) malloc(6*sizeof(char));
	int i;
	srand(time(NULL));
	for(i = 0; i < 6; ++i){
    		capcha[i] = '0' + rand()%72; // starting on '0', ending on '}'
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

void showListRoom()
{
	
}

//process while Code is USER
char *userCodeProcess(struct sockaddr_in cliAddr, int connd, int pos, int i)
{
	struct Session session;
	if (i == -1) return C_NOT_FOUND_ID; //if not found user
	if (users[i].userStatus == BLOCKED)	return C_BLOCK; //if found user but user blocked
	if (pos == -1) //if not found session
	{
		printf("aaa\n");
		session = newSession(users[i], NOT_AUTHENTICATED, cliAddr, connd);// create new session
		addSession(session);
		printf("session status:%d\n",sess[sessCount-1].sessStatus);                                       // add session
		return C_FOUND_ID;
	}
	//found session
	else if (sess[pos].sessStatus == NOT_AUTHENTICATED || sess[pos].sessStatus == NOT_IDENTIFIED_USER) {//found user != user of session
		sess[pos].sessStatus = NOT_AUTHENTICATED;
		printf("aaa\n");
		printf("session status:%d\n",sess[pos].sessStatus);
		memcpy(&(sess[pos].user), &users[i], sizeof(struct User));
		return C_FOUND_ID;
	}
	return "Login Sequence Is Wrong";
}

//Process while Code is PASS
char *passCodeProcess(char messAcgument[], int pos)
{
	int i;
	printf("session status:%d\n",sess[pos].sessStatus);
	if (sess[pos].user.userStatus == BLOCKED)
		{
			return C_BLOCK;
		}
		//if PASS ok
		if (strcmp (sess[pos].user.password, messAcgument) == 0) 
		{
			sess[pos].sessStatus = AUTHENTICATED; //next status  
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
		sess[pos].sessStatus = WAIT_QUICH_QUES;
		sess[pos].room = room;
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
		printf("countUser:%d\n", rooms[i].countUser);
	}else {
		printf("room full slot\n");
	}
}

//Process while Code is ROOM
char *joinCodeProcess(char messAcgument[], int pos, int connd)
{
	//in ra nguoi choi ... da vao phong ...
	//them nguoi choi vao room nay
	int i = findRoomById(atoi(messAcgument));
	if (i < 0 ){ //phong khong ton tai
		return C_OUT_ROOM;
	} else if (rooms[i].roomStatus == PLAY){ //phong dang choi
		return C_OUT_ROOM;
	}else {
		addUserToRoom(i, connd, sess[pos].user);
		sess[pos].sessStatus = WAIT_QUICH_QUES;
		sess[pos].room = rooms[i];

		return C_IN_ROOM;
	}
}


//make full question (question + choose answer)
char *makeFullQues(Question question)
{
	char *content = NULL;
	content = question.content;
	strcat(content, "\n");
	strcat(content, question.choiceA);
	strcat(content, "\n");
	strcat(content, question.choiceB);
	strcat(content, "\n");
	strcat(content, question.choiceC);
	strcat(content, "\n");
	strcat(content, question.choiceD);
	return content;
}

////Process while Code is STAR
char *starCodeProcess(char messAcgument[], int pos, struct sockaddr_in cliAddr)
{
	int posRoom, posSess, random;
	//if messAcgument is id of room
	if (sess[pos].room.id == atoi(messAcgument))
	{
		//if is room master
		if (memcmp(&(sess[pos].user), &(sess[pos].room.users[0]), sizeof(struct User))  == 0)
		{
			//send question to all users of room
			posRoom = findRoomById(atoi(messAcgument));
			rooms[posRoom].roomStatus = PLAY;  //set trang thai room = dang choi cho thang khac ko vao dc
			for (int i = 0; i <= rooms[posRoom].countUser; ++i)
			{
				posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[i]); //find session of user in room
				random = randomQuestion(easyList); //random quick question
				quickQues = questionList[random];
				quickQues.quesStatus = 0; //chua co nguoi tra loi dung
				respond(rooms[posRoom].connd[i], makeFullQues(quickQues));
				sess[posSess].sessStatus = PLAYING_QUICK_QUES;
				sess[posSess].room = rooms[posRoom]; //update status room is PLAY on session
			}
			return "NULL";
		}else {
			return "you are not room master";
		}
	}else {
		return "Id room fails";
	}
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
			random = randomQuestion(easyList); //random question easy lever
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			printf("medium ok\n");
			random = randomQuestion(mediumList) + QUES_IN_LEVER; //random question medium lever
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
			random = randomQuestion(hardList) + QUES_IN_LEVER * 2; //random question hard lerver
			break;
		case 15:
			break;
		default:
			break;		
	}
	// question = questionList[random -1];
	printListQues();
	printf("questionList[random -1].content:%s\n",questionList[random -1].content );
	return questionList[random -1];
}


//Process while Code is ANSW -quick question
char *answQuickCodeProcess(char messAcgument[], int pos)
{
	Question question;
	int posRoom = findRoomById(sess[pos].room.id);
	if(strlen(messAcgument) == 1 && messAcgument[0] == quickQues.answer && quickQues.quesStatus == 0){
		//tra loi dung
			sess[pos].sessStatus = PLAYING; //set main
			quickQues.quesStatus = 1;
			//return question to main && save question to list question of session
			question = getQuestion(pos);
			rooms[posRoom].questions[0] = question;
			rooms[posRoom].countQues++;
			sess[pos].room = rooms[posRoom];
			return makeFullQues(question);
			// return C_A_QQ_CORRECT;
	}else{
		sess[pos].sessStatus = WATCHING; //chi dc xem
		//todo cho client lang nghe lien
		return C_A_QQ_INCORRECT;
	}
}

//Process while Code is ANSW --main
char *answCodeProcess(char messAcgument[], int pos, struct sockaddr_in cliAddr)
{
	Question question;
	int posSess;
	int posRoom = findRoomById(sess[pos].room.id);
	int countQues = rooms[posRoom].countQues;
	printf("countQues: %d\n",countQues );
	printf("rooms[posRoom].questions[countQues-1]: %c\n",rooms[posRoom].questions[countQues-1].answer );
	if(strlen(messAcgument) == 1 && messAcgument[0] == rooms[posRoom].questions[countQues-1].answer){
		//return question to main && save question to list question of session && tang so cau tra loi dung len +1
		if (rooms[posRoom].countQues >= MAX_QUESTION)
		{
			//todo set status all member of room to wait_QQ
			return C_YOU_WIN;
		}
		question = getQuestion(pos);
		rooms[posRoom].questions[countQues] = question;
		rooms[posRoom].countQues++;
		sess[pos].room = rooms[posRoom];
		printf("DDSFSAF : %s\n",makeFullQues(question) );
		// content = question.content;

		return makeFullQues(question);
		// return C_A_QQ_CORRECT;
	} else {
		//tra loi sai
		rooms[posRoom].roomStatus = WAIT;
		rooms[posRoom].countQues = 0;
		posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[0]); //find session of user in room
		respond(rooms[posRoom].connd[0], C_YOU_LOSE_KEY_ROOM);
		sess[posSess].sessStatus = WAIT_QUICH_QUES;
		sess[posSess].room = rooms[posRoom]; //update status room is PLAY on session
		for (int i = 1; i <= rooms[posRoom].countUser; ++i)
		{
			posSess = findSessByAddr(cliAddr, rooms[posRoom].connd[i]); //find session of user in room
			respond(rooms[posRoom].connd[i], C_YOU_LOSE);
			sess[posSess].sessStatus = WAIT_QUICH_QUES;
			sess[posSess].room = rooms[posRoom]; //update status room is PLAY on session
		}
		//muc tien thuong = rooms[posRoom].countQues / 5
		return "NULL";//thong bao ban da ra ve voi so tien ...
	} 
	
}


//Process while Code is LOUT
char *loutCodeProcess(char messAcgument[], int pos)
{
	if (strcmp(sess[pos].user.id, messAcgument) == 0) //check userId is valid?
		{
			sess[pos].sessStatus = NOT_IDENTIFIED_USER; //reset session status
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
	else if (sessSignup[pos].sessStatus == START_SIGNUP || sessSignup[pos].sessStatus == USERNAME_CREATED) {//found user != user of session
		sessSignup[pos].sessStatus = USERNAME_CREATED;
		memcpy(&(sessSignup[pos].user), &user, sizeof(struct User));
		return C_NEW_USER;
	}
	return "Login Sequence Is Wrong";
}


//Process while Code is SIGP
char *sigpCodeProcess(char messAcgument[], int posSign)
{
	int i;
	if (checkPass(messAcgument)){
		strcpy(sessSignup[posSign].user.password, messAcgument);
		strcpy(sessSignup[posSign].capcha, makeCapcha());
		posCapchar = posSign;
		sessSignup[posSign].sessStatus = PASSWORD_CREATED; //next status  
		return C_CORRECT_PASS;
	}
	else return C_INCORRECT_PASS;
}

//Process while Code is SIGC
char *sigcCodeProcess(char messAcgument[], int pos)
{
	if (strcmp(sessSignup[pos].capcha, messAcgument) == 0) //check capcha
	{
		sessSignup[pos].sessStatus = SIGNUP_SUCCESSFUL; 
		sessSignup[pos].user.count = userCount;
		if (addUser(sessSignup[pos].user))
		{
			writeUserToFile(FILE_NAME);
			return C_CORRECT_CODE;
		}
	} else 
		return C_INCORRECT_CODE;
}


//process request
char *process(char messCode[], char messAcgument[], struct sockaddr_in cliAddr, int connd )
{
	int pos,  posSign, i;
	pos = findSessByAddr(cliAddr, connd); //find Session return -1 if session not exists
	posSign = findSessSignByAddr(cliAddr, connd);

	/***********messcode is USER***********/
	if (strcmp(messCode, USER) == 0 ){
		// printSession(pos);
		i = findUserById (messAcgument); //find user return -1 if user not exists
		return userCodeProcess(cliAddr,connd, pos, i);
	}

	/********messcode is PASS**********/
	if (strcmp(messCode, PASS) == 0 && pos != -1 && sess[pos].sessStatus == NOT_AUTHENTICATED )
	{
		// printSession(pos);
		printf("session status:%d\n",sess[pos].sessStatus);
		return passCodeProcess(messAcgument, pos);
	}

	/********messcode is CRRM**********/
	if (strcmp(messCode, CRRM) == 0 && pos != -1 && sess[pos].sessStatus == AUTHENTICATED )
	{
		// printSession(pos);
		printf("session status:%d\n",sess[pos].sessStatus);
		return crrmCodeProcess(messAcgument, pos);
	}

	/********messcode is JOIN**********/
	if (strcmp(messCode, JOIN) == 0 && pos != -1 && sess[pos].sessStatus == AUTHENTICATED )
	{
		// printSession(pos);
		printf("session status:%d\n",sess[pos].sessStatus);
		return joinCodeProcess(messAcgument, pos, connd);
	}

	/********messcode is STAR**********/
	if (strcmp(messCode, STAR) == 0 && pos != -1 && sess[pos].sessStatus == WAIT_QUICH_QUES )
	{
		// printSession(pos);
		printf("session status:%d\n",sess[pos].sessStatus);
		return starCodeProcess(messAcgument, pos, cliAddr);
	}

	/********messcode is ANSW**********/
	if (strcmp(messCode, ANSW) == 0 && pos != -1 && sess[pos].sessStatus == PLAYING_QUICK_QUES )
	{
		// printSession(pos);
		printf("session status:%d\n",sess[pos].sessStatus);
		return answQuickCodeProcess(messAcgument, pos);
	}

	/********messcode is ANSW**********/
	if (strcmp(messCode, ANSW) == 0 && pos != -1 && sess[pos].sessStatus == PLAYING )
	{
		// printSession(pos);
		return answCodeProcess(messAcgument, pos, cliAddr);
	}
	/********messcode is LOUT*********/
	// if (strcmp(messCode, LOUT) == 0 && pos != -1 && sess[pos].sessStatus == AUTHENTICATED)
	// {
	// 	return loutCodeProcess(messAcgument, pos);
	// }

	/********messcode is SIGU*********/
	if (strcmp(messCode, SIGU) == 0)
	{
		i = findUserById (messAcgument); //find user return -1 if user not exists
		return siguCodeProcess(messAcgument, cliAddr,connd, posSign, i);
	}

	/********messcode is SIGP*********/
	if (strcmp(messCode, SIGP) == 0 && posSign != -1 && sessSignup[posSign].sessStatus == USERNAME_CREATED)
	{
		return sigpCodeProcess(messAcgument, posSign);
	}

	/********messcode is SIGC*********/
	if (strcmp(messCode, SIGC) == 0 && posSign != -1 && sessSignup[posSign].sessStatus == PASSWORD_CREATED)
	{
		return sigcCodeProcess(messAcgument, posSign);
	}

	else
	{
		return "Login Sequence Is Wrong";
	}
}

//convert to full message
void changeFull(char message[])
{
	if (strcmp(message, C_FOUND_PASSWORD) == 0)
	{
		strcat(message, " -> Password ok\n");
		if (countRoom > 0){
			strcat(message, "List room :");
			for (int i = 0; i < countRoom; ++i)
			{
				strcat(message, "\nRoom id :");
				char str[10];
				sprintf(str, "%d",rooms[i].id);
				strcat(message, str);
				strcat(message, "\tPeople count : ");
				sprintf(str, "%d",rooms[i].countUser);
				strcat(message, str);
				strcat(message, " Choose the room or create new room?\n");
			}
		} else {
			strcat(message, "No rooms were created! Let's create room now!");
		}
	}else if (strcmp(message, C_CORRECT_PASS) == 0)
	{
		char capcha[6];
		strcpy(capcha, sessSignup[posCapchar].capcha);
		strcat(message, " -> Please enter capcha code : ");
		strcat(message, capcha);
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
	// printListQues();
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
					// showUser();
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
							strcpy(message, "Syntax Error!");
						}
						//send data
						if (strcmp(message, "NULL") != 0)
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