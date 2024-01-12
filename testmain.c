#include "ny_utility.h"
#include "ny_state.h"
#include "ny_parse.h"
#include "ny_object.h"
#include "ny_assembly.h"

int main(int argc, char** argv)
{
	Ny_State* state = Ny_CreateState();

	//Ny_ParseSourceCode(state, "2 = a + b");
	//Ny_PrintAllStateMessages(state);

	Ny_CompileAssembly(state, "LOAD_CONST");
	Ny_PrintAllStateMessages(state);

	Ny_DestroyState(state);

	return 0;
}
