#include "ny_utility.h"
#include "ny_state.h"
#include "ny_parse.h"
#include "ny_object.h"

int main(int argc, char** argv)
{
	Ny_DebugPrint("Hello, world!\n");
	
	Ny_State* state = Ny_CreateState();

	char* sourcecode = Ny_LoadTextFile("test.ny");
	Ny_ParseSourceCode(state, sourcecode);
	Ny_PrintAllStateMessages(state);

	Ny_DestroyState(state);

	return 0;
}
