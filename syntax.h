#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "table.h"
#include "helperFunctions.h"

typedef enum TokenType {
	LABELDEF, /*label def 'a23s:'*/
	LABEL_OR_INSTRUCTION, /*string of alphanumeric chars 'sdf4advc'*/
	REGISTER, /* 'r3' */
	IMMIDIATE, /* '#-3','#+4','#2'*/
	RELATIVE,  /* '%a' */
	NUMBER,    /* '3' */
	STRING,	   /* "sdfsdf" */
	D_DIRECTIVE, /*'.data'*/
	S_DIRECTIVE, /*'.string'*/
	EN_DIRECTIVE, /*'.entry'*/
	EX_DIRECTIVE, /*'.external'*/
	COMMENT, /*';dsfsdfsdf'*/
	NULL_P, /* special case saved for when there is no para
			for instruction*/
	ERROR
} TokenType;

typedef struct Token {
	TokenType type;
	char* tag;	
}Token;

void token_free(Token* t);
TokenType check_tok_type(char* str);
Token** analyze_tokens(char* str[],int size);
int check_syntax(char* line,Table* instrctable,Token** toks[]);


