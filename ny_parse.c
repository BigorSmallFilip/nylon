#include "ny_parse.h"



void Ny_PrintExprNode(const Ny_ExprNode* node)
{
	if (!node) return;
	if (node->type < 0 && node->type >= Ny_NUM_EXPRNODETYPES)
	{
		printf("(invalid expression node)");
		return;
	}

	switch (node->type)
	{
	case Ny_ET_OPERATOR: printf("%s", ny_operatorid_strings[node->op]); break;
	case Ny_ET_INTLITERAL: printf("%lld", node->numint); break;
	case Ny_ET_FLOATLITERAL: printf("%f", node->numfloat); break;
	case Ny_ET_VARIABLE: printf("%s", node->variable); break;
	default:
		printf("uniplemented");
		break;
	}
}

void Ny_DestroyExprNode(Ny_ExprNode* node)
{
	if (!node) return;
	switch (node->type)
	{
	case Ny_ET_STRINGLITERAL:
		Ny_Free(node->string);
		break;
	case Ny_ET_VARIABLE:
		Ny_Free(node->variable);
		break;

	default:
		break;
	}
}



static void print_indent(int depth)
{
	for (int i = 0; i < depth - 1; i++)
	{
		printf(" | ");
	}
	if (depth > 0)
		printf(" |=");
}

static void recursive_print_exprnode(const Ny_ExprNode* node, int depth)
{
	if (node->type == Ny_ET_OPERATOR)
	{
		print_indent(depth);
		Ny_PrintExprNode(node);
		putchar('\n');

		if (node->left)
			recursive_print_exprnode(node->left, depth + 1);
		if (node->right)
			recursive_print_exprnode(node->right, depth + 1);
	} else
	{
		print_indent(depth);
		Ny_PrintExprNode(node);
		putchar('\n');
	}
}

void Ny_PrintExpressionTree(const Ny_Expression* expr)
{
	recursive_print_exprnode(expr->topnode, 0);
}

void Ny_DestroyExpression(Ny_Expression* expr)
{
	if (!expr) return;
	for (int i = 0; i < (int)expr->nodes.count; i++)
	{
		Ny_DestroyExprNode(expr->nodes.buffer[i]);
	}
	Ny_Free(expr->nodes.buffer);
	Ny_Free(expr);
}





Ny_AST* Ny_ParseSourceCode(Ny_State* state, const char* sourcecode)
{
	Ny_DebugPrint("Parsing source code:\n%s\n", sourcecode);

	Ny_ParserState parser = { 0 };
	parser.main_state = state;
	Ny_TokenizeSourceCode(&parser, sourcecode);

	Ny_DebugPrint("Tokenization process finished\n");
	for (int i = 0; i < (int)parser.tokens.count; i++)
	{
		Ny_Token* token = parser.tokens.buffer[i];
		Ny_PrintToken(token);
		putchar(' ');
	}
	putchar('\n');

	return NULL;
}
