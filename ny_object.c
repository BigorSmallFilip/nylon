#include "ny_object.h"







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
