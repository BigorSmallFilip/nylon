#ifndef __Ny_PARSE_H__
#define __Ny_PARSE_H__

#include "ny_utility.h"

#include "ny_state.h"



typedef char Ny_KeywordID;
enum
{
	Ny_KW_IF,
	Ny_KW_ELSE,
	Ny_KW_FOR,
	Ny_KW_DO,
	Ny_KW_WHILE,
	Ny_KW_FUNCTION,
	Ny_KW_RETURN,
	Ny_KW_FALSE,
	Ny_KW_TRUE,
	Ny_NUM_KEYWORDS,
	Ny_KW_NULL = -1, /* Invalid or non keyword */
};
#define Ny_KeywordInvalid(id) (id < 0)
extern const char* ny_keyword_strings[Ny_NUM_KEYWORDS];

/*
** @brief Checks the keyword id of a string.
** @param string String to check.
** @return The ID of the keyword or Ny_KW_NULL if not keyword.
*/
Ny_KeywordID Ny_GetKeyword(const char* string);



typedef char Ny_OperatorID;
enum
{
	Ny_OP_ASSIGN,       /* '=' */
	Ny_OP_ASSIGNADD,    /* '+=' */
	Ny_OP_ASSIGNSUB,    /* '-=' */
	Ny_OP_ASSIGNMUL,    /* '*=' */
	Ny_OP_ASSIGNDIV,    /* '/=' */

	Ny_OP_NOT,          /* '!' Logical not */
	Ny_OP_AND,          /* '&' Logical and */
	Ny_OP_OR,           /* '|' Logical or */
	Ny_OP_XOR,          /* '^' Logical xor */
	Ny_OP_NEGATIVE,     /* '-' Unary negative */

	Ny_OP_EQUALITY,     /* '==' */
	Ny_OP_INEQUALITY,   /* '!=' */
	Ny_OP_LESS,         /* '<'  */
	Ny_OP_GREATER,      /* '>'  */
	Ny_OP_LESSEQUAL,    /* '<=' */
	Ny_OP_GREATEREQUAL, /* '>=' */

	Ny_OP_ADD,          /* '+' Arithmetic add */
	Ny_OP_SUB,          /* '-' Arithmetic subtract */
	Ny_OP_MUL,          /* '*' Arithmetic multiply */
	Ny_OP_DIV,          /* '/' Arithmetic divide */
	Ny_OP_MOD,          /* '%' Arithmetic mod */

	Ny_OP_MEMBERACCESS, /* '.' */
	Ny_OP_ARRAYACCESS,  /* '[]' */

	Ny_NUM_OPERATORS,
	Ny_OP_NULL = -1,    /* Invalid or non operator */
};
#define Ny_OperatorInvalid(id) (id < 0)
extern const char* ny_operator_strings[Ny_NUM_OPERATORS];
extern const char* ny_operatorid_strings[Ny_NUM_OPERATORS];
extern const unsigned char ny_operator_precedence[Ny_NUM_OPERATORS];

#define Ny_OpPrecedence(op)	(ny_operator_precedence[op])

/*
** @brief Checks the operator id of a string.
** @param string String to check.
** @return The ID of the operator or Ny_OP_NULL if not operator.
*/
Ny_OperatorID Ny_GetOperator(const char* string);

#define Ny_IsAssignmentOp(op)   ((op) >= Ny_OP_ASSIGN   && (op) <= Ny_OP_ASSIGNDIV)
#define Ny_IsLogicalOp(op)      ((op) >= Ny_OP_NOT      && (op) <= Ny_OP_XOR)
#define Ny_IsRelationalOp(op)   ((op) >= Ny_OP_EQUALITY && (op) <= Ny_OP_GREATEREQUAL)
#define Ny_IsArithmeticOp(op)   ((op) >= Ny_OP_ADD      && (op) <= Ny_OP_MOD)
#define Ny_IsUnaryOp(op)        ((op) == Ny_OP_NOT      && (op) == Ny_OP_NEGATIVE)



typedef char Ny_SeparatorID;
enum
{
	Ny_SP_LPAREN,		/* '(' */
	Ny_SP_RPAREN,		/* ')' */
	Ny_SP_LBRACKET,	/* '[' */
	Ny_SP_RBRACKET,	/* ']' */
	Ny_SP_LBRACE,		/* '{' */
	Ny_SP_RBRACE,		/* '}' */
	Ny_SP_COMMA,		/* ',' */
	Ny_NUM_SEPARATORS,
	Ny_SP_NULL = -1,	/* Invalid or non separator */
};
#define Ny_SeparatorInvalid(id) (id < 0)
extern const char ny_separator_chars[Ny_NUM_SEPARATORS];

/*
** @brief Checks the separator id of a char.
** @param c Character to check.
** @return The ID of the separator or Ny_SP_NULL if not separator char.
*/
Ny_SeparatorID Ny_GetSeparator(const char c);



