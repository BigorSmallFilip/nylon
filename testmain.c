#include "ny_utility.h"
#include "ny_state.h"
#include "ny_parse.h"
#include "ny_object.h"

int main(int argc, char** argv)
{
	Ny_DebugPrint("Hello, world!\n");
	
	Ny_State* state = Ny_CreateState();

	Ny_ParseSourceCode(state, "1 + 2 * 3 / 4 - 5");

	Ny_DestroyState(state);

	return 0;
}
