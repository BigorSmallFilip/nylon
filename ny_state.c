#include "ny_state.h"

#include <stdarg.h>



Ny_State* Ny_CreateState()
{
	Ny_State* state = Ny_AllocType(Ny_State);
	Ny_InitVector(&state->messages, Ny_STD_VECTOR_CAPACITY);
	return state;
}

void Ny_DestroyState(Ny_State* state)
{
	Ny_Free(state);
}



void Ny_PushStateMessage(
	Ny_State* state,
	Ny_StateMessageType type,
	int linenum,
	char* filename,
	const char* const format, ...)
{
	Ny_Assert(state);
	Ny_Assert(type >= 0 && type <= Ny_MSGT_RUNTIMEERROR); /* Validate type param */

	Ny_StateMessage* msg = Ny_AllocType(Ny_StateMessage);
	msg->type = type;
	msg->linenum = linenum;
	if (filename)
		msg->filename = _strdup(filename);
	else
		msg->filename = NULL;
	
	msg->msg = Ny_Malloc(256);
	va_list args;
	va_start(args, format);
	vsprintf_s(msg->msg, 256, format, args);
	va_end(args);

	Ny_PushBackVector(&state->messages, msg);
}

void Ny_PrintStateMessage(Ny_StateMessage* msg)
{
	if (!msg) return;
	if (msg->type < 0 || msg->type > Ny_MSGT_RUNTIMEERROR) return;
	switch (msg->type)
	{
	case Ny_MSGT_GENERAL:
		printf("Message ");
		break;
	case Ny_MSGT_WARNING:
		printf(Ny_ANSICOLOR_WARNING "Warning ");
		break;
	case Ny_MSGT_SYNTAXERROR:
		printf(Ny_ANSICOLOR_ERROR "Syntax error ");
		break;
	case Ny_MSGT_RUNTIMEERROR:
		printf(Ny_ANSICOLOR_ERROR "Runtime error ");
		break;
	}
	if (msg->filename)
		printf("in \"%s\" ", msg->filename);
	if (msg->linenum > 0)
		printf("on line %i", msg->linenum);
	printf(":\n%s\n" Ny_ANSICOLOR_RESET, msg->msg);
}

void Ny_PrintAllStateMessages(Ny_State* state)
{
	printf("--------State messages--------\n");
	if (state->messages.count == 0)
	{
		printf(" No messages\n");
	} else
	{
		Ny_StateMessage* msg;
		for (int i = 0; i < state->messages.count; i++)
		{
			Ny_PrintStateMessage(state->messages.buffer[i]);
		}
	}
	printf("------------------------------\n");
	Ny_ClearStateMessages(state);
}



void Ny_ClearStateMessages(Ny_State* state)
{
	for (int i = 0; i < state->messages.count; i++)
	{
		Ny_StateMessage* msg = state->messages.buffer[i];
		Ny_Free(msg->filename);
		Ny_Free(msg->msg);
		Ny_Free(msg);
	}
	Ny_Free(state->messages.buffer);
	Ny_InitVector(&state->messages, Ny_STD_VECTOR_CAPACITY);
}
