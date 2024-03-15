#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "HashADT.h"

///Username:jw5250
///Name:Justin Wu

///
/// General Notes on hash table Operation
///
/// - The operations identified below are the public interface to HashADT.
///
/// - The client supplies functions for hashing the key, checking key
///   equality, printing key, value pairs, and deleting pairs.
///
/// - The table assumes ownership of keys and values.  The delete function
///   (by default) will free the (key,value) pair upon destruction; to
///   prevent this, the application should pass a NULL pointer for the
///   delete function, which causes the delete function to NOT free the
///   (key, value) pair.
///
/// - There is no remove functionality. Entries remain until you call destroy.
///
/// - The destroy calls a no-operation delete if the client passes NULL destroy.
///
/// - Wherever a function has a precondition, and the client violates the
///   condition, and the code detects the violation, then the function will
///   assert failure and abort.
///
/// - Wherever a function has a postcondition, and the client violates the
///   condition, and the code detects the violation, then the function will
///   assert failure and abort.
///

///Separate thing for key value pairs.
typedef struct mapPair{
	void* key;
	void* value;
} keyValuePair;

//Note: Rehashing is calculated at 75% capacity
//Doubles upon each rehash
struct hashtab_s{
	size_t capacity;
	size_t size;
	size_t rehashes;
	size_t collisions;
	keyValuePair** hashTable;
	size_t (*hashFunc)( const void* key );
	bool (*equalsFunc)( const void *key1, const void *key2 );
	void (*printFunc)( const void *key, const void *value );
	void (*deleteFunc)( void *key, void *value );
};

///Specifically delete the keyValuePair pointers with a given method of deletion.
///@param t the table which determines the method of deletion
///@param k the keyValuePair struct pointer in question
///@return a null pointer
///@pre if t->deletFunc is null, assume that the void* point to statically allocated values.
///@post deleted the keyValuePair k.
static keyValuePair* deleteKeyValuePair(HashADT t, keyValuePair* k){
        if(k == NULL){
                return NULL;
        }
        if(t->deleteFunc != NULL){
                t->deleteFunc(k->key, k->value);
        }
        k->key = NULL;
        k->value = NULL;
        free(k);
        k = NULL;
        return k;
}

/// Checks if the hash table is a valid instance.
/// @param t the hash table in question
static void checkForValidHashADT(HashADT t){
	assert(t != NULL);
	assert(t->hashFunc != NULL);
	assert(t->equalsFunc != NULL);
	assert(t->printFunc != NULL);
}

///
/// Create a new hash table instance.  If delete is NULL, destroying the
/// table will NOT free the (key,value) data pairs.
///
/// @param hash The hash function for key data
/// @param equals The equal function for key comparison
/// @param print The print function for key, value pairs is used by dump().
/// @param delete The delete function for key, value pairs is used by destroy().
///
/// @exception Assert fails if it cannot allocate space
///
/// @pre hash, equals and print are valid function pointers.
///
/// @return A newly created table
///
HashADT ht_create(
    size_t (*hash)( const void *key ),
    bool (*equals)( const void *key1, const void *key2 ),
    void (*print)( const void *key, const void *value ),
    void (*delete)( void *key, void *value )
){
	HashADT newTable = (HashADT)malloc(sizeof(struct hashtab_s));
	assert(newTable != NULL);
	newTable->hashTable = calloc(INITIAL_CAPACITY, sizeof(keyValuePair*));
	assert(newTable->hashTable != NULL);
	for(size_t i = 0; i < INITIAL_CAPACITY;i++){
		*(newTable->hashTable + i) = NULL;
	}
	newTable->collisions = 0;
	newTable->rehashes = 0;
	newTable->size = 0;
	newTable->capacity = INITIAL_CAPACITY;
	newTable->hashFunc = hash;
	newTable->equalsFunc = equals;
	newTable->printFunc = print;
	newTable->deleteFunc = delete;
	return newTable;
}




