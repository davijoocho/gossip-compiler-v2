




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

		struct stmt* parsed_stmt = parse_stmt(tokens, src, 0, &error);
		if (!error) {
			program->stmts[program->n_stmts++] = parsed_stmt;
		}
	}


	if (error) {
		// free memory
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

	printf("    ");
	end_idx = beg_idx;
	while (src[end_idx] != '\n') {
		fputc(src[end_idx], stdout);
		end_idx++;
	}
	fputc('\n', stdout);

	for (int i = 0; i < err_token->start_idx - beg_idx + 4; i++) {
		fputc(' ', stdout);
	}
	printf(ANSI_COLOR_GREEN "^" ANSI_COLOR_RESET "\n");
}




// advances toks->idx to the index of the next statement's beginning symbol
void panic_mode(struct tokens* toks) {
	struct token* cur_tok = toks->tokens[toks->idx];

	while (!(cur_tok->type >= 100 && cur_tok->type <= 116)) {
		cur_tok = toks->tokens[++toks->idx];
	}

	if (cur_tok->type == SEMI_COLON) {
		toks->idx++;
	}
}




struct token* expect(enum token_type expected_tok, struct token* cur_tok, struct tokens* toks, char* src, char* err_msg) {
	/*
	struct token* tok_to_ret = cur_tok;
	if (cur_tok->type != expected_tok) {
		print_error(cur_tok, src, err_msg);
		panic_mode(toks);
		// *error = 1;
	} else {
		toks->idx++;
	}

	return ret_tok
	*/
}


// dont forget about pointers
// if scope = 0, then only struct and functions should be allowed to be parsed

struct stmt* parse_stmt(struct tokens* toks, char* src, int check_term_sym, int scope, int* error) {
	struct token* cur_tok = toks->tokens[toks->idx++];

	switch (cur_tok->type) {
		case STRUCT: {
			struct _stmt* struct_def = malloc(sizeof(struct _struct));

			// ERROR
			if (scope != 0) {
				print_error(cur_tok, src, "cannot define a struct in a local context");
				panic_mode(toks);
				*error = 1;
			}


			// HERE

			expect(IDENTIFIER, cur_tok, toks, src, "expected an identifier for struct definition");
			// struct_def->id = cur_tok
			expect(LEFT_BRACE, cur_tok, toks, src, "expected a '{' after struct identifier");



			// ensure that parsed_stmts are variable declarations, else error
			// while (not '}')
			// parse_stmts
			// expect ('}')



		}
		break;





		// types
		case C8:
		case I32:
		case I64:
		case F32:
		case F64:
		case STRING:
		case VOID:
		break;

		case IF:
		case ELIF:
		break;

		case ELSE:
		break;

		case WHILE:
		break;

		case RETURN:
		break;

		case IDENTIFIER:
		// check ipad for cases
		break;

		case LEFT_BRACE:
		// block statement
		break;


		default:
			print_error(cur_tok, src, "unknown statement");
			panic_mode(toks);
			*error = 1;
		break;
	}

}
















