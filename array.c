#include "array.h"

struct arrBody{
	void** arr;
	size_t size;
	size_t capacity;
	void (*freeMember)(void**);
};


array newArray(void (freeMem)(void**) ){
	array a = malloc(sizeof(struct arrBody));
	if(a == NULL){
		fprintf(stderr, "Can't allocate memory.\n");
		return NULL;
	}
	a->size = 0;
	a->capacity = ARRCAP;
	a->arr = calloc(a->capacity, sizeof(void*));
	if(a->arr == NULL){
		fprintf(stderr, "Can't allocate memory.\n");
		return NULL;
	}
	a->freeMember = freeMem;
	return a;
}

void addToArray(array a, void* item){
	if(a->size == a->capacity){
		a->capacity *= 2;

		void** newBody = realloc(a->arr, a->capacity * sizeof(void*) );
		if(newBody == NULL){
			fprintf(stderr, "Can't allocate memory.\n");
			return;
		}
		a->arr = newBody;
	}
	*(a->arr + a->size) = item;
	
	a->size++;
}

void removeItem(array a, size_t i){
	if(i >= getArrLen(a)){
		fprintf(stderr, "Index out of bounds : %lu\n", i);
		return;
	}
	if(a->freeMember != NULL){
		a->freeMember( (a->arr)+i );
	}
	*((a->arr)+i) = NULL;
	for(size_t j = i; j < getArrLen(a)-1;j++){
		*((a->arr)+(j)) = *((a->arr)+(j+1));
		*((a->arr)+(j+1)) = NULL;
	}
	a->size--;

}


void deleteArray(array* a){
	if(a == NULL){
		return;
	}
	if(*a != NULL){
		for(size_t i = 0; i < (*a)->size;i++){
			if((*a)->freeMember != NULL){
				(*a)->freeMember( ((*a)->arr)+i );
			}
		}
		free((*a)->arr);
		(*a)->arr = NULL;
		free(*a);
		*a = NULL;
	}
}

void* getItem(array a, size_t i){
	if(i >= getArrLen(a)){
		fprintf(stderr, "Index out of bounds : %lu\n", i);
		return NULL;
	}
	return *( (a->arr)+i );
}


size_t getArrLen(array a){
	return a->size;
}
