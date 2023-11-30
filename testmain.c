#include "ny_utility.h"
#include "ny_state.h"
#include "ny_parse.h"
#include "ny_object.h"

int main(int argc, char** argv)
{
	Ny_DebugPrint("Hello, world!\n");
	
	Ny_State* state = Ny_CreateState();

	Ny_ParseSourceCode(state, "2 = a + b");
	Ny_PrintAllStateMessages(state);

	Ny_PushStateMessage(state, Ny_MSGT_GENERAL, 1, "brigh", "THIS IS really BAD");
	Ny_PushStateMessage(state, Ny_MSGT_WARNING, 1, "brigh", "THIS IS really BAD");
	Ny_PushStateMessage(state, Ny_MSGT_SYNTAXERROR, 1, "brigh", "THIS IS really BAD");
	Ny_PushStateMessage(state, Ny_MSGT_RUNTIMEERROR, 1, "brigh", "THIS IS really BAD");
	Ny_PrintAllStateMessages(state);

	Ny_DestroyState(state);

	return 0;
}
