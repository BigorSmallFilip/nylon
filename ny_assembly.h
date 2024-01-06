#ifndef __Ny_ASSEMBLY_H__
#define __Ny_ASSEMBLY_H__

#include "ny_utility.h"
#include "ny_state.h"
#include "ny_bytecode.h"

Ny_BytecodeBlock* Ny_CompileAssembly(Ny_State* state, const char* assembly);

#endif