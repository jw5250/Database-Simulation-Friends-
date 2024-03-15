#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "array.h"
#include "command.h"
#include "user.h"
#include "HashADT.h"

///@author Justin Wu
//Goals:
	//Finish up documentations.

///The max amount of characters a buffer can take in a single go.
#define BUFFERLEN 256

///For parsing tokens of a string.
#define DELIMITERS " \t\n"

///The point where the tokenizer stops taking in arguments.
#define COMMENTDELIM '#'

///Set of constants for determining the behavior of the shell.
#define CONTINUE 0
#define STOP 1
#define ERROR 2

///Stores all users
static HashADT database = NULL;

///Stores the command line
static array commandsUsed = NULL;

///Stores the status of the program.
static int status = CONTINUE;

///Keep track of total values of the database
static size_t totalFriendships = 0;
static size_t totalPeople = 0;

///Pointer to the file opened by this module
static FILE* f;


///Hash function from hw6.
/**
Hash a string to some number.
@param element the string in question
*/
static size_t str_hash( const void *element ) {
    unsigned char *str = (unsigned char *) element;
    size_t hash = 5381;
    int c;

    while( (c = *str++) ) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return (size_t)hash;
}
///Equals function from hw6.
/**
Compare two strings, given they are keys of a hash table.
@param element1 the first string
@param element2 the second string
*/
static bool str_equals( const void *element1, const void *element2 ) {
    return strcmp( (char*)element1, (char*)element2 ) == 0;
}
///Print the handle (key) and the handle of the user.
/**
Prints the key value pairs of a username and the user's username.
@param element1 the first string
@param element2 the user.
*/
static void printKeyValuePair( const void *element1, const void *element2 ){
	printf( "%s : %s", (char*)element1, getHandle((user)(element2)) );	
}
///Deletion function from hw6.
/**
Delete the key value pair.
@param the key
@param the value
*/
//Needs the delete function.
static void deleteKeyValuePair(void* handle, void* userValue){
	free(handle);
	handle = NULL;
	deleteUser((user)userValue);
}

/**
Checks if a character is alphabetical.
@param c the character in question
@return if c fits the condition or not.
*/
bool isLetter(char c){
	if( ('a' <= tolower(c)) && (tolower(c) <= 'z') ){
		return true;
	
	}
	return false;
}
/**
Checks if a character is numerical.
@param c the character in question
@return if c fits the condition or not.
*/
bool isNumber(char c){
	if( ('0' <= c) && (c <= '9') ){
		return true;
	}
	return false;
}

/**
Checks if a character is a hyphen or single quotation mark.
@param c the character in question
@return if c fits the condition or not.
*/
bool isOther(char c){
	if(c == '-' || c == '\''){
		return true;
	}
	return false;
}

/**
Echo the command input.
@param cLine the output to echo
*/
void printEcho(array cLine){
	printf("+ ");
        for(size_t i = 0; i < getArrLen(cLine);i++){
                printf("\"%s\"", (char*)getItem(cLine, i) );
		if(i != (getArrLen(cLine)-1) ){
			printf(" ");
		}
        }
        printf("\n");
}

