#ifndef __Ny_BYTECODE_H__
#define __Ny_BYTECODE_H__

#include "ny_utility.h"
#include "ny_state.h"



enum
{
	Ny_I_LOAD_CONST,
	Ny_I_LOAD_LOCAL,
	Ny_I_LOAD_GLOBAL,
	Ny_I_STORE_LOCAL,
	Ny_I_STORE_GLOBAL,

	Ny_NUM_INSTRUCTIONS,
	Ny_I_NULL
};
typedef int Ny_Instruction;
extern const char* ny_instruction_strings[Ny_NUM_INSTRUCTIONS];



typedef struct
{
	char* name;
	Ny_ByteVector bytecode;
} Ny_BytecodeBlock;



#endif