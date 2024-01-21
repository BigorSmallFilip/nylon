#ifndef __Ny_UTILITY_H__
#define __Ny_UTILITY_H__

#define _CRT_SECURE_NO_WARNINGS
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
Ny_Hash Ny_HashString(const char* str);



/* Nylon vector */

#define Ny_MIN_VECTOR_CAPACITY 4

typedef struct
{
	void** buffer;
	size_t count;
	size_t capacity;
} Ny_Vector;

/* #define Ny_InitVector (vector) ((vector)->buffer = NULL, (vector)->count = 0, (vector)->capacity = 0) */

Ny_Bool Ny_InitVector(Ny_Vector* vector, size_t capacity);

Ny_Bool Ny_ResizeVector(Ny_Vector* vector, size_t newcapacity);
#define Ny_HalfVectorSize(vector) Ny_ResizeVector((vector), (vector)->capacity >> 1)
#define Ny_DoubleVectorSize(vector) Ny_ResizeVector((vector), (vector)->capacity << 1)

Ny_Bool Ny_PushBackVector(Ny_Vector* vector, void* item);
void* Ny_PopBackVector(Ny_Vector* vector);

#define Ny_VectorBack(vector, type) (vector.count > 0 ? (type)(vector.buffer[vector.count - 1]) : NULL)
#define Ny_VectorPBack(vector, type) (vector->count > 0 ? (type)(vector->buffer[vector->count - 1]) : NULL)



/* Vector of bytes */

typedef struct
{
	void* buffer;
	size_t size;
	size_t capacity;
} Ny_ByteVector;

Ny_Bool Ny_InitByteVector(Ny_ByteVector* vector, size_t capacity);
Ny_Bool Ny_ResizeByteVector(Ny_ByteVector* vector, size_t newcapacity);
#define Ny_HalfByteVectorSize(vector) Ny_ResizeByteVector((vector), (vector)->capacity >> 1)
#define Ny_DoubleByteVectorSize(vector) Ny_ResizeByteVector((vector), (vector)->capacity << 1)

Ny_Bool Ny_PushBackByteVector(Ny_ByteVector* vector, void* data, size_t size);
Ny_Bool Ny_PopBackByteVector(Ny_ByteVector* vector, void** data, size_t size);



typedef struct
{
	size_t length;
	Ny_Hash hash;
	char* str;
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



#ifdef Ny_DEBUG && Ny_DEBUGPRINT
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