/**
Add a user to the database.
@param a the parameters of the shell
@param sizeReq the number of things a must have for the function.
*/
static int add(array a, size_t sizeReq){
	if( (a == NULL) || (getArrLen(a) != sizeReq) ){
		fprintf(stdout, "error: usage: add first-name last-name handle\n");
		return CONTINUE;
	}
        if(f == stdin){
        	printEcho(a);
        }
	char* first = (char*)getItem(a, 1);
	char* last = (char*)getItem(a, 2);
	char* username = (char*)getItem(a, 3);
        for(size_t i = 0; i < strlen(first);i++){
                if( (i == 0) && !(isLetter(first[i])) ){
			fprintf(stdout, "error: argument \"%s\" is invalid\n", first);
                        return ERROR;
		}
                if(  !( isOther(first[i]) || isLetter(first[i]) ) ){
                        fprintf(stdout, "error: argument \"%s\" is invalid\n", first);
                        return ERROR;
                }
        }
        for(size_t i = 0; i < strlen(last);i++){
                if( (i == 0) && !(isLetter(last[i])) ){
                        fprintf(stdout, "error: argument \"%s\" is invalid\n", last);

                        return ERROR;
                }
                if(  !( isOther(last[i]) || isLetter(last[i]) ) ){
                        fprintf(stdout, "error: argument \"%s\" is invalid\n", last);
                        return ERROR;
                        
                }
        }
        for(size_t i = 0; i < strlen(username);i++){
                if( (i == 0) && !(isLetter(username[i])) ){
                        fprintf(stdout, "error: argument \"%s\" is invalid\n", username);
                        return ERROR;
                }
                if(  !( isNumber(username[i]) || isLetter(username[i]) ) ){
                        fprintf(stdout, "error: argument \"%s\" is invalid\n", username);
			return ERROR;
                }
        }
	if(ht_has(database, username)){
		fprintf(stdout, "error: handle \"%s\" is already in use\n", username);
		return ERROR;
	}
	char* key = malloc(sizeof(char)*(strlen(username) + 1));
	strcpy(key, username);
	user new = newUser(first, last, username, NULL);
	ht_put(database, key, new);
	totalPeople++;
	return CONTINUE;
}
/**
Have a user friend another.
@param a the parameters of the shell
@param sizeReq the number of things a must have for the function.
*/
static int friend(array a, size_t sizeReq){

	if( (a == NULL) || (getArrLen(a) != sizeReq) ){
		fprintf(stdout, "error: usage: friend handle1 handle2\n");
		return ERROR;
	}
        if(f == stdin){
                printEcho(a);
        }
	char* handle1 = (char*)getItem(a, 1);
	char* handle2 = (char*)getItem(a, 2);
	
	if(!ht_has(database, handle1)){
                fprintf(stdout, "error: handle \"%s\" is unknown\n", handle1);
                return ERROR;
        }
        if(!ht_has(database, handle2)){
                fprintf(stdout, "error: handle \"%s\" is unknown\n", handle2);
                return ERROR;
        }
	user user1 = (user)ht_get(database, handle1);
	user user2 = (user)ht_get(database, handle2);
	if(/*cmpUsers(user1, user2)*/strcmp(handle1, handle2) == 0){
		fprintf(stdout, "error: \"%s\" and \"%s\" are the same person\n", handle1, handle2);
		return ERROR;
        }
	int status = friendUsers(user1, user2);
	if(status == EXIT_SUCCESS){
		totalFriendships++;
	}
	//Find if the two users exist.
	//Call function that adds the users as friends.
	return CONTINUE;
}
/**
Have a user unfriend another.
@param a the parameters of the shell
@param sizeReq the number of things a must have for the function.
*/
static int unfriend(array a, size_t sizeReq){

	if( (a == NULL) || (getArrLen(a) != sizeReq) ){
		fprintf(stdout, "error: usage: unfriend handle1 handle2\n");
		return CONTINUE;
	}
	if(f == stdin){
                printEcho(a);
        }
	char* handle1 = (char*)getItem(a, 1);
	char* handle2 = (char*)getItem(a, 2);
	
	if(!ht_has(database, handle1)){
		fprintf(stdout, "error: handle \"%s\" is unknown\n", handle1);
		return ERROR;	
	}
	if(!ht_has(database, handle2)){
		fprintf(stdout, "error: handle \"%s\" is unknown\n", handle2);
		return ERROR;
	}
	user user1 = (user)ht_get(database, handle1);
	user user2 = (user)ht_get(database, handle2);
	if(/*cmpUsers(user1, user2)*/strcmp(handle1, handle2) == 0){
		fprintf(stdout, "error: \"%s\" and \"%s\" are the same person\n", handle1, handle2);
		return ERROR;
	}
	int status = unfriendUsers(user1, user2);
	if(status == EXIT_SUCCESS){
		totalFriendships--;
	}
	//Find if the two users exist.
	//Call function that adds the users as friends.
	return CONTINUE;
}
/**
Print the friends of the user.
@param a the parameters of the shell
@param sizeReq the number of things a must have for the function.
*/
static int print(array a, size_t sizeReq){
	if( (a == NULL) || (getArrLen(a) != sizeReq) ){
                fprintf(stdout, "error: usage: print handle\n");
                return ERROR;
        }
        if(f == stdin){
                printEcho(a);
        }
        char* handle = (char*)getItem(a, 1);
        //Find if the user exists.
        if(!ht_has(database, handle)){
                fprintf(stdout, "error: handle \"%s\" is unknown\n", handle);
                return ERROR;
        }
        user user1 = (user)ht_get(database, handle);
	printFriends(user1);
	return CONTINUE;
}
/**
Print the amount of friends of a user.
@param a the parameters of the shell
@param sizeReq the number of things a must have for the function.
*/
static int size(array a, size_t sizeReq){
	if( (a == NULL) || (getArrLen(a) != sizeReq) ){
		fprintf(stdout, "error: usage: size handle\n");
		return ERROR;
	}
        if(f == stdin){
                printEcho(a);
        }
	char* handle = (char*)getItem(a, 1);
	//Find if the user exists.
	if(!ht_has(database, handle)){
		fprintf(stdout, "error: handle \"%s\" is unknown\n", handle);
		return ERROR;
	}
	user user1 = (user)ht_get(database, handle);
	userFriendNumber(user1);
	return CONTINUE;
	//Call function.
}
/**
Get the statistics of the database.
@param a the parameters of the shell
@param sizeReq the number of things a must have for the function.
*/
static int stats(array a, size_t sizeReq){
	if( (a == NULL) || (getArrLen(a) != sizeReq) ){
		fprintf(stdout, "error: usage: stats\n");
		return ERROR;
	}
	//Print format:
        if(f == stdin){
                printEcho(a);
        }
	//Don't know how I can get the size from a void pointer. How could I do that?
	fprintf(stdout, "Statistics:  ");
	if(totalPeople == 0){
		fprintf(stdout, "no people, ");
	}else if(totalPeople == 1){
		fprintf(stdout, "%lu person, ", totalPeople);
	}else{
		fprintf(stdout, "%lu people, ", totalPeople);
	}
	if(totalFriendships == 0){
		fprintf(stdout, "no friendships\n");
	}else if(totalFriendships == 1){
		fprintf(stdout, "%lu friendship\n", totalFriendships);
	}else{
		fprintf(stdout, "%lu friendships\n", totalFriendships);
	}
	return CONTINUE;
	
}
/**
Restart the program.
@param a the parameters of the shell
@param sizeReq the number of things a must have for the function.
*/
static int init(array a, size_t sizeReq){
        if( (a == NULL) || (getArrLen(a) != sizeReq) ){
		fprintf(stdout, "error: usage: init\n");
                return ERROR;
        }
        if(f == stdin){
                printEcho(a);
        }
	ht_destroy(database);
	database = ht_create(str_hash, str_equals, printKeyValuePair, deleteKeyValuePair);
	totalFriendships = 0;
	totalPeople = 0;
	fprintf(stdout, "System re-initialized\n");
	return CONTINUE;

}
/**
Stop the shell.
@param a the parameters of the shell
@param sizeReq the number of things a must have for the function.
*/
static int quit(array a, size_t sizeReq){
	if(a == NULL || getArrLen(a) != sizeReq){
		fprintf(stdout, "error: usage: quit\n");
		return CONTINUE;
	}
        if(f == stdin){
                printEcho(a);
        }
	if(database != NULL){
		ht_destroy(database);
		database = NULL;
	}
	return STOP;
	//Delete everything in the system. Make system terminate with EXIT_SUCCESS.
}

