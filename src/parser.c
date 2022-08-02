
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"





// TODO
void free_stmt(struct stmt* stmt) {
	switch (stmt->type) {
		case _STRUCT: {
			struct _struct* _struct = stmt->content;
			for (int i = 0; i < _struct->n_fields; i++) {
				struct _vardecl* field = _struct->fields[i];
				free_expr(field->value);
				free(field);
			}
			free(_struct);
		}
		break;

		default:
		break;
	}

	free(stmt);
}



// TODO
void free_expr(struct expr* expr) {
}




struct program* syntax_analysis(struct tokens* tokens, char* src) {
	struct program* program = malloc(sizeof(struct program));
	program->stmts = malloc(sizeof(struct stmt*) * 512);
	program->n_stmts = 512;

	int capacity = 512;
	int global_error = 0;
	int local_error = 0;

	while (tokens->tokens[tokens->idx]->type != EOFF) {
		if (program != NULL) {
			if (program->n_stmts == capacity) {
				program->stmts = realloc(program->stmts, sizeof(struct stmt*) * capacity * 2);
				capacity *= 2;
			}
		}
	
		local_error = 0;
		struct stmt* parsed_stmt = parse_stmt(tokens, src, &local_error, &global_error, DONT_EXPECT_BLOCK);

		if (global_error) {
			if (program != NULL) {
				for (int i = 0; i < program->n_stmts; i++) {
					free_stmt(program->stmts[i]);
				}
				free(program->stmts);
				free(program);
				program = NULL;
			}
		} else {
			if (program->n_stmts == capacity) {
				program->stmts = realloc(program->stmts, sizeof(struct stmt*) * capacity * 2);
				capacity *= 2;
			}
			program->stmts[program->n_stmts++] = parsed_stmt;
		}
	}

	return program;
}



void print_error(struct token* err_token, char* src, char* err_msg) {
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


void panic(struct tokens* toks, enum token_type lbnd, enum token_type ubnd, int* local_error, int* global_error) {
	struct token* tok = toks->tokens[toks->idx];
	*local_error = *global_error = 1;

	while ((tok->type < lbnd || tok->type > ubnd) && tok->type != EOFF) {
		tok = toks->tokens[++toks->idx];
	}

	if (tok->type == SEMI_COLON) {
		toks->idx++;
	}
}

void expect(enum token_type expect_lbnd, enum token_type expect_ubnd, struct token* cur_tok, struct tokens* toks, char* src, 
	enum token_type err_lbnd, enum token_type err_ubnd, int* local_error, int* global_error, char* err_msg) {

	if (cur_tok->type < expect_lbnd || cur_tok->type > expect_ubnd)  {
		print_error(cur_tok, src, err_msg);
		panic(toks, err_lbnd, err_ubnd, local_error, global_error);
	}
}


struct stmt* parse_stmt(struct tokens* toks, char* src, int* local_error, int* global_error, int expect_block) {
	struct token* tok = toks->tokens[toks->idx++];
	struct stmt* stmt = NULL;

	switch (tok->type) {

		case STRUCT: {
			struct _struct* _struct = NULL;
			struct token* id = toks->tokens[toks->idx++];
			expect(IDENTIFIER, IDENTIFIER, tok, toks, src, 116, 117, local_error, global_error, "expected an identifier for struct definition"); 

			if (!*local_error) {
				struct token* left_brace = toks->tokens[toks->idx++];
				expect(LEFT_BRACE, LEFT_BRACE, left_brace, toks, src, 116, 117, local_error, global_error, "expected a '{'");

				if (!*local_error) {
					if (!*global_error) {
						_struct = malloc(sizeof(struct _struct));
						_struct->fields = malloc(sizeof(struct _vardecl*) * 8);
						_struct->n_fields = 0;
						_struct->capacity = 8;
					}

					tok = toks->tokens[toks->idx];

					// FIELD
					if (tok->type != RIGHT_BRACE) {
						do {
							struct token* id = toks->tokens[toks->idx++];
							expect(IDENTIFIER, IDENTIFIER, id, toks, src, 116, 117, local_error, global_error, "expected an identifier for struct field");
							
							if (!*local_error) {
								expect(COLON, COLON, toks->tokens[toks->idx++], toks, src, 116, 117, local_error, global_error, "expected a ':'");

								if (!*local_error) {
									int n_indirect = 0;
									struct token* type = toks->tokens[toks->idx++];
									expect(IDENTIFIER, VOID, type, toks, src, 116, 117, local_error, global_error, "invalid variable type");
									
									tok = toks->tokens[toks->idx];
									while (tok->type == STAR) {
										n_indirect++;
										tok = toks->tokens[++toks->idx];
									}

									if (!*global_error) {
										if (_struct->n_fields == _struct->capacity) {
											_struct->fields = realloc(_struct->fields, sizeof(struct _vardecl*) * _struct->capacity * 2);
											_struct->capacity *= 2;
										}
										struct _vardecl* field = _struct->fields[_struct->n_fields++] = malloc(sizeof(struct _vardecl));
										field->value = NULL;
										field->type = type;
										field->id = id;
										field->n_indirect = n_indirect;
									}
								}
							}

						} while (tok->type == COMMA && !*local_error);
					}

					// ERROR
					if (*local_error && _struct != NULL) {
						for (int i = 0; i < _struct->n_fields; i++) {
							free(_struct->fields[i]);
						}
						free(_struct->fields);
						free(_struct);
					}

					
					// UNCLOSED 
					tok = toks->tokens[toks->idx];
					if (tok->type == EOFF || tok->type == STRUCT || tok->type == FUNCTION) {
						*local_error = *global_error = 1;
						print_error(left_brace, src, "unclosed '{'");
					} else {
						expect(RIGHT_BRACE, RIGHT_BRACE, tok, toks, src, 116, 117, local_error, global_error, "expected a '}'");
					}

					if (!*global_error) {
						stmt = malloc(sizeof(struct stmt));
						stmt->content = _struct;
						stmt->type = _STRUCT;
					}
				}
			}
		}
		break;



		case FUNCTION:
		break;

		case RIGHT_BRACE:
		break;

		case LET:
		break;

		case DECL:
		break;

		case IDENTIFIER:
		break;

		default:
		break;
	}


	return stmt;
}

