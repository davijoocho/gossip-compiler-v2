
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"



// BAD BUT WHATEVER - WILL NOT FREE MEMORY IN THE CASE OF AN ERROR BECAUSE PROGRAM WILL EXIT ANYWAYS

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
		struct stmt* parsed_stmt = parse_stmt(tokens, src, 0,  &error);
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

	printf(ANSI_COLOR_RED "ERROR " ANSI_COLOR_RESET "(LINE %d): %s\n", err_token->line, err_msg);
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




struct token* expect(enum token_type expected_tok, struct tokens* toks, char* src, int* error, char* err_msg) {
	struct token* check_tok = toks->tokens[toks->idx];
	if (check_tok->type == expected_tok) {
		toks->idx++;
	} else {
		*error = 1;
	}
	print_error(check_tok, src, err_msg);
	panic_mode(toks);
	return check_tok;
}



struct stmt* parse_def(struct tokens* toks, char* src, int* error) {
	// panic to 'struct' or 'fn'
}


struct stmt* parse_stmt(struct tokens* toks, char* src, int* error) {
	// panic to ';' or 'statement keywords: if while ret identifier types'
}



































/*
// if scope = 0, then only struct and functions should be allowed to be parsed
// FOR FUNCTION PARAM PARSING, do a do{} while(nxt == COMMA) stmt 

struct stmt* parse_stmt(struct tokens* toks, char* src, int is_local, int* error) {
 	struct stmt* stmt = malloc(sizeof(struct stmt));
	struct token* cur_tok = toks->tokens[toks->idx++];
	stmt->beg_tok = cur_tok;

	switch (cur_tok->type) {
		case STRUCT: {
			struct _stmt* struct_def = malloc(sizeof(struct _struct));

			if (is_local) {
				print_error(cur_tok, src, "cannot define a struct in a local context");
				panic_mode(toks);
				*error = 1;
			}

			struct_def->id = expect(IDENTIFIER, toks, src, error, "expected an identifier for struct definition");
			expect(LEFT_BRACE, toks, src, error, "expected a '{' after struct identifier");

			struct_def->max_fields = 6;
			struct_def->n_fields = 0;
			struct_def->fields = malloc(sizeof(struct stmt*) * 6);

			while (cur_tok != '}') {
				struct stmt* parsed_stmt = parse_stmt(toks, src, 1, error);

				if (parsed_stmt->type != _VAR_DECL) {
					print_error(parsed_stmt->beg_tok, src, "statements besides variable declarations are not allowed in a struct definition");
					*error = 1;
				} else {
					struct _vardecl* decl = parsed_stmt->content;
					if (decl->value != NULL) {
						print_error(decl->id, src, "cannot initialize struct fields");
						*error = 1;
					}
				}

				expect(SEMI_COLON, toks, src, error, "expected a ';' after struct field");

				if (struct_def->n_fields == struct_def->max_fields) {
					struct_def->fields = realloc(struct_def->fields, sizeof(struct stmt*) * struct_def->max_fields * 2);
					struct_def->max_fields *= 2;
				}

				struct_def->fields[struct_def->n_fields++] = parsed_stmt;
			}



			// if EOFF then unclosed }

			expect(RIGHT_BRACE, toks, src, error, "expected a '}' after struct definition");
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
		// variable declarations, dont forget pointers
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

	return stmt;
}
*/