/**
Auxiliary function for freeing a string that is dynamically allocated.
@param f the pointer to the dynamically allocated thing.
*/
static void freeString(void** f){
	if(f == NULL){
		return;
	}
	if(*f != NULL){
		free(*f);
		*f = NULL;
	}
	return;
}

/**
Go through the list of available commands to see if it fits.
@param commandLine the arguments to parse.
*/
int doOperation(array commandLine){
	int status = CONTINUE;
	for(size_t i = 0; i < getArrLen(commandsUsed);i++){
		
		if( strcmp(getName((command)getItem(commandsUsed, i)), (char*)getItem(commandLine, 0)) == 0 ){
			status = runCommand((command)getItem(commandsUsed, i), commandLine);			
			break;
		}
		
	}
	return status;
}
//Loop should print error messages given some input without terminating the shell.
/**
The shell loop.
@param filename the name of the file to open.
*/
int shellLoop(char* filename){
	database = ht_create(str_hash, str_equals, printKeyValuePair, deleteKeyValuePair);
	char buffer[BUFFERLEN];
	array commandLine = NULL;
	if(filename == NULL){
		f = stdin;
	}else{
		f = fopen(filename, "r");
		if(f == NULL){
			perror(filename);
			ht_destroy(database);
			return EXIT_FAILURE;
		}
	}
	while( status != STOP ){
		if(f == stdin){
			printf("Amici> ");
		}
		if(fgets(buffer, BUFFERLEN, f) == NULL){
			fclose(f);
			f = stdin;
			continue;//How does this keep the UI from breaking???
		}
		
		commandLine = newArray(freeString);
		char* token;
		token = strtok(buffer, DELIMITERS);
		
		while( (token != NULL) && (*token != COMMENTDELIM) ){
			char* string = malloc(strlen(token) + 1);
			strcpy(string, token);
			addToArray(commandLine, string);		
			token = strtok(NULL, DELIMITERS);
		}
		
		if( getArrLen(commandLine) == 0 ){
			deleteArray(&commandLine);
			continue;
		}
		status = doOperation(commandLine);
		deleteArray(&commandLine);
	}
	if(database != NULL){
		ht_destroy(database);
	}
	deleteArray(&commandLine);
	fclose(f);
	if(status == ERROR){
		return EXIT_FAILURE;
	}else{
		return EXIT_SUCCESS;
	}
}
int main(int argc, char** argv){
	if(argc > 2){
		fprintf(stderr, "usage: amici [ datafile ]\n");
		return EXIT_FAILURE;
	}
	//Only one command is added to the list. why?	
	commandsUsed = newArray(deleteCommand);
	addToArray(commandsUsed, newCommand(1, init, "init"));
	addToArray(commandsUsed, newCommand(1, quit, "quit"));
	addToArray(commandsUsed, newCommand(1, stats, "stats"));
	addToArray(commandsUsed, newCommand(4, add, "add"));
	addToArray(commandsUsed, newCommand(3, friend, "friend"));
	addToArray(commandsUsed, newCommand(3, unfriend, "unfriend"));
	addToArray(commandsUsed, newCommand(2, size, "size"));
	addToArray(commandsUsed, newCommand(2, print, "print"));
	
	char* name = NULL;
	if(argc == 2){
		name = argv[1];
	}
	int programStatus = shellLoop(name);
	deleteArray(&commandsUsed);
	
	if(programStatus == EXIT_SUCCESS){
		return EXIT_SUCCESS;
	}else{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

