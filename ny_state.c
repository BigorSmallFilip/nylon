#include "ny_state.h"

#include <stdarg.h>



Ny_State* Ny_CreateState()
{
	Ny_State* state = Ny_AllocType(Ny_State);
	return state;
}

void Ny_DestroyState(Ny_State* state)
{
	Ny_Free(state);
}



void Ny_PushStateMessage(
	Ny_State* state,
	Ny_StateMessageType type,
	Ny_SourceUnit* sourceunit,
	int linenum,
	int charpos,
	const char* const format, ...)
{
	Ny_Assert(state);
	Ny_Assert(type >= 0 && type <= Ny_MSGT_RUNTIMEERROR); /* Validate type param */

	Ny_StateMessage* msg = Ny_AllocType(Ny_StateMessage);
	msg->type = type;
	msg->linenum = linenum;
	msg->charpos = charpos;
	
	char str[256];
	va_list args;
	va_start(args, format);
	vsprintf_s(&str, 255, format, args);
	va_end(args);
}



void Ny_PrintStateMessage(Ny_StateMessage* msg)
{
}
