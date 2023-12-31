#include "ny_parse.h"

#include <ctype.h>



const char* ny_keyword_strings[Ny_NUM_KEYWORDS] =
{
	"if",
	"else",
	"for",
	"do",
	"while",
	"function",
	"return",
	"false",
	"true"
};

Ny_KeywordID Ny_GetKeyword(const char* string)
{
	for (int i = 0; i < Ny_NUM_KEYWORDS; i++)
		if (strcmp(string, ny_keyword_strings[i]) == 0)
			return (Ny_KeywordID)i;
	return Ny_KW_NULL;
}


const char* ny_operator_strings[Ny_NUM_OPERATORS] =
{
	"=",
	"+=",
	"-=",
	"*=",
	"/=",

	"!",
	"&",
	"|",
	"^",
	"", /* Unary negative has a special case */

	"==",
	"!=",
	"<",
	">",
	"<=",
	">=",

	"+",
	"-",
	"*",
	"/",
	"%",

	".",
	"", /* Array access has a special case */
};

const char* ny_operatorid_strings[Ny_NUM_OPERATORS] =
{
	"OP_ASSIGN",
	"OP_ASSIGNADD",
	"OP_ASSIGNSUB",
	"OP_ASSIGNMUL",
	"OP_ASSIGNDIV",

	"OP_NOT",
	"OP_AND",
	"OP_OR",
	"OP_XOR",
	"OP_NEGATIVE",

	"OP_EQUALITY",
	"OP_INEQUALITY",
	"OP_LESS",
	"OP_GREATER",
	"OP_LESSEQUAL",
	"OP_GREATEREQUAL",

	"OP_ADD",
	"OP_SUB",
	"OP_MUL",
	"OP_DIV",
	"OP_MOD",

	"OP_MEMBERACCESS",
	"OP_ARRAYACCESS",
};

const unsigned char ny_operator_precedence[Ny_NUM_OPERATORS] =
{
	1,
	1,
	1,
	1,
	1,

	7,
	8,
	10,
	9,
	6,

	2,
	2,
	3,
	3,
	3,
	3,

	4,
	4,
	5,
	5,
	5,

	11,
	12,
};

Ny_OperatorID Ny_GetOperator(const char* string)
{
	for (int i = 0; i < Ny_NUM_OPERATORS; i++)
		if (strcmp(string, ny_operator_strings[i]) == 0)
			return (Ny_OperatorID)i;
	return Ny_OP_NULL;
}


const char ny_separator_chars[Ny_NUM_SEPARATORS] =
{
	'(',
	')',
	'[',
	']',
	'{',
	'}',
	',',
};

Ny_SeparatorID Ny_GetSeparator(const char c)
{
	for (int i = 0; i < Ny_NUM_SEPARATORS; i++)
		if (c == ny_separator_chars[i])
			return (Ny_SeparatorID)i;
	return Ny_SP_NULL;
}



void Ny_PrintToken(const Ny_Token* token)
{
	if (!token)
	{
		printf("(null token)");
		return;
	}
	switch (token->type)
	{
	case Ny_TT_KEYWORD:
		if (token->keywordid >= 0 && token->keywordid < Ny_NUM_KEYWORDS)
			printf("%s", ny_keyword_strings[token->keywordid]);
		else
			printf("(invalid keywordid: %d)", token->keywordid);
		break;
	case Ny_TT_OPERATOR:
		if (token->operatorid >= 0 && token->operatorid < Ny_NUM_OPERATORS)
			printf("%s", ny_operator_strings[token->operatorid]);
		else
			printf("(invalid operatorid: %d)", token->operatorid);
		break;
	case Ny_TT_SEPARATOR:
		if (token->separatorid >= 0 && token->separatorid < Ny_NUM_SEPARATORS)
			printf("%c", ny_separator_chars[token->separatorid]);
		else
			printf("(invalid separatorid: %d)", token->separatorid);
		break;
	case Ny_TT_INTLITERAL: printf("%lld", token->numint); break;
	case Ny_TT_FLOATLITERAL: printf("%f", token->numfloat); break;
	case Ny_TT_IDENTIFIER: printf("%s", token->string); break;
	case Ny_TT_STRINGLITERAL: printf("\"%s\"", token->string); break;
	default:
		break;
	}
}



/* ================ Tokenization ================*/

