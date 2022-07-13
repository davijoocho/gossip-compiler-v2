




#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


struct program* syntax_analysis(struct tokens* tokens, char* src) {
	struct program* program = malloc(sizeof(struct program));
	program->stmts = malloc(sizeof(struct stmt*) * 512);
	program->n_stmts = 512;

	int capacity = 512;
	int error = 0;

	while (tokens->tokens[tokens->idx]->type != EOFF) {
		if (program->n_stmts == capacity) {
			program->stmts = realloc(program->stmts, sizeof(struct stmt*) * capacity * 2);
			capacity *= 2;
		}

		struct stmt* parsed_stmt = parse_def(tokens, src);

		if (!error) {
			if (parsed_stmt != NULL) {
				program->stmts[program->n_stmts++] = parsed_stmt;
			} else {
				error = 1;
			}
		}
	}


	if (error) {
		return NULL;
	}

	return program;
}



void print_err(struct token* err_token, char* src, char* err_msg) {
	int line = 1;
	int beg_idx = 0;
	int end_idx = 0;

	printf(ANSI_COLOR_RED "ERROR " ANSI_COLOR_RESET "LINE (%d): %s\n", err_token->line, err_msg);
	while (line != err_token->line) {
		if (src[beg_idx] == '\n') {
			line++;
		}
		beg_idx++;
	}

	end_idx = beg_idx;
	while (src[end_idx] != '\n') {
		fputc(src[end_idx], stdout);
		end_idx++;
	}
	fputc('\n', stdout);

	for (int i = 0; i < err_token->start_idx - beg_idx; i++) {
		fputc(' ', stdout);
	}
	printf(ANSI_COLOR_GREEN "^" ANSI_COLOR_RESET "\n");
}



// mode == 0 --> definition, mode == 1 --> statements
void panic_mode(struct tokens* toks, int mode) {
	if (mode == 0) {
		enum token_type tok_type = toks->tokens[toks->idx]->type;
		while (tok_type != STRUCT && tok_type != FUNCTION && tok_type != EOFF) {
			tok_type = toks->tokens[++toks->idx]->type;
		}

	} else if (mode == 1) {
		// TODO
	}
}




struct stmt* parse_def(struct tokens* toks, char* src) {
	struct token* cur_tok = toks->tokens[toks->idx];
	struct stmt* def = NULL;

	if (cur_tok->type == STRUCT) {
		struct _struct* _struct = NULL;

		// ERROR: CURRENT TOKEN IS NOT AN IDENTIFIER
		cur_tok = toks->tokens[++toks->idx];
		if (cur_tok->type != IDENTIFIER) {

			printf("here\n");
			print_err(cur_tok, src, "expected an identifier for a struct definition");
			panic_mode(toks, 0);
			return NULL;
		}

/*

		//_struct->id = tokens->tokens[tokens->idx++];

		// error: if token is not '{'
		tokens->idx++;

		while (tokens->tokens[tokens->idx]->type != '}') {

		}
		def->parse_stmt(tokens);
		// error: if token is not '}'
		//def->type = _STRUCT;
		*/

	}  else if (cur_tok->type == FUNCTION) {

		//tokens->idx++;

	} else {
		// error top-level definitions must be struct or function definition
	}

	return def;
}

















