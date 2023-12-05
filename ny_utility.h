#ifndef __Ny_UTILITY_H__
#define __Ny_UTILITY_H__

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>



#ifdef _DEBUG
#define Ny_DEBUG
#endif

#ifdef Ny_DEBUG
#define Ny_Assert(expr) assert(expr)
#else
#define Ny_Assert(expr) void
#endif



#define Ny_Bool _Bool
#define Ny_FALSE false
#define Ny_TRUE true

#ifndef Ny_USE_32BIT_NUMBERS
typedef double Ny_Float;
typedef int64_t Ny_Int;
#define Ny_StringToFloat strtod
#define Ny_StringToInt strtol
#else
typedef float Ny_Float;
typedef int32_t Ny_Int;
#define Ny_StringToFloat strtof
#define Ny_StringToInt strtol
#endif



#define Ny_Malloc(size) malloc(size)
#define Ny_Calloc(count, size) calloc(count, size)
#define Ny_Realloc(block, size) realloc(block, size)
#define Ny_AllocType(type) calloc(1, sizeof(type))
#define Ny_Free(block) free(block)



#define Ny_Stringify2(str) #str
#define Ny_Stringify(str) Ny_Stringify2(str)

#define Ny_CharInvalid(c) ((char)c < 0)



typedef unsigned int Ny_Hash;



/* Nylon vector */

typedef struct
{
	void** buffer;
	size_t count;
	size_t capacity;
} Ny_Vector;

/* #define Ny_InitVector (vector) ((vector)->buffer = NULL, (vector)->count = 0, (vector)->capacity = 0) */

Ny_Bool Ny_InitVector(Ny_Vector* vector, size_t capacity);

#define Ny_STD_VECTOR_CAPACITY 4
#define Ny_MIN_VECTOR_CAPACITY 4
Ny_Bool Ny_ResizeVector(Ny_Vector* vector, size_t newcapacity);

#define Ny_HalfVectorSize(vector) Ny_ResizeVector((vector), (vector)->capacity >> 1)
#define Ny_DoubleVectorSize(vector) Ny_ResizeVector((vector), (vector)->capacity << 1)

Ny_Bool Ny_PushBackVector(Ny_Vector* vector, void* item);
void* Ny_PopBackVector(Ny_Vector* vector);

#define Ny_VectorBack(vector, type) (vector.count > 0 ? (type)(vector.buffer[vector.count - 1]) : NULL)
#define Ny_VectorPBack(vector, type) (vector->count > 0 ? (type)(vector->buffer[vector->count - 1]) : NULL)



/* Nylon linked list */

typedef struct Ny_ListNode
{
	struct Ny_ListNode* prev;
	struct Ny_ListNode* next;
} Ny_ListNode;

typedef struct
{
	Ny_ListNode* begin;
	Ny_ListNode* end;
	size_t count;
} Ny_List;

/*
** @brief Pushes an element onto the beginning of a list.
** @param list The list to push the element onto.
** @param node Pointer to the element to push.
*/
void Ny_PushFrontList(
	Ny_List* list,
	Ny_ListNode* node
);

/*
** @brief Pushes an element onto the end of a list.
** @param list The list to push the element onto.
** @param node Pointer to the element to push.
*/
void Ny_PushBackList(
	Ny_List* list,
	Ny_ListNode* node
);

/*
** @brief Removes the element at the beginning of the list and returns a pointer to it.
** @param list The list to get the element from.
** @return A pointer to the element or NULL if the list was empty. Make sure to know
** the type of the element returned to cast it from the void* returned.
*/
void* Ny_PopFrontList(
	Ny_List* list
);

/*
** @brief Removes the element at the end of the list and returns a pointer to it.
** @param list The list to get the element from.
** @return A pointer to the element or NULL if the list was empty. Make sure to know
** the type of the element returned to cast it from the void* returned.
*/
void* Ny_PopBackList(
	Ny_List* list
);

/*
** @brief Removes a node from a list and links the surrounding elements together.
** @param list The list to unlink from.
** @param node The node to unlink from the list.
*/
void Ny_UnlinkFromList(
	Ny_List* list,
	Ny_ListNode* node
);

/*
** @brief Removes all elements from a list and calling the destroy_func on every element.
** This does not free the list itself, only empties it.
** @param list Pointer to the list to empty.
** @param destroy_func Pointer to a destructor function, or NULL to use Lnn_Free().
*/
void Ny_ClearList(
	Ny_List* list,
	void(*destroy_func)(void*)
);



typedef struct
{
	size_t length;
	char* str;
	Ny_Hash hash;
} Ny_String;

/*
** @brief Copies part of a string and returns a new string of that.
** @param srcstring String to copy from.
** @param start Where in the srcstring to start.
** @param length The length of the part to copy.
** @return Pointer to the new string, remember to free!
*/
char* Ny_CopyCutString(
	const char* srcstring,
	size_t start,
	size_t length
);



char* Ny_LoadTextFile(const char* filename);



#ifdef Ny_DEBUG & Ny_DEBUGPRINT
#define Ny_DebugPrint printf
#else
#define Ny_DebugPrint() void
#endif

#define Ny_CONSOLE_ANSI_COLOR
#ifdef Ny_CONSOLE_ANSI_COLOR
#define Ny_ANSICOLOR_ERROR "\x1B[0;91m"
#define Ny_ANSICOLOR_WARNING "\x1B[0;93m"
#define Ny_ANSICOLOR_RESET "\x1B[0m"
#else
#define Ny_ANSICOLOR_ERROR
#define Ny_ANSICOLOR_WARNING
#define Ny_ANSICOLOR_RESET
#endif

#endif