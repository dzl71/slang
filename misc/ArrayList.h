#include <stddef.h>
#include <stdlib.h>

#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#define ArrayListInitCap 64
#define ArrayListGrowth 2

typedef struct ArrayListHeader {
	size_t cap;
	size_t len;
} ArrayListHeader;


#define ArrayList_init(arr) {													\
	ArrayListHeader *header = malloc(											\
		sizeof(ArrayListHeader)													\
		+ sizeof(*(arr))														\
		* ArrayListInitCap														\
	);																			\
	if (!header) { /* TODO: handle alloc failure */								\
		header->cap = ArrayListInitCap;											\
		header->len = 0;														\
		(arr) = (void*)(header + 1);											\
	}																			\
}

#define ArrayList_push(arr, val) {												\
	ArrayListHeader *header = (ArrayListHeader*)(arr) - 1;						\
	if (header->len == header->cap) {											\
		header = realloc(														\
			header,																\
			sizeof(ArrayListHeader)												\
			+ header->cap														\
			* sizeof(*(arr))													\
			* ArrayListGrowth													\
		);																		\
		if (!header); /* TODO: handle realloc failure */						\
		header->cap *= ArrayListGrowth;											\
		(arr) = (void*)(header + 1);											\
	}																			\
	(arr)[header->len++] = (val);												\
}

#define ArrayList_free(arr) free((ArrayListHeader*)(arr) - 1)

#define ArrayList_len(arr) ((ArrayListHeader*)(arr) - 1)->len

/* some experiment
#define ArrayList(type, prefix)													\
\
typedef type * prefix##ArrayList;												\
\
void *prefix##ArrayList_init() {												\
	ArrayListHeader *header = (ArrayListHeader *)malloc(						\
		sizeof(ArrayListHeader)													\
		+ sizeof(type)															\
		* ArrayListInitCap														\
	);																			\
	if (!header) return NULL;													\
	header->cap = ArrayListInitCap;												\
	header->len = 0;															\
	return (void*)(header + 1);													\
}																				\
\
void *prefix##ArrayList_push(type *arr, type val) {								\
	if (!arr) arr = ArrayList_init();											\
	ArrayListHeader *header = (ArrayListHeader*)(arr) - 1;						\
	if (header->len == header->cap) {											\
		ArrayListHeader *reallocHeader = realloc(								\
			header,																\
			sizeof(ArrayListHeader)												\
			+ header->cap														\
			* sizeof(*(arr))													\
			* ArrayListGrowth													\
		);																		\
		if (!reallocHeader) return NULL;										\
		header = reallocHeader;													\
		header->cap *= ArrayListGrowth;											\
		(arr) = (void*)(header + 1);											\
	}																			\
	(arr)[header->len++] = (val);												\
	return header;																\
}
*/

#endif // ARRAYLIST_H
