#ifndef __Ny_OBJECT_H__
#define __Ny_OBJECT_H__

#include "ny_utility.h"
#include "ny_state.h"



enum
{
	Ny_OT_BOOL,
	Ny_OT_INT,
	Ny_OT_FLOAT,
	Ny_OT_STRING,

	Ny_OT_ARRAY,
	Ny_OT_LIST,
	Ny_OT_TUPLE,
	Ny_OT_DICT,

	Ny_OT_NULL = -1
};
typedef char Ny_ObjectType;

enum
{
	Ny_OF_CONST = (1 << 7) /* Flag for if an object is constant, highest bit of object's type value */
};
typedef unsigned char Ny_ObjectFlags;

typedef struct Ny_Object
{
	Ny_ObjectType type;
	Ny_ObjectFlags flags;
	union
	{
		Ny_Bool t_bool;
		Ny_Int t_int;
		Ny_Float t_float;
		Ny_String t_string;
	};
} Ny_Object;



Ny_Bool Ny_OpAddInt(Ny_State* state, Ny_Object* object, Ny_Int n);
Ny_Bool Ny_OpAddFloat(Ny_State* state, Ny_Object* object, Ny_Float n);



#define Ny_IntObject(n) { Ny_OT_INT, 0, .t_int = { n } }

void Ny_PrintObject(const Ny_Object* obj);

#endif