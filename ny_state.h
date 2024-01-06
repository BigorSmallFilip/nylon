#ifndef __Ny_STATE_H__
#define __Ny_STATE_H__

#include "ny_utility.h"

typedef struct
{
	Ny_Vector stack;
	Ny_Vector messages;
} Ny_State;

Ny_State* Ny_CreateState();

void Ny_DestroyState(Ny_State* state);



enum
{
	Ny_MSGT_GENERAL,
	Ny_MSGT_WARNING,
	Ny_MSGT_SYNTAXERROR,
	Ny_MSGT_RUNTIMEERROR
};
typedef unsigned char Ny_StateMessageType;

typedef struct
{
	Ny_StateMessageType type;
	int linenum;
	char* filename;
	char* msg;
} Ny_StateMessage;

void Ny_PushStateMessage(
	Ny_State* state,
	Ny_StateMessageType type,
	int linenum,
	char* filename,
	const char* const format,
	...
);

void Ny_PrintStateMessage(
	Ny_StateMessage* msg
);

void Ny_PrintAllStateMessages(
	Ny_State* state
);

#endif