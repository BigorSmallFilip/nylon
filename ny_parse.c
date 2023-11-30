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

static void print_indent(int depth)
{
	for (int i = 0; i < depth; i++)
	{
		printf("  ");
	}
}

static void recursive_print_exprnode(const Ny_ExprNode* node, int depth)
{
	if (node->type == Ny_ET_OPERATOR)
	{
		print_indent(depth);
		Ny_PrintExprNode(node);
		printf(": {\n");

		if (node->left)
		{
			print_indent(depth + 1);
			printf("left: {\n");
			recursive_print_exprnode(node->left, depth + 2);
			print_indent(depth + 1);
			printf("}\n");
		}
		if (node->right)
		{
			print_indent(depth + 1);
			printf("right: {\n");
			recursive_print_exprnode(node->right, depth + 2);
			print_indent(depth + 1);
			printf("}\n");
		}

		print_indent(depth);
		printf("}\n");
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
	
}





/* Forward declarations */

static Ny_CodeBlock* parse_code_block(Ny_ParserState* parser);
static Ny_Statement* parse_statement(Ny_ParserState* parser);
static Ny_Bool parse_subexpression(Ny_ParserState* parser, Ny_Expression* expr, Ny_Bool endline);



static Ny_Token* get_current_token(Ny_ParserState* parser)
{
	if (parser->curtoken - 1 < 0) return NULL;
	return parser->tokens.buffer[parser->curtoken - 1];
}static Ny_Token* get_next_token(Ny_ParserState* parser)
{
	if (parser->curtoken >= parser->tokens.count) return NULL;
	return parser->tokens.buffer[parser->curtoken++];
}
static Ny_Token* peek_next_token(const Ny_ParserState* parser)
{
	if (parser->curtoken >= parser->tokens.count) return NULL;
	return parser->tokens.buffer[parser->curtoken];
}



static Ny_ExprNode* create_exprnode(Ny_ExprNodeType type)
{
	Ny_ExprNode* node = Ny_AllocType(Ny_ExprNode);
	node->type = type;
	return node;
}

static Ny_ExprNode* parse_function_call(Ny_ParserState* parser, Ny_Token* token)
{
	printf("unimplemented functoin\n");
	return NULL;
}

static Ny_ExprNode* parse_operand(Ny_ParserState* parser, Ny_Token* token)
{
	Ny_ExprNode* node = NULL;
	switch (token->type)
	{
	case Ny_TT_IDENTIFIER:
	{
		Ny_Token* nexttoken = peek_next_token(parser);
		if (nexttoken && nexttoken->separatorid == Ny_SP_LPAREN)
		{
			/* Function call */
			node = parse_function_call(parser, token);
		} else
		{
			/* Variable */
			node = create_exprnode(Ny_ET_VARIABLE);
			node->variable = _strdup(token->string);
		}
		break;
	}

	case Ny_TT_INTLITERAL:
		node = create_exprnode(Ny_ET_INTLITERAL);
		node->numint = token->numint;
		break;
	case Ny_TT_FLOATLITERAL:
		node = create_exprnode(Ny_ET_FLOATLITERAL);
		node->numfloat = token->numfloat;
		break;
	case Ny_TT_STRINGLITERAL:
		node = create_exprnode(Ny_ET_STRINGLITERAL);
		node->string = _strdup(token->string);
		break;

	default:
		break;
	}
	return node;
}

static Ny_Bool parse_operator(
	Ny_ParserState* parser,
	Ny_Vector* operatorstack,
	Ny_Expression* expr,
	Ny_Token* token
)
{
	Ny_ExprNode* node = create_exprnode(Ny_ET_OPERATOR);
	node->op = token->operatorid;

	Ny_ExprNode* stacktop;
	if (/*Ny_IsAssignmentOp(node->op) || */!(stacktop = Ny_VectorPBack(operatorstack, Ny_ExprNode*)))
	{
		Ny_PushBackVector(operatorstack, node);
	} else if (Ny_OpPrecedence(node->op) > Ny_OpPrecedence(stacktop->op))
	{
		/* Higher precedence */
		if (stacktop->op == node->op)
		{
			/* If operators are the same */
			if (Ny_IsAssignmentOp(node->op))
			{
				printf("Oh dear god this is confusing\n");
				Ny_Assert(0);
			} else
			{
				Ny_PopBackVector(operatorstack);
				Ny_PushBackVector(&expr->nodes, stacktop);
				Ny_PushBackVector(operatorstack, node);
			}
		} else
		{
			/* Push operator to stack */
			Ny_PushBackVector(operatorstack, node);
		}
	} else
	{
		/* Lesser than or equal equal precedence */
		while (stacktop = Ny_VectorPBack(operatorstack, Ny_ExprNode*))
		{
			if (Ny_IsAssignmentOp(node->op) && Ny_IsAssignmentOp(stacktop->op)) break;

			if (Ny_OpPrecedence(stacktop->op) >= Ny_OpPrecedence(node->op))
			{
				Ny_PopBackVector(operatorstack);
				Ny_PushBackVector(&expr->nodes, stacktop);
			} else break;
		}
		Ny_PushBackVector(operatorstack, node);
	}
	return Ny_TRUE;
}

static Ny_Bool parse_separator(
	Ny_ParserState* parser,
	Ny_Vector* operatorstack,
	Ny_Expression* expr,
	Ny_Token* token,
	Ny_Bool* exprend
)
{
	Ny_ExprNode* node;

	switch (token->separatorid)
	{
	case Ny_SP_LPAREN:
		if (!parse_subexpression(parser, expr, Ny_FALSE)) return Ny_FALSE;
		token = get_current_token(parser);
		if (!token || token->separatorid != Ny_SP_RPAREN)
		{
			printf("Expected closing parenthesis\n");
			return Ny_FALSE;
		}
		break;

	case Ny_SP_LBRACKET:
		node = create_exprnode(Ny_ET_OPERATOR);
		node->op = Ny_OP_ARRAYACCESS;
		Ny_PushBackVector(&operatorstack, node);
		if (!parse_subexpression(parser, expr, Ny_FALSE)) return Ny_FALSE;
		token = get_current_token(parser);
		if (!token || token->separatorid != Ny_SP_RBRACKET)
		{
			printf("Expected closing square bracket\n");
			return Ny_FALSE;
		}
		/* Push the arrayaccess operator */
		Ny_PopBackVector(&operatorstack);
		Ny_PushBackVector(&expr->nodes, node);
		break;

	case Ny_SP_LBRACE:
		/* Read object probably */
		Ny_Assert(0);
		break;

	default:
		*exprend = Ny_TRUE;
		break;
	}
	return Ny_TRUE;
}

static Ny_Bool parse_subexpression(Ny_ParserState* parser, Ny_Expression* expr, Ny_Bool endline)
{
	Ny_Bool exprend = Ny_FALSE;
	Ny_Vector operatorstack;
	Ny_InitVector(&operatorstack, Ny_MIN_VECTOR_CAPACITY);

	while (!exprend)
	{
		Ny_Token* token = get_next_token(parser);
		Ny_Assert(token);
		Ny_ExprNode* node;

		if (token->type == Ny_TT_OPERATOR)
		{
			if (!parse_operator(parser, &operatorstack, expr, token)) goto fail;
		} else if (token->type == Ny_TT_SEPARATOR)
		{
			if (!parse_separator(parser, &operatorstack, expr, token, &exprend)) goto fail;
		} else
		{
			/* Operand */
			node = parse_operand(parser, token);
			if (!node) goto fail;
			Ny_PushBackVector(&expr->nodes, node);
		}

		/* If expression should end on endline and token is last on line then expression has ended */
		if (endline)
		{
			token = get_current_token(parser);
			if (!token || token->lastonline)
				exprend = Ny_TRUE;
		}

		/* Print the expression as it is right now */
		/*printf("Expression: ");
		for (int i = 0; i < expr->nodes.count; i++)
		{
			Ny_PrintExprNode(expr->nodes.buffer[i]);
			printf(", ");
		}
		putchar('\n');
		printf("Stack: ");
		for (int i = 0; i < operatorstack.count; i++)
		{
			Ny_PrintExprNode(operatorstack.buffer[i]);
			printf(", ");
		}
		printf("\n\n");*/
	}

	/* Put all operators left in the stack in the expression */
	int remaining = operatorstack.count;
	for (int i = 0; i < remaining; i++)
	{
		Ny_ExprNode* node = Ny_PopBackVector(&operatorstack);
		Ny_PushBackVector(&expr->nodes, node);
	}

	/*printf("\n---final expression: ");
	for (int i = 0; i < expr->nodes.count; i++)
	{
		Ny_PrintExprNode(expr->nodes.buffer[i]);
		printf(", ");
	}
	putchar('\n');*/
	return Ny_TRUE;

fail:
	return Ny_FALSE;
}

static Ny_Bool build_expression_tree_operator(Ny_ParserState* parser, Ny_Expression* expr, int* nodeid)
{
	Ny_ExprNode* node = expr->nodes.buffer[*nodeid];
	Ny_Assert(node->type == Ny_ET_OPERATOR);

	//Ny_PrintExprNode(node);
	//printf("\n");

	if (!Ny_IsUnaryOp(node->op))
	{
		Ny_ExprNode* right = expr->nodes.buffer[--(*nodeid)];
		if (right->type == Ny_ET_OPERATOR)
			build_expression_tree_operator(parser, expr, nodeid);
		node->right = right;

		Ny_ExprNode* left = expr->nodes.buffer[--(*nodeid)];
		if (left->type == Ny_ET_OPERATOR)
			build_expression_tree_operator(parser, expr, nodeid);
		node->left = left;
	} else
	{
		printf("unimplemented\n");
		return Ny_FALSE;
	}

	return Ny_TRUE;
}

static Ny_Bool build_expression_tree(Ny_ParserState* parser, Ny_Expression* expr)
{
	Ny_Assert(!expr->topnode);

	Ny_ExprNode* topnode = Ny_VectorBack(expr->nodes, Ny_ExprNode*);
	if (!topnode) return Ny_FALSE;
	if (topnode->type != Ny_ET_OPERATOR)
	{
		/* Single node tree */
		Ny_Assert(expr->nodes.count == 1);
	} else
	{
		int nodeid = expr->nodes.count - 1;
		build_expression_tree_operator(parser, expr, &nodeid);
	}
	expr->topnode = topnode;

	return Ny_TRUE;
}

static Ny_Expression* parse_expression(Ny_ParserState* parser, Ny_Bool endline)
{
	Ny_Expression* expr = Ny_AllocType(Ny_Expression);
	parse_subexpression(parser, expr, endline);

	printf("---final expression: ");
	for (int i = 0; i < expr->nodes.count; i++)
	{
		Ny_PrintExprNode(expr->nodes.buffer[i]);
		printf(", ");
	}
	putchar('\n');

	build_expression_tree(parser, expr);

	Ny_PrintExpressionTree(expr);

	return expr;
}



static Ny_Statement* parse_expression_statement(Ny_ParserState* parser)
{
	Ny_Expression* expr = parse_expression(parser, Ny_TRUE);
	if (!expr) return NULL;
	Ny_Statement* stmt = Ny_AllocType(Ny_Statement);
	stmt->type = Ny_ST_EXPRESSION;
	stmt->stmt_expr.expression = expr;
}



static Ny_Statement* parse_if_statement(Ny_ParserState* parser)
{
	/* Check for left parenthesis */
	Ny_Token* token = get_next_token(parser);
	if (token->separatorid != Ny_SP_LPAREN)
	{
		/* Missing left parenthesis */
		return NULL;
	}
	/* Parse condition expression */
	Ny_Expression* condition = parse_expression(parser, Ny_FALSE);
	if (!condition) return NULL;
	/* Check for right parenthesis */
	token = get_next_token(parser);
	if (token->separatorid != Ny_SP_RPAREN)
	{
		/* Missing right parenthesis */
		goto fail;
	}

	/* Parse ontrue code block */
	Ny_CodeBlock* block_ontrue = NULL;
	token = peek_next_token(parser);
	if (!token)
	{
		/* Unexpected eof */
		goto fail;
	}
	if (token->separatorid == Ny_SP_LBRACE)
	{
		parser->curtoken++;
		block_ontrue = parse_code_block(parser);
		if (!block_ontrue) goto fail;
		token = get_next_token(parser);
		if (!token || token->separatorid != Ny_SP_RBRACE)
		{
			/* Missing } */
			goto fail;
		}
	} else
	{
		/* Single statement code block */
		parse_statement(parser);
	}

	Ny_Statement* stmt = Ny_AllocType(Ny_Statement);
	stmt->type = Ny_ST_IF;
	stmt->stmt_if.condition = condition;
	stmt->stmt_if.block_ontrue = block_ontrue;

fail:
	Ny_DestroyExpression(condition);
	return NULL;
}

static Ny_Statement* parse_statement(Ny_ParserState* parser)
{
	Ny_Token* token = get_next_token(parser);
	printf("Parsing statement starting with token: ");
	Ny_PrintToken(token);
	printf("\n");
	if (!token) return NULL;

	switch (token->keywordid)
	{
	case Ny_KW_IF: return parse_if_statement(parser);
	default:
		parser->curtoken--; /* Include first token of statement */
		parse_expression_statement(parser);
		break;
	}
}

static Ny_CodeBlock* parse_code_block(Ny_ParserState* parser)
{
	Ny_CodeBlock* block = Ny_AllocType(Ny_CodeBlock);
	
	while (parser->curtoken < parser->tokens.count)
	{
		parse_statement(parser);
	}
}



Ny_AST* Ny_ParseSourceCode(Ny_State* state, const char* sourcecode)
{
	Ny_DebugPrint("Parsing source code:\n%s\n", sourcecode);

	Ny_ParserState parser = { 0 };
	Ny_TokenizeSourceCode(&parser, sourcecode);

	Ny_DebugPrint("Tokenization process finished\n");
	for (int i = 0; i < parser.tokens.count; i++)
	{
		Ny_Token* token = parser.tokens.buffer[i];
		Ny_PrintToken(token);
		putchar(' ');
	}
	putchar('\n');

	parse_code_block(&parser);

	return NULL;
}
