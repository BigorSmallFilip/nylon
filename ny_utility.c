#include "ny_utility.h"



Ny_Bool Ny_InitVector(Ny_Vector* vector, size_t capacity)
{
	vector->buffer = Ny_Calloc(capacity, sizeof(void*));
	vector->capacity = capacity;
	vector->count = 0;
	return vector;
}

Ny_Bool Ny_ResizeVector(Ny_Vector* vector, size_t newcapacity)
{
	if (newcapacity < vector->count)
	{
		return Ny_FALSE;
	}
	if (newcapacity < Ny_MIN_VECTOR_CAPACITY) newcapacity = Ny_MIN_VECTOR_CAPACITY;
	vector->buffer = Ny_Realloc(vector->buffer, newcapacity * sizeof(void*));
	vector->capacity = newcapacity;
	return Ny_TRUE;
}

Ny_Bool Ny_PushBackVector(Ny_Vector* vector, void* item)
{
	if (vector->count + 1 > vector->capacity)
	{
		if (!Ny_DoubleVectorSize(vector)) return Ny_FALSE;
	}
	vector->buffer[vector->count] = item;
	vector->count++;
	return Ny_TRUE;
}

void* Ny_PopBackVector(Ny_Vector* vector)
{
	if (vector->count <= 0) return NULL;
	void* item = vector->buffer[vector->count - 1];
	vector->count--;
	if (vector->count * 2 <= vector->capacity)
	{
		if (!Ny_HalfVectorSize(vector)) return NULL;
	}
	return item;
}



Ny_Bool Ny_InitByteVector(Ny_ByteVector* vector, size_t capacity)
{
	vector->buffer = Ny_Calloc(capacity, sizeof(uint8_t));
	vector->capacity = capacity;
	vector->size = 0;
	return vector;
}

Ny_Bool Ny_ResizeByteVector(Ny_ByteVector* vector, size_t newcapacity)
{
	if (newcapacity < vector->size)
	{
		return Ny_FALSE;
	}
	if (newcapacity < Ny_MIN_VECTOR_CAPACITY) newcapacity = Ny_MIN_VECTOR_CAPACITY;
	vector->buffer = Ny_Realloc(vector->buffer, newcapacity * sizeof(uint8_t));
	vector->capacity = newcapacity;
	return Ny_TRUE;
}

Ny_Bool Ny_PushBackByteVector(Ny_ByteVector* vector, void* data, size_t size)
{
	if (vector->size + size > vector->capacity)
	{
		if (!Ny_DoubleVectorSize(vector)) return Ny_FALSE;
	}
	memcpy((size_t)vector->buffer + vector->size, data, size);
	vector->size += size;
	return Ny_TRUE;
}

Ny_Bool Ny_PopBackByteVector(Ny_ByteVector* vector, void** data, size_t size)
{
	if (vector->size < size) return Ny_FALSE;
	memcpy(data, (size_t)vector->buffer + vector->size - size, size);
	vector->size -= size;
	if (vector->size << 1 <= vector->capacity)
	{
		if (!Ny_HalfVectorSize(vector)) return Ny_FALSE;
	}
	return Ny_TRUE;
}



void Ny_PushFrontList(
	Ny_List* list,
	Ny_ListNode* node
)
{
	Ny_Assert(list);
	Ny_Assert(node);
	if (list->begin == NULL)
	{
		node->prev = NULL;
		node->next = NULL;
		list->begin = node;
		list->end = node;
	} else
	{
		node->prev = NULL;
		node->next = list->begin;
		list->begin->prev = node;
		list->begin = node;
	}
	list->count++;
}

void Ny_PushBackList(
	Ny_List* list,
	Ny_ListNode* node
)
{
	Ny_Assert(list);
	Ny_Assert(node);
	if (list->begin == NULL)
	{
		node->prev = NULL;
		node->next = NULL;
		list->begin = node;
		list->end = node;
	} else
	{
		node->prev = list->end;
		node->next = NULL;
		list->end->next = node;
		list->end = node;
	}
	list->count++;
}

void* Ny_PopFrontList(
	Ny_List* list
)
{
	Ny_ListNode* node = list->begin;
	Ny_UnlinkFromList(list, node);
	return node;
}

void* Ny_PopBackList(
	Ny_List* list
)
{
	Ny_ListNode* node = list->end;
	Ny_UnlinkFromList(list, node);
	return node;
}

void Ny_UnlinkFromList(
	Ny_List* list,
	Ny_ListNode* node
)
{
	if (node->prev)
		node->prev->next = node->next;
	else
		list->begin = node->next;

	if (node->next)
		node->next->prev = node->prev;
	else
		list->end = node->prev;

	node->prev = NULL;
	node->next = NULL;
	list->count--;
}

void Ny_ClearList(
	Ny_List* list,
	void(*destroy_func)(void*)
)
{
	if (!list) return;
	if (list->count <= 0) return;
	if (list->begin == NULL) return;
	if (list->end == NULL) return;

	Ny_ListNode* iterator = list->begin;
	while (iterator)
	{
		Ny_ListNode* delblock = iterator;
		iterator = iterator->next;
		if (destroy_func)
			destroy_func(delblock);
		else
			free(delblock);
	}
	list->begin = NULL;
	list->end = NULL;
	list->count = 0;
}



char* Ny_CopyCutString(const char* srcstring, size_t start, size_t length)
{
	char* str = Ny_Malloc(length + 1); /* Plus 1 to include null terminator */
	for (int i = 0; i < length; i++)
		str[i] = srcstring[i + start];
	str[length] = '\0';
	return str;
}
