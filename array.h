#ifndef ARRAY_H
#define ARRAY_H
#define ARRCAP 10;
#include <stdlib.h>
#include <stdio.h>
//@author Justin Wu, jw5250


//Array data structure made to dynamically handle commands.

typedef struct arrBody* array;


/**
Constructor for building new array.
@param freeMem the function for freeing memory of all items in the array.
*/
array newArray(void (freeMem)(void**) );


/**
Add to the back of the array
@param a the array in question
@param item the pointer to the item to add.
*/
void addToArray(array a, void* item);


/**
Remove an element of an array at index i. Shifts everything beyond it down.
@param a the array in question
@param i the index.
*/
void removeItem(array a, size_t i);

/**
Delete the array
@param a the location of the array to be deleted.
*/
void deleteArray(array* a);

/**
Get a pointer to an item at some array. Does not deep copy. Checks bounds.
@param a the array in question
@param i the index of the array to access

*/
void* getItem(array a, size_t i);



/**
Get the length of the array
@param a the array in question
*/
size_t getArrLen(array a);
#endif
