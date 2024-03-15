#include "user.h"

#define MAXFRIENDS 30
struct userObject{
        char* firstName;
        char* lastName;
        char* handle;
        array friendList;//Pointer to a bunch of pointers to friends. Should NEVER free given it only allocates pointers to memory instead of copying the memory of the friend.
        size_t maxFriends;//No idea what I should do with this. Probably remove later unless specified.

};

user newUser(char* first, char* last, char* username, void freeList(void**)){
        user newUser = malloc(sizeof(struct userObject));
        newUser->firstName = calloc(strlen(first) + 1, sizeof(char));
        strcpy(newUser->firstName, first);

        newUser->lastName = calloc(strlen(last) + 1, sizeof(char));
        strcpy(newUser->lastName, last);
        
	newUser->handle = calloc(strlen(username) + 1, sizeof(char));
        strcpy(newUser->handle, username);
        
	newUser->maxFriends = MAXFRIENDS;
        newUser->friendList = newArray(freeList);
        return newUser;
}

/**
Compare user handles.
@param u1 the first user
@param u2 the second user
*/
static int compareHandles(user u1, user u2){
        if(strcmp(u1->handle, u2->handle) == 0){
                return 0;
        }else{
                return 1;
        }
}

int friendUsers(user u1, user u2){

        for(size_t i = 0; i < getArrLen(u1->friendList);i++){
                if( compareHandles( (user)getItem(u1->friendList, i), u2 ) == 0 ){
                        fprintf(stdout, "%s and %s are already friends.\n", u1->handle, u2->handle);
                        return EXIT_FAILURE;
                }
        }
        addToArray(u1->friendList, u2);
        addToArray(u2->friendList, u1);
        fprintf(stdout, "%s and %s are now friends.\n", u1->handle, u2->handle);
        return EXIT_SUCCESS;
}

int unfriendUsers(user u1, user u2){
        int friendshipExists = 0;
        for(size_t i = 0; i < getArrLen(u1->friendList);i++){
                if( compareHandles( (user)getItem(u1->friendList, i), u2 ) == 0 ){
                        friendshipExists = 1;
                }
        }
        if(friendshipExists == 0){
                fprintf(stdout, "%s and %s are not friends.\n", u1->handle, u2->handle);
                return EXIT_FAILURE;
        }else{
                for(size_t i = 0; i < getArrLen(u1->friendList);i++){
                        if( compareHandles( (user)getItem(u1->friendList, i), u2 ) == 0 ){
                                removeItem(u1->friendList, i);
                        }
                }
                for(size_t i = 0; i < getArrLen(u2->friendList);i++){
                        if( compareHandles( (user)getItem(u2->friendList, i), u1 ) == 0 ){
                                removeItem(u2->friendList, i);
                        }
                }
		fprintf(stdout,"%s and %s are no longer friends.\n", u1->handle, u2->handle);
        }
        return EXIT_SUCCESS;

}


void userFriendNumber(user u){
        fprintf(stdout, "%s (%s %s) has ", u->handle, u->firstName, u->lastName);
        if(getArrLen(u->friendList) == 0){
                fprintf(stdout, "no friends\n");
        }else if(getArrLen(u->friendList) == 1){
		fprintf(stdout, "%lu friend\n", getArrLen(u->friendList));
	}else{
                fprintf(stdout, "%lu friends\n", getArrLen(u->friendList));
        }
        return;
}


void printFriends(user u){
        userFriendNumber(u);
        if(getArrLen(u->friendList) > 0){
                for(size_t i = 0; i < getArrLen(u->friendList);i++){
                        user friend = (user)(getItem(u->friendList, i));
                        fprintf(stdout, "\t%s (%s %s)\n", (char*)friend->handle, (char*)friend->firstName, (char*)friend->lastName);
                }
        }
        return;
}

void deleteUser(user u){
        if( u != NULL){
                if( u->firstName != NULL ){
                        free( u->firstName );
                        u->firstName = NULL;
                }
                if( u->lastName != NULL ){
                        free( u->lastName );
                        u->lastName = NULL;
                }
                if( u->handle != NULL ){
                        free( u->handle );
			u->handle = NULL;
		}
                deleteArray( &(u->friendList) );

                free(u);
                u = NULL;
        }
        return;
}

char* getHandle(user u){
	return u->handle;
}

int cmpUsers(user u1, user u2){
	if( strcmp(u1->firstName, u2->firstName) == 0 && strcmp(u1->lastName, u2->lastName) == 0 ){
		return 0;
	}else{
		return 1;
	}

}
/*
User Test
        user one = newUser("Bob", "Jones", "Bob1", NULL);
        user two = newUser("Bob", "Bob", "Bob2", NULL);
        user three = newUser("Bob", "Bob", "Bob3", NULL);
        unfriendUsers(three, two);
        printFriends(three);

        friendUsers(three, two);
        friendUsers(two, three);
        friendUsers(three, one);

        printFriends(three);
        unfriendUsers(three, two);
        printFriends(three);


        deleteUser(&one);
        deleteUser(&two);
        deleteUser(&three);
	*/
