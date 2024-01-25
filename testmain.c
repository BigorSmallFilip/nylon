#include "ny_utility.h"
#include "ny_state.h"
#include "ny_parse.h"

#include <time.h>



char* load_string_from_file(const char* filename)
{
	FILE* file = fopen(filename, "r");
	if (!file) return NULL;

	int length = 0;
	while (!feof(file))
	{
		fgetc(file);
		length++;
	}

	char* filebuffer = malloc(length + 1);
	if (!filebuffer) goto fail;
	rewind(file);
	int pos = 0;
	while (!feof(file) && pos < length)
	{
		char c = fgetc(file);
		filebuffer[pos] = c;
		pos++;
	}

	filebuffer[pos - 1] = '\0';

fail:
	fclose(file);
	return filebuffer;
}



int main(int argc, char** argv)
{
	Ny_State* state = Ny_CreateState();

	char* testfile = load_string_from_file("test.ny");
	
	Ny_ParseSourceCode(state, testfile);

	free(testfile);
	Ny_PrintAllStateMessages(state);

	Ny_FreeState(state);

	return 0;
}