typedef enum
{
	CT_NULL,		/* Default type or any char not recognised */
	CT_ALPHA,		/* Letter or underscore */
	CT_NUMBER,
	CT_POINT,
	CT_OPERATOR,
	CT_SEPARATOR,
	CT_SPACE,		/* Space or tab */
	CT_QUOTE,		/* Quotation marks for strings */
	CT_COMMENT,		/* Comments start with # and end with an endline */
	CT_ENDLINE,
} chartype;

#define is_alpha(c) (isalpha(c) || c == '_')
#define is_operator_char(c) (strchr("+-/*=<>!&|^", c))
#define is_separator_char(c) (strchr("()[]{},", c))
#define is_quote_char(c) (c == '\"' || c == '\'')
#define is_invalid(c) (c < 0)

static chartype check_chartype(const char c)
{
	if (is_alpha(c))           return CT_ALPHA;
	if (isdigit(c))            return CT_NUMBER;
	if (c == '.')              return CT_POINT;
	if (is_operator_char(c))   return CT_OPERATOR;
	if (is_separator_char(c))  return CT_SEPARATOR;
	if (isblank(c))            return CT_SPACE;
	if (c == '\n' || c == ';') return CT_ENDLINE; /* Semicolon acts the same as endline */
	if (is_quote_char(c))      return CT_QUOTE;
	if (c == '#')              return CT_COMMENT;
	if (c == '\n')             return CT_ENDLINE;
	return CT_NULL;
}

static Ny_Token* create_token()
{
	Ny_Token* token = malloc(sizeof(Ny_Token)); /* Malloc instead of calloc since this will setup everything manually so no need to 0 out token */
	token->type = Ny_TT_NULL;
	token->keywordid = Ny_KW_NULL;
	token->operatorid = Ny_OP_NULL;
	token->separatorid = Ny_SP_NULL;
	token->string = NULL;
	token->lastonline = Ny_FALSE;
	token->linenum = -1;
	return token;
}



static Ny_Token* read_alpha_token(
	Ny_ParserState* parser,
	const char* sourcecode
)
{
	int start = parser->charpos;
	int end = -1;
	for (int i = start + 1; i < Ny_MAX_SOURCECODE_LENGTH; i++)
	{
		char c = sourcecode[i];
		if (!is_invalid(c) && !is_alpha(c) && !isdigit(c))
		{
			end = i;
			break;
		}
	}
	Ny_Token* token = create_token();
	char* cutstring = Ny_CopyCutString(sourcecode, start, end - start);
	token->keywordid = Ny_GetKeyword(cutstring);
	if (token->keywordid == Ny_KW_NULL)
	{
		token->type = Ny_TT_IDENTIFIER;
		token->string = cutstring;
	} else
	{
		token->type = Ny_TT_KEYWORD;
		token->string = NULL;
		free(cutstring);
	}
	parser->charpos = end;
	return token;
}

static Ny_Token* read_number_token(
	Ny_ParserState* parser,
	const char* sourcecode
)
{
	int start = parser->charpos;
	int end = -1;
	Ny_Bool decimalpoint = Ny_FALSE; /* For checking if there are two decimal points in one number */
	for (int i = start + 1; i < Ny_MAX_SOURCECODE_LENGTH; i++)
	{
		char c = sourcecode[i];
		if (is_invalid(c))
		{
			printf("Invalid char");
			break;
		} else if (c == '.')
		{
			if (decimalpoint)
			{
				//Lnn_PushParserSyntaxError(linenum, "Two decimal points in one number");
				printf("Two decimal points in one number");
				return NULL;
			}
			decimalpoint = Ny_TRUE;
		} else if (!isdigit(c))
		{
			if (is_alpha(c))
			{
				//Lnn_PushParserSyntaxError(linenum, "Letter character '%c' directly after number", sourcecode[i]);
				return NULL;
			}
			end = i;
			break;
		}
	}
	Ny_Token* token = create_token();
	char* string = Ny_CopyCutString(sourcecode, start, end - start);
	if (decimalpoint)
	{
		token->numfloat = Ny_StringToFloat(string, NULL);
		token->type = Ny_TT_FLOATLITERAL;
	} else
	{
		token->numint = Ny_StringToInt(string, NULL, 10);
		token->type = Ny_TT_INTLITERAL;
	}
	free(string);
	parser->charpos = end;
	return token;
}

