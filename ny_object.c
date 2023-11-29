#include "ny_object.h"



Ny_Bool Ny_OpAddInt(Ny_State* state, Ny_Object* object, Ny_Int n)
{
	if (object->type == Ny_OT_FLOAT)
		object->t_float += (Ny_Float)n;
	else if (object->type == Ny_OT_INT)
		object->t_int += n;
	else if (object->type == Ny_OT_STRING)
	{
		/* Push to da string */
	}
	else
	{
		/* Push error to state */
		return Ny_FALSE;
	}
	return Ny_TRUE;
}
Ny_Bool Ny_OpAddFloat(Ny_State* state, Ny_Object* object, Ny_Float n)
{
	if (object->type == Ny_OT_FLOAT)
		object->t_float += n;
	else if (object->type == Ny_OT_INT)
		object->t_int += (Ny_Int)n;
	else if (object->type == Ny_OT_STRING)
	{
		/* Push to da string */
	}
	else
	{
		/* Push error to state */
		return Ny_FALSE;
	}
	return Ny_TRUE;
}



void Ny_PrintObject(const Ny_Object* obj)
{
	if (!obj) return;
	switch (obj->type)
	{
	case Ny_OT_BOOL:
		printf(obj->t_bool ? "true" : "false");
		break;
	case Ny_OT_INT:
		printf("%lld", obj->t_int);
		break;
	case Ny_OT_FLOAT:
		printf("%f", obj->t_float);
		break;
	default:
		exit(-1);
		break;
	}
}