///
/// Destroy the table instance, and call delete function on (key,value) pair.
/// 
/// @param t The table to destroy
/// 
/// @pre t is a valid instance of table.
/// 
/// @post t is not a valid instance of table.
///
void ht_destroy( HashADT t ){
	checkForValidHashADT(t);
	if(t == NULL){
		return;
	}
	for(size_t i = 0; i < t->capacity; i++){
		if( (*(t->hashTable + i)) != NULL ){
			//Auxiliary function for removing key value pairs.
			//Dynamically allocated pointer frees the actual pointer of the table alongside the values it contains.
			deleteKeyValuePair(t, *(t->hashTable + i));
		}
	}
	free(t->hashTable);
	t->hashTable = 0;
	//Free the actual thing.
	free(t);
	t = NULL;
}



///
/// Print information about hash table (size, capacity, collisions, rehashes).
///
/// If contents is true, also print the entire contents of the hash table
/// using the registered print function with each non-null entry.
///
/// @param t The table to display
/// @param contents Do a full dump including the entire table contents
///
/// @pre t is a valid instance of table.
///
void ht_dump( const HashADT t, bool contents ){
	checkForValidHashADT(t);
	fprintf(stdout, "Size: %zu\n", t->size);
	fprintf(stdout, "Capacity: %zu\n", t->capacity);
	fprintf(stdout, "Collisions: %zu\n", t->collisions);
	fprintf(stdout, "Rehashes: %zu\n", t->rehashes);
	if(contents == true){
		for(size_t i = 0; i < t->capacity; i++){
			fprintf(stdout, "%zu: ", i);
			if( *(t->hashTable + i) != NULL ){
				fprintf(stdout, "(");
				t->printFunc( (*(t->hashTable + i))->key , (*(t->hashTable + i))->value  );
				fprintf(stdout, ")");
			}else{
				fprintf(stdout, "null");
			}
			fprintf(stdout, "\n");
		}
	}
}


///
/// Get the value associated with a key from the table.  This function
/// uses the registered hash function to locate the key, and the
/// registered equals function to check for equality.
///
/// @param t The table
/// @param key The key
///
/// @pre The table must have the key, or the function will assert failure
/// @pre has( t, key) is true.
/// @pre t is a valid instance of table, and key is not NULL.
///
/// @return The value associated with the key
///
const void *ht_get( const HashADT t, const void *key ){
	checkForValidHashADT(t);
	//assert(ht_has(t, key) == true);
	size_t index = t->hashFunc(key) % t->capacity;
	//Starts to probe for the key (while the next space is either empty or not equal to the choosen key)
	while( ((*(t->hashTable + index)) == NULL) || (t->equalsFunc((*(t->hashTable + index))->key, key) == false) ){
		index = (index + 1) % t->capacity;
		t->collisions++;
	}
	return (*(t->hashTable + index))->value;
}

///
/// Check if the table has a key.  This function uses the registered hash
/// function to locate the key, and the registered equals function to
/// check for equality.
///
/// @param t The table
/// @param key The key
///
/// @pre t is a valid instance of table, and key is not NULL.
///
/// @return Whether the key exists in the table.
///
bool ht_has( const HashADT t, const void *key ){
	//Has "missing" despite it not existing in here. Why?
	checkForValidHashADT(t);
	//If the key is not null, but in this case it is.
	//assert( (void*)key != NULL );
	size_t index = t->hashFunc(key) % t->capacity;
	//Starts to probe for the key (while the next space is either empty or not equal to the choosen key)
	while( ((*(t->hashTable + index)) != NULL) && (t->equalsFunc((*(t->hashTable + index))->key, key) == false) ){
		
		index = (index + 1) % t->capacity;
		t->collisions++;
		//Loop finished, nothing found.
		if((*(t->hashTable + index)) == NULL){
			return false;
		}
	}
	if((*(t->hashTable + index)) == NULL){
		return false;
	}else{
		return true;
	}
}



