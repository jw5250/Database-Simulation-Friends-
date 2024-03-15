#ifndef USER_H_INCULDED
#define USER_H_INCLUDED
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "array.h"
typedef struct userObject* user;


/**
Makes a new user.
@param first the first name of the person. First letter must be alphabetical
@param last the last name of the person. First letter must be alphabetical
@param username the username of the person. First letter must be alphabetical.
@param freeList the function responsible for freeing members of the array.
@return a new user.
*/
user newUser(char* first, char* last, char* username, void freeList(void**));

/**
Adds friends between two users.
Assumes u1 and u2 have each other in their friend list or don't.
@param u1 the first friend
@param u2 the second friend
*/
int friendUsers(user u1, user u2);

/**
Removes two users from each other's friend list.
@param u1 the first friend
@param u2 the second friend
*/
int unfriendUsers(user u1, user u2);


/**
Print the amount of friends a user has.
@param u the user in question
*/
void userFriendNumber(user u);

/**
Prints the friends of a user.
@param u the user in question
*/
void printFriends(user u);

/**
Deletes a user.
@param u the user.
*/
void deleteUser(user u);


/**
Gets the handle of a user.
@param u the user in question.
*/
char* getHandle(user u);

/**
Check if two users are the same person.
*/
int cmpUsers(user u1, user u2);
#endif
