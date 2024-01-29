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



#define syntax_error(format, ...)    \
	Ny_PushStateMessage(         \
	parser->main_state,          \
	Ny_MSGT_SYNTAXERROR,         \
	parser->linenum,             \
	parser->sourcecode_filename, \
	format,                      \
	__VA_ARGS__);                \
	parser->errorcount++



/*
** @brief Gets the token at parser->curtoken
** @param parser Parser state
** @return The token at parser->curtoken
*/
static Ny_Token* get_current_token(const Ny_ParserState* parser)
{
	if (parser->curtoken >= parser->tokens.count) return NULL;
	return parser->tokens.buffer[parser->curtoken];
}
/*
** @brief Increments parser->curtoken and gets the token at that index
** @param parser Parser state
** @return The token at parser->curtoken + 1
*/
static Ny_Token* get_next_token(Ny_ParserState* parser)
{
	parser->curtoken++;
	if (parser->curtoken >= parser->tokens.count) return NULL;
	return parser->tokens.buffer[parser->curtoken];
}

/**
 * @brief Attemps to find the next token on indentlevel or lower. This is for error recovery.
 * parser->curtoken will be set to this token if it is found. It will be set to parser->tokens.count if it doesn't find any
 * @param parser Parser state
 * @param indentlevel The indent level to exit to
*/
static void find_next_token_on_indentlevel(
	Ny_ParserState* parser,
	int indentlevel
)
{
	//printf("Attempting recovery from "); Ny_PrintToken(parser->tokens.buffer[parser->curtoken]);
	while (++parser->curtoken < parser->tokens.count)
	{
		Ny_Token* token = parser->tokens.buffer[parser->curtoken];
		if (token->indentlevel >= 0 && token->indentlevel <= indentlevel)
		{
			//printf(" to "); Ny_PrintToken(parser->tokens.buffer[parser->curtoken]); putchar('\n');
			return;
		}
	}
	//printf(" to EOF\n");
}



static Ny_CodeBlock* parse_codeblock(Ny_ParserState* parser, int parent_indentlevel);



/* ================ Expression parsing ================ */

static Ny_Expression* parse_expression(Ny_ParserState* parser)
{
	Ny_Token* token = get_current_token(parser);
	//printf("Parsing expression starting with '"); Ny_PrintToken(token); printf("'\n");

	parser->curtoken += 3;
	return NULL;
}



static Ny_Statement* parse_if_statement(
	Ny_ParserState* parser
)
{
	Ny_Token* iftoken = get_current_token(parser);
	Ny_Token* conditiontoken = get_next_token(parser);
	if (!conditiontoken)
	{
		syntax_error("If statement needs a condition expression");
		return NULL;
	}
	Ny_Expression* condition = parse_expression(parser);

	Ny_CodeBlock* trueblock = NULL;
	Ny_CodeBlock* falseblock = NULL;

	Ny_Token* trueblock_token = get_current_token(parser);
	if (trueblock_token->indentlevel == -1 || trueblock_token->indentlevel > iftoken->indentlevel)
	{
		trueblock = parse_codeblock(parser, iftoken->indentlevel);
		Ny_Token* elsetoken = get_current_token(parser);
		if (elsetoken && elsetoken->keywordid == Ny_KW_ELSE)
		{
			
		}
	}
	
	

	Ny_Statement* stmt = Ny_AllocType(Ny_Statement);
	stmt->type = Ny_ST_IF;
	stmt->linenum = iftoken->linenum;
	stmt->stmt_if.condition = condition;
	stmt->stmt_if.block_ontrue = trueblock;
	stmt->stmt_if.block_onfalse = falseblock;
	return stmt;
}



/**
 * @brief Parses a statement
 * @param parser Parser state
 * A higher level than this is an error. A lower ends the block and returns NULL.
 * @return A statement pointer on success and the parser->curtoken is set at the token after the statement. NULL if the block should end.
*/
static Ny_Statement* parse_statement(
	Ny_ParserState* parser
)
{
	Ny_Token* token = get_current_token(parser);
	printf("Parsing statement starting with '"); Ny_PrintToken(token); printf("'\n");
	
	Ny_Statement* stmt = NULL;
	switch (token->keywordid)
	{
	case Ny_KW_IF: stmt = parse_if_statement(parser);
	default:
		break;
	}

	return stmt;
}



/*
** @brief 
** @param parser 
** @param parent_indentlevel The previous non -1 indent level
** @return 
*/
static Ny_CodeBlock* parse_codeblock(
	Ny_ParserState* parser,
	int parent_indentlevel
)
{
	Ny_Assert(parent_indentlevel >= 0);
	
	Ny_CodeBlock* block = Ny_AllocType(Ny_CodeBlock);
	if (!block) return NULL;
	if (!Ny_InitVector(&block->statements));
	Ny_Token* block_firsttoken = get_current_token(parser);
	if (!block_firsttoken) return NULL;
	int block_indentlevel = block_firsttoken->indentlevel;
	Ny_Assert(block_indentlevel >= -1);

	printf("<Block> {\n");

	while (parser->curtoken < parser->tokens.count)
	{
		Ny_Token* token = get_current_token(parser);
		Ny_Assert(token); /* The while loop already prevents token from being null */
		
		/* Based on the indentation, decide if there is an error, to exit the block (break) or continue looping */
		if (block_indentlevel == -1)
		{
			/* Inline block */
			//if (token->indentlevel >)
			if (token->indentlevel != -1) break;
		} else
		{
			/* Not inline block */
		}
		

		Ny_Statement* stmt = parse_statement(parser);
		if (!stmt)
		{
			find_next_token_on_indentlevel(parser, block_indentlevel);
			break;
		}
		else
		{
			Ny_PushBackVector(&block->statements, stmt);
		}
	}

	printf("} </block>\n");
	return block;
}





#include <time.h>

Ny_AST* Ny_ParseSourceCode(Ny_State* state, const char* sourcecode)
{
	Ny_ParserState parser = { 0 };
	parser.main_state = state;
	parser.tabsize = state->tabsize;
	parser.sourcecode_filename = "gruplin";

	clock_t lexer_start = clock();
	if (!Ny_TokenizeSourceCode(&parser, sourcecode)) return NULL;
	clock_t lexer_time = clock() - lexer_start;

	Ny_PrintSourceCodeTokens(&parser);

	clock_t parser_start = clock();
	Ny_CodeBlock* globalscope_block = parse_codeblock(&parser, 0, 0);
	clock_t parser_time = clock() - parser_start;
	
	if (!globalscope_block) goto fail_0;
	Ny_AST* ast = Ny_AllocType(Ny_AST);
	if (!ast) goto fail_1;
	ast->globalscope_block = globalscope_block;

	printf("Lexing took: %dms\n", lexer_time);
	printf("Parsing took: %dms\n\n", parser_time);

	Ny_Assert(state->messages.count == parser.errorcount);

	return ast;


fail_1:
	Ny_FreeCodeBlock(globalscope_block);
fail_0:
	for (int i = 0; i < (int)parser.tokens.count; i++)
	{
		Ny_FreeToken(parser.tokens.buffer[i]);
	}
	Ny_Free(parser.tokens.buffer);
	return NULL;
}