///
/// Add a key value pair to the table, or update an existing key's value.
/// This function uses the registered hash function to locate the key,
/// and the registered equals function to check for equality.
///
/// @param t The table
/// @param key The key
/// @param value The value
///
/// @exception Assert fails if it cannot allocate space
///
/// @pre t is a valid instance of table.
///
/// @post if size reached the LOAD_THRESHOLD, table has grown by RESIZE_FACTOR.
///
/// @return The old value associated with the key, if one exists.
///
void *ht_put( HashADT t, const void *key, const void *value ){
	checkForValidHashADT(t);
	float currentLoad = ((float)t->size) / ((float)t->capacity);
	if( currentLoad >= LOAD_THRESHOLD ){
		size_t oldCapacity = t->capacity;
		t->capacity *= RESIZE_FACTOR;
		
		keyValuePair** temp = t->hashTable;
		t->hashTable = calloc(t->capacity, sizeof(keyValuePair*));
		assert(t->hashTable != NULL);
		t->size = 0;
		for(size_t i = 0; i < oldCapacity; i++){
			if(*(temp+i) != NULL){
				int index = t->hashFunc((*(temp+i))->key) % t->capacity;
				while( (*(t->hashTable + index) != NULL) && (t->equalsFunc((*(t->hashTable + index))->key, (*(temp+i))->key) == false) ){
					index = (index+1) % t->capacity;
					t->collisions++;
				}
				*(t->hashTable + index) = malloc(sizeof(keyValuePair));
				assert(*(t->hashTable + index) != NULL);
				(*(t->hashTable + index))->key = (void*)((*(temp+i))->key);
				(*(t->hashTable + index))->value = (void*)((*(temp+i))->value);
				t->size++;
				free(*(temp+i));
				*(temp+i) = NULL;
			}
		}
		free(temp);
		temp = NULL;
		t->rehashes++;//Increase amount of rehashes
	}
	
	size_t index = t->hashFunc(key) % t->capacity;
	//Linear probe for free space or matching key
	while( (*(t->hashTable + index) != NULL) && (t->equalsFunc((*(t->hashTable + index))->key, key) == false) ){
		index = (index+1) % t->capacity;
		t->collisions++;//Increase collisions
	}
	void* returnVal = NULL;
	if(NULL == *(t->hashTable + index)){//Assumes that *(t->hashTable + index) choosen is NULL.
		//Memory in the following line is lost after rehashing, but not before. Why?`
		*(t->hashTable + index) = malloc(sizeof(keyValuePair));//Memory lost here. Exactly 12 allocations are lost.
		assert( *(t->hashTable + index) != NULL );
		(*(t->hashTable + index))->key = (void*)key;
		t->size++;
	}else{
		returnVal = (*(t->hashTable + index))->value;
	}
	//Remove existing key value pair
	(*(t->hashTable + index))->value = (void*)value;
	return returnVal;

}

///
/// Get the collection of keys from the table.  This function allocates
/// space to store the keys, which the caller is responsible for freeing.
///
/// @param t The table
///
/// @exception Assert fails if it cannot allocate space
///
/// @pre t is a valid instance of table.
///
/// @post client is responsible for freeing the returned array.
///
/// @return A dynamic array of keys
///
void **ht_keys( const HashADT t ){
	checkForValidHashADT(t);
	void** keys = calloc(t->capacity, sizeof(void*));
	assert(keys != NULL);
	size_t keysIndex = 0;
	for(size_t i = 0; i < t->capacity; i++){
		if(*(t->hashTable + i) != NULL){
			*(keys + keysIndex) = (*(t->hashTable + i))->key;
			keysIndex++;
		}
	}
	//Will this leak memory?
	void** temp = realloc(keys, (sizeof(void*))*(keysIndex+1) );
	assert(temp != NULL);
	keys = temp;
	return keys;
}

///
/// Get the collection of values from the table.  This function allocates
/// space to store the values, which the caller is responsible for freeing.
///
/// @param t The table
///
/// @exception Assert fails if it cannot allocate space
///
/// @pre t is a valid instance of table.
///
/// @post client is responsible for freeing the returned array.
///
/// @return A dynamic array of values
///
void **ht_values( const HashADT t ){
	checkForValidHashADT(t);
	void** values = calloc(t->capacity, sizeof(void*));
	assert(values != NULL);
	size_t valuesIndex = 0;
	for(size_t i = 0; i < t->capacity; i++){
		if(*(t->hashTable + i) != NULL){
			*(values + valuesIndex) = (*(t->hashTable + i))->value;
			valuesIndex++;
		}
	}
	//Will this leak memory?
	void** temp = realloc(values, (sizeof(void*))*(valuesIndex+1) );
	assert(temp != NULL);
	values = temp;
	return values;
}
