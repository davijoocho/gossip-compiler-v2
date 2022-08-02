
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

int main(int argc, char** argv)
{
    // Open and read in source file
    FILE* fp = fopen(argv[1], "rb");
    fseek(fp, 0, SEEK_END);
    long src_len = ftell(fp);
    char* src = malloc(src_len);
    rewind(fp);
    fread(src, 1, src_len, fp);
    fclose(fp);


    struct tokens* tokens = lexical_analysis(src, src_len);

	/*

	char* types[] = {"ARROW", "DOT", "LEFT_BRACK", "STAR", "SLASH", "MODULUS", "PLUS", "MINUS",
	"BIT_SHIFTR", "BIT_SHIFTL", "GT", "LT", "GTEQ", "LTEQ", "EQUAL", "NOT_EQUAL", "BIT_AND",
	"BIT_OR", "BIT_XOR", "LOG_AND", "LOG_OR", "BIT_NOT", "LOG_NOT", "LEFT_PAREN", "RIGHT_PAREN",
	"RIGHT_BRACK", "COMMA", "LEFT_BRACE", "RIGHT_BRACE", "SEMI_COLON", "CHARACTER", "INTEGER", "LONG",
	"FLOAT", "DOUBLE", "IDENTIFIER", "STRING_LITERAL", "NONE", "C8", "I32", "I64", "F32", "F64", "STRING", 
	"VOID", "FUNCTION", "STRUCT", "ASSIGN", "IF", "ELIF", "ELSE", "WHILE", "RETURN", "EOFF"};

	for (int i = 0; i < tokens->n_tokens; i++) {
		struct token* token = tokens->tokens[i];
		char* id = malloc(token->end_idx - token->start_idx + 1);
		id[token->end_idx - token->start_idx] = '\0';
		strncpy(id, src + token->start_idx, token->end_idx - token->start_idx);
		printf("%x %x %s %s %d \n", token->start_idx, token->end_idx, types[token->type], id, token->line);
	}

	*/
	struct program* program = syntax_analysis(tokens, src);



    return 0;
}
