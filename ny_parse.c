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

void Ny_FreeExprNode(Ny_ExprNode* node)
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

void Ny_FreeExpression(Ny_Expression* expr)
{
	if (!expr) return;
	for (int i = 0; i < (int)expr->nodes.count; i++)
	{
		Ny_FreeExprNode(expr->nodes.buffer[i]);
	}
	Ny_Free(expr->nodes.buffer);
	Ny_Free(expr);
}

void Ny_FreeCodeBlock(Ny_CodeBlock* block)
{
	if (!block) return;
	printf("Unimplemented\n");
}



/* ================ Parsing tokens to build the AST ================ */



/**
 * @brief Gets the token at parser->curtoken and then increments parser->curtoken. Note that the only index increments *after* getting the token!
 * @param parser Parser state
 * @return The token at parser->curtoken before it is incremented.
*/
static Ny_Token* get_next_token(Ny_ParserState* parser)
{
	if (parser->curtoken >= parser->tokens.count) return NULL;
	return parser->tokens.buffer[parser->curtoken++];
}



static Ny_Statement* parse_statement(Ny_ParserState* parser)
{
	get_next_token(parser);
	return NULL;
}

static Ny_CodeBlock* parse_codeblock(Ny_ParserState* parser, int indentlevel)
{
	Ny_CodeBlock* block = Ny_AllocType(Ny_CodeBlock);
	if (!block) return NULL;
	

	while (parser->curtoken < parser->tokens.count)
	{
		Ny_Statement* stmt = parse_statement(parser);

	}
	return block;
}



#include <time.h>

Ny_AST* Ny_ParseSourceCode(Ny_State* state, const char* sourcecode)
{
	Ny_ParserState parser = { 0 };
	parser.main_state = state;
	parser.tabsize = 4;
	parser.sourcecode_filename = "gruplin";

	clock_t lexer_start = clock();
	if (!Ny_TokenizeSourceCode(&parser, sourcecode)) return NULL;
	clock_t lexer_time = clock() - lexer_start;

	//Ny_PrintSourceCodeTokens(&parser);
	
	clock_t parser_start = clock();
	Ny_CodeBlock* globalscope_block = parse_codeblock(&parser, 0);
	clock_t parser_time = clock() - parser_start;
	
	if (!globalscope_block) goto fail_0;
	Ny_AST* ast = Ny_AllocType(Ny_AST);
	if (!ast) goto fail_1;
	ast->globalscope_block = globalscope_block;

	printf("Lexing took: %dms\n", lexer_time);
	printf("Parsing took: %dms\n\n", parser_time);

	return ast;


fail_1:
	Ny_FreeCodeBlock(globalscope_block);
fail_0:
	for (int i = 0; i < (int)parser.tokens.count; i++)
	{
		Ny_FreeToken(&parser.tokens.buffer[i]);
	}
	Ny_Free(parser.tokens.buffer);
	return NULL;
}
