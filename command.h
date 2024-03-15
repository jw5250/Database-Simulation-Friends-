#ifndef COMMAND_H
#define COMMAND_H
#include <stdlib.h>
#include <string.h>
#include "array.h"
//@author Justin Wu

typedef struct commandObject* command;


/**
Make a new command
@param size the require number of arguments for running a command (may be interpreted depending on its function)
@param commandFunc the function to execute, given an array and a size.
@param name the identifier of the command.
*/
command newCommand(size_t size, int (*commandFunc) (array, size_t), const char* name);
/**
Runs a command, given some command.
@param c the command to run
@param a the data to be input
*/
int runCommand(command c, array a);

/**
Delete the command
@param c the pointer to the location of a command
*/
void deleteCommand(void** c);

/**
Get the name of a command.
@param c the command in question
*/
char* getName(command c);

#endif
