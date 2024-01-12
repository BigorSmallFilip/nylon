#include "ny_assembly.h"

#include "ny_parse.h"

Ny_Instruction get_string_instruction(const char* str)
{
	for (int i = 0; i < Ny_NUM_INSTRUCTIONS; i++)
	{
		if (strcmp(str, ny_instruction_strings[i]) == 0)
			return (Ny_Instruction)i;
	}
	return -1;
}

static Ny_BytecodeBlock* assemble_assembly(Ny_ParserState* parser)
{
	Ny_BytecodeBlock* bytecode = Ny_AllocType(Ny_BytecodeBlock);
	Ny_InitVector(&bytecode->bytecode, Ny_STD_VECTOR_CAPACITY);

	int i = 0;
	Ny_Token* curtoken = NULL;
	while (i < parser->tokens.count)
	{
		curtoken = (Ny_Token*)parser->tokens.buffer[i];
		if (curtoken->type != Ny_TT_IDENTIFIER)
		{
			Ny_PushStateMessage(parser->main_state, Ny_MSGT_SYNTAXERROR, curtoken->linenum, "ass", "Expected an instruction");
			goto fail;
		}

		Ny_Instruction ins = get_string_instruction(curtoken->string);
		if (ins >= 0)
		{
			Ny_PushBackByteVector(&bytecode->bytecode, &ins, sizeof(Ny_Instruction));
			printf("Ins: %s\n", ny_instruction_strings[ins]);
		} else
		{
			Ny_PushStateMessage(parser->main_state, Ny_MSGT_SYNTAXERROR, curtoken->linenum, "ass", "Invalid instruction %s", curtoken->string);
			goto fail;
		}
		
		i++;
	}

	return bytecode;

fail:
	printf("FREE THINGS!\n");
	return NULL;
}

Ny_BytecodeBlock* Ny_CompileAssembly(Ny_State* state, const char* assembly)
{
	printf("Assembling source code: {\n%s\n}\n", assembly);

	Ny_ParserState parser = { 0 };
	parser.main_state = state;
	Ny_TokenizeSourceCode(&parser, assembly);

	Ny_DebugPrint("Tokenization process finished\n");
	for (int i = 0; i < parser.tokens.count; i++)
	{
		Ny_Token* token = parser.tokens.buffer[i];
		Ny_PrintToken(token);
		putchar(' ');
	}
	putchar('\n');

	Ny_BytecodeBlock* bytecode = assemble_assembly(&parser);
	return bytecode;
}