enum
{
	Ny_ET_OPERATOR,
	Ny_ET_INTLITERAL,
	Ny_ET_FLOATLITERAL,
	Ny_ET_STRINGLITERAL,
	Ny_ET_BOOLLITERAL,
	Ny_ET_OBJECT,
	Ny_ET_VARIABLE,
	Ny_ET_CLOSURE,
	Ny_ET_FUNCTIONCALL,
	Ny_NUM_EXPRNODETYPES
};
typedef char Ny_ExprNodeType;
extern const char* ny_exprnodetype_names[Ny_NUM_EXPRNODETYPES];

typedef struct Ny_ExprNode
{
	Ny_ExprNodeType type;
	struct Ny_ExprNode* left; /* Left child */
	struct Ny_ExprNode* right; /* Right child */
	union
	{
		Ny_OperatorID op;
		Ny_Int numint;
		Ny_Float numfloat;
		Ny_Bool boolean;
		char* string;
		//Ny_Function* closure;
		char* variable;
		struct
		{
			char* identifier;
			int numargs;
			struct Ny_Expression* args; /* Array of arguments */
		} functioncall;
	};
} Ny_ExprNode;

void Ny_PrintExprNode(const Ny_ExprNode* node);

void Ny_DestroyExprNode(Ny_ExprNode* node);

typedef struct
{
	Ny_Vector nodes;
	Ny_ExprNode* topnode;
} Ny_Expression;

void Ny_PrintExpressionTree(const Ny_Expression* expr);

/*
** @brief Destroys an expression and all its nodes recursively
** @param expr Expression to destroy.
*/
void Ny_DestroyExpression(Ny_Expression* expr);



/*
** @brief Code blocks are containers for a list statements.
** These are the statements that are in the same scope depth and are executed in order.
** Scopes contain code blocks and are executed recursively from other code blocks.
*/
typedef struct Ny_CodeBlock
{
	Ny_Vector statements;

	struct Ny_CodeBlock* parent; /* Used to go up in the tree to the top node */
	char* filename; /* Only written in the global scope, top node */
} Ny_CodeBlock;

void Ny_PrintCodeTree(const Ny_CodeBlock* block);

/*
** @brief Destroys a code block and all its child nodes recursively.
** @param block Block to destroy.
*/
void Ny_DestroyCodeBlock(Ny_CodeBlock* block);



typedef enum
{
	Ny_ST_EXPRESSION,
	Ny_ST_IF,
	Ny_ST_FOR,
	Ny_ST_WHILE,
	Ny_ST_DOWHILE,
	Ny_ST_RETURN,
	Ny_NUM_STATEMENTTYPES
} Ny_StatementType;
extern const char* ny_statementtype_names[Ny_NUM_STATEMENTTYPES];

typedef struct Ny_Statement
{
	Ny_StatementType type;
	union
	{
		struct
		{
			Ny_Expression* expression;
		} stmt_expr;
		struct
		{
			Ny_Expression* expression;
		} stmt_return;
		struct
		{
			Ny_Expression* condition;
			Ny_CodeBlock* block_ontrue;
			Ny_CodeBlock* block_onfalse;
		} stmt_if;
		struct
		{
			Ny_Expression* init;
			Ny_Expression* condition;
			Ny_Expression* loop;
			Ny_CodeBlock* block;
		} stmt_for;
		struct
		{
			Ny_Expression* condition;
			Ny_CodeBlock* block;
		} stmt_while;
		struct
		{
			Ny_Expression* condition;
			Ny_CodeBlock* block;
		} stmt_dowhile;
		struct
		{
			Ny_CodeBlock* block;
		} stmt_scope;
	};
	int linenum;
} Ny_Statement;

const char* ny_statementtype_names[Ny_NUM_STATEMENTTYPES];

/*
** @brief Destroys a statement and all its child nodes recursively.
** @param stmt Statement to destroy.
*/
void Ny_DestroyStatement(Ny_Statement* stmt);



enum
{
	Ny_TT_KEYWORD,
	Ny_TT_OPERATOR,
	Ny_TT_SEPARATOR,
	Ny_TT_INTLITERAL,
	Ny_TT_FLOATLITERAL,
	Ny_TT_STRINGLITERAL,
	Ny_TT_IDENTIFIER,
	Ny_NUM_TOKENTYPES,
	Ny_TT_NULL = -1
};
typedef char Ny_TokenType;

typedef struct Ny_Token
{
	Ny_TokenType   type;
	Ny_KeywordID   keywordid;
	Ny_OperatorID  operatorid;
	Ny_SeparatorID separatorid;
	union
	{
		char*    string;
		Ny_Int   numint;
		Ny_Float numfloat;
	};
	unsigned short linenum;
	short lastonline; /* If this token is the last on a line */
} Ny_Token;

void Ny_PrintToken(const Ny_Token* token);

typedef struct
{
	Ny_State* main_state;
	const char* sourcecode;
	const char* filename;
	int charpos;
	int linenum;
	int errorcount;
	Ny_Vector tokens;
	int curtoken;
} Ny_ParserState;

#define Ny_MAX_SOURCECODE_LENGTH 50000

Ny_Bool Ny_TokenizeSourceCode(Ny_ParserState* parser, const char* sourcecode, const char* filename);





typedef struct Ny_AST
{
	Ny_CodeBlock global_codeblock;
} Ny_AST;

Ny_AST* Ny_ParseSourceCode(Ny_State* state, const char* sourcecode);

#endif