static Ny_Token* read_operator_token(
	Ny_ParserState* parser,
	const char* sourcecode
)
{
	int start = parser->charpos;
	int end = -1;
	char opstring[3] = { 0 };
	opstring[0] = sourcecode[start];
	char opchar2 = sourcecode[start + 1];
	if (!is_invalid(opchar2) && is_operator_char(opchar2))
	{
		opstring[1] = opchar2;
		end = start + 2;
	} else
		end = start + 1;
	Ny_OperatorID id = Ny_GetOperator(&opstring);
	if (Ny_OperatorInvalid(id))
	{
		printf("Invalid operator\n");
		return NULL;
	}
	Ny_Token* token = create_token();
	token->operatorid = id;
	token->type = Ny_TT_OPERATOR;
	parser->charpos = end;
	return token;
}

static Ny_Token* read_separator_token(
	Ny_ParserState* parser,
	char c
)
{
	Ny_SeparatorID id = Ny_GetSeparator(c);
	if (Ny_SeparatorInvalid(id))
	{
		printf("Invalid separator\n");
		return NULL;
	}
	Ny_Token* token = create_token();
	token->separatorid = id;
	token->type = Ny_TT_SEPARATOR;
	parser->charpos++;
	return token;
}

static Ny_Token* read_string_token(
	Ny_ParserState* parser,
	const char* sourcecode
)
{
	int start = parser->charpos;
	int end = -1;
	for (int i = start + 1; i < Ny_MAX_SOURCECODE_LENGTH; i++)
	{
		char c = sourcecode[i];
		if (!is_invalid(c) && is_quote_char(c))
		{
			end = i + 1;
			break;
		}
	}
	Ny_Token* token = create_token();
	char* cutstring = Ny_CopyCutString(sourcecode, start + 1, end - start - 2);
	token->type = Ny_TT_STRINGLITERAL;
	token->string = cutstring;
	parser->charpos = end;
	return token;
}



Ny_Bool Ny_TokenizeSourceCode(Ny_ParserState* parser, const char* sourcecode)
{
	parser->charpos = 0;
	parser->linenum = 1;
	Ny_InitVector(&parser->tokens, Ny_MIN_VECTOR_CAPACITY);
	parser->sourcecode = sourcecode;

	while (Ny_TRUE)
	{
		if (parser->charpos >= Ny_MAX_SOURCECODE_LENGTH)
		{
			printf("ERROR! Sourcecode exceeds max length of " Ny_Stringify(Ny_MAX_SOURCECODE_LENGTH) " characters.\n");
			break;
		}

		char c = sourcecode[parser->charpos];
		if (c == '\0') break;
		if (c < 0)
		{
			printf("ERROR! Source code contains invalid character on line %i. Nylon currently only supports ASCII.\n", parser->linenum);
			parser->charpos++;
			continue;
		}

		Ny_Token* token = NULL;
		switch (check_chartype(c))
		{
		case CT_ALPHA:     token = read_alpha_token(parser, sourcecode); break;
		case CT_NUMBER:	   token = read_number_token(parser, sourcecode); break;
		case CT_POINT:
		case CT_OPERATOR:  token = read_operator_token(parser, sourcecode); break;
		case CT_SEPARATOR: token = read_separator_token(parser, c); break;
		case CT_QUOTE:     token = read_string_token(parser, sourcecode); break;
		case CT_ENDLINE:
		{
			/* If an endline char is found then the previous token gets the lastonline flag set */
			Ny_Token* lasttoken = (Ny_Token*)parser->tokens.buffer[parser->tokens.count - 1];
			lasttoken->lastonline = Ny_TRUE;
		} /* Fall through */
		case CT_SPACE:
			parser->charpos++;
			break;
		default:
			printf("Invalid character in source code!\n");
			parser->charpos++;
			break;
		}

		if (token)
		{
			token->linenum = parser->linenum;
			Ny_PushBackVector(&parser->tokens, token);
		} else
		{
			parser->errorcount++;
		}
	}

	/* Last token in file gets the lastonline flag set */
	Ny_Token* lasttoken = Ny_VectorBack(parser->tokens, Ny_Token*);
	if (lasttoken) lasttoken->lastonline = Ny_TRUE;

	return parser;
}