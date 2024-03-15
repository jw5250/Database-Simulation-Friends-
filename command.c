#include "command.h"


/**
Object representing a single command. Requires a name, a required amount of arguments, and a function that takes in an array of arguments.
*/
struct commandObject{
	size_t argReq;
	//Function that uses an array of arguments and the amount that is needed.
                //May modify global variables.
                //Needs size_t and conditional functions to properly sanitize each element.
                        //Needed for:
                                //Adding friends
                                //Removing friends
                                //Adding people to database
                                //Reseting database
                                //Displaying all of the stats of the database
	int (*func)(array, size_t);
	char* name;
};

command newCommand(size_t argNum, int (*commandFunc) (array, size_t), const char* id){
	command c = malloc(sizeof(struct commandObject));
	if(c == NULL){
		fprintf(stderr, "Can't allocate memory.\n");
		return NULL;
	}
	c->name = calloc((strlen(id)+1), sizeof(char));
	if(c->name == NULL){
		fprintf(stderr, "Can't allocate memory.\n");
		deleteCommand((void**)(&c));
		return NULL;
	}
	strcpy(c->name, id);
	c->func = commandFunc;
	c->argReq = argNum;
	return c;
}

int runCommand(command c, array arr){
	return c->func(arr, c->argReq);
}

void deleteCommand(void** c){
	if(c == NULL){
		return;
        }
	if(*c != NULL){
		if( ((command)(*c))->name != NULL){
			free( ((command)(*c))->name);
			((command)(*c))->name = NULL;
		}
		free(*c);
		*c = NULL;
	}
}

char* getName(command c){
	return c->name;
}
