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

#define ArrayList_push(arr, val) {												\
	if (!arr) {																	\
		ArrayListHeader *header = malloc(										\
			sizeof(ArrayListHeader)												\
			+ sizeof(*(arr))													\
			* ArrayListInitCap													\
		);																		\
		if (!header); /* TODO: handle alloca failure */							\
		header->cap = ArrayListInitCap;											\
		header->len = 0;														\
		(arr) = (void*)(header + 1);											\
	}																			\
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

#endif // ARRAYLIST_H
