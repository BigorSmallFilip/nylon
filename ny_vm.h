#ifndef __Ny_VM_H__
#define __Ny_VM_H__

#include "ny_utility.h"

enum
{
	Ny_OP_NULL,
	Ny_OP_SET,
	Ny_OP_IADD,
	Ny_OP_ISUB,
	Ny_OP_IMOD,

};
typedef unsigned char Ny_Opcode;

#endif