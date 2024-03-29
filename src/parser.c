
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


// DONT SUPPORT STACK/ LITERAL ARRAYS YET 




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



// TODO 5
void free_expr(struct expr* expr) {
}




struct program* syntax_analysis(struct tokens* tokens, char* src) {
	struct program* program = malloc(sizeof(struct program));
	program->stmts = malloc(sizeof(struct stmt*) * 512);
	program->n_stmts = 0;

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



void print_error(struct token* err_token, struct tokens* toks, char* src, char* err_msg) {
	int line = 1;
	int beg_idx = 0;
	int end_idx = 0;
	int skip = 4;
    int indicator = err_token->start_idx;

	if (err_token->type == EOFF) {
		skip = 5;
		err_token = toks->tokens[toks->idx - 1];
        indicator = err_token->end_idx;
	} 



	printf(BOLD ANSI_COLOR_RED "ERROR "  ANSI_COLOR_RESET BOLD "(LINE %d): %s\n", err_token->line, err_msg);

	// find index to beginning of line with error
	while (line != err_token->line) {
		if (src[beg_idx] == '\n') {
			line++;
		}
		beg_idx++;
	}

	// find index to end of line with error
	printf("    ");
	end_idx = beg_idx;
	while (src[end_idx] != '\n') {
		fputc(src[end_idx], stdout);
		end_idx++;
	}
	fputc('\n', stdout);

	for (int i = 0; i < indicator - beg_idx + skip; i++) {
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
}

void expect(enum token_type expect_lbnd, enum token_type expect_ubnd, struct token* cur_tok, struct token* err_token, struct tokens* toks, char* src, 
	enum token_type err_lbnd, enum token_type err_ubnd, int* local_error, int* global_error, char* err_msg) {

	if (cur_tok->type < expect_lbnd || cur_tok->type > expect_ubnd)  {
		print_error(err_token, toks, src, err_msg);
		panic(toks, err_lbnd, err_ubnd, local_error, global_error);
		return;
	}
	toks->idx++;
}


// blocks panic to '}'
// local_error needs to reset

struct stmt* parse_stmt(struct tokens* toks, char* src, int* local_error, int* global_error, int expect_block) {
	struct token* tok = toks->tokens[toks->idx];
	struct stmt* stmt = NULL;
    *local_error = 0;

    if (expect_block) {
        expect(LEFT_BRACE, LEFT_BRACE, tok, tok, toks, src, IDENTIFIER, FUNCTION, local_error, global_error, "expected a '{'");
        if (*local_error) {
            return NULL;
        }
    }

	switch (tok->type) {

            case STRUCT: {
                toks->idx++;
                struct _struct* _struct = NULL;
                struct token* id = toks->tokens[toks->idx];
                expect(IDENTIFIER, IDENTIFIER, id, id, toks, src, STRUCT, FUNCTION, local_error, global_error, "expected an identifier for struct definition"); 

                if (!*local_error) {
                    struct token* left_brace = toks->tokens[toks->idx];
                    expect(LEFT_BRACE, LEFT_BRACE, left_brace, left_brace, toks, src, STRUCT, FUNCTION, local_error, global_error, "expected a '{'");

                    if (!*local_error) {
                        if (!*global_error) {
                            _struct = malloc(sizeof(struct _struct));
                            _struct->fields = malloc(sizeof(struct _vardecl*) * 8);
                            _struct->n_fields = 0;
                            _struct->capacity = 8;
                        }

                        tok = toks->tokens[toks->idx];

                        if (tok->type != RIGHT_BRACE) {
							int is_comma = 0;
                            do {
								is_comma = 0;
                                struct token* field_id = toks->tokens[toks->idx];
                                expect(IDENTIFIER, IDENTIFIER, field_id, field_id, toks, src, RIGHT_BRACE, RIGHT_BRACE, local_error, global_error, "expected a struct field");
                                
                                if (!*local_error) {
                                    expect(COLON, COLON, toks->tokens[toks->idx], toks->tokens[toks->idx], toks, src, RIGHT_BRACE, RIGHT_BRACE, local_error, global_error, "expected a ':'");

                                    if (!*local_error) {
                                        int n_indirect = 0;
                                        struct token* type = toks->tokens[toks->idx];
                                        expect(IDENTIFIER, VOID, type, type, toks, src, RIGHT_BRACE, RIGHT_BRACE, local_error, global_error, "invalid variable type");

                                        if (!*local_error) {
                                            tok = toks->tokens[toks->idx];
                                            while (tok->type == STAR) {
                                                n_indirect++;
                                                tok = toks->tokens[++toks->idx];

                                            }

                                            if (tok->type == COMMA) {
                                                is_comma = 1;
                                                toks->idx++;
                                            }
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

                            } while (is_comma && !*local_error);

                            // ACCOMODATE THE FACT THAT NONEXISTENCE OF COMMA DOESNT PANIC TO RIGHT BRACE
                            tok = toks->tokens[toks->idx];
                            if (tok->type != RIGHT_BRACE) {
                                print_error(tok, toks, src, "expected a '}'");
                                panic(toks, RIGHT_BRACE, RIGHT_BRACE, local_error, global_error);
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
                        expect(RIGHT_BRACE, RIGHT_BRACE, tok, left_brace, toks, src, EOFF, EOFF, local_error, global_error, "unclosed '{'");


                        if (!*global_error) {
                            stmt = malloc(sizeof(struct stmt));
                            stmt->content = _struct;
                            stmt->type = _STRUCT;
                        }


                    }
                }
            }
		}
		break;



		case FUNCTION: {
            toks->idx++;
            struct _function* fn = NULL;
            struct token* id = toks->tokens[toks->idx];
            expect(IDENTIFIER, IDENTIFIER, id, id, toks, src, STRUCT, FUNCTION, local_error, global_error, "expected an identifier for function definition");

            // IDENTIFIER
            if (!*local_error) {
                tok = toks->tokens[toks->idx];
                expect(LEFT_PAREN, LEFT_PAREN, tok, tok, toks, src, STRUCT, FUNCTION, local_error, global_error, "expected a '('");

                // LEFT_PAREN
                if (!*local_error) {
                    if (!*global_error) {
                        fn = malloc(sizeof(struct _function));
                        fn->params = malloc(sizeof(struct _vardecl*) * 8);
                        fn->n_params = 0;
                        fn->capacity = 8;
                    }


					// PARAMETERS
                    tok = toks->tokens[toks->idx];
                    if (tok->type != RIGHT_PAREN && tok->type != EOFF) {
                        int is_comma = 0;
                        do {
                            is_comma = 0;
                            struct token* param_id = toks->tokens[toks->idx];
                            expect(IDENTIFIER, IDENTIFIER, param_id, param_id, toks, src, STRUCT, FUNCTION, local_error, global_error, "expected an identifier for function parameter");

                            if (!*local_error) {
                                expect(COLON, COLON, toks->tokens[toks->idx], toks->tokens[toks->idx], toks, src, STRUCT, FUNCTION, local_error, global_error, "expected a ':'");

                                if (!*local_error) {
									int n_indirect = 0;
									struct token* param_type = toks->tokens[toks->idx];
									expect(IDENTIFIER, VOID, param_type, param_type, toks, src, STRUCT, FUNCTION, local_error, global_error, "invalid variable type");

									if (!*local_error) {
										tok = toks->tokens[toks->idx];
										while (tok->type == STAR) {
											n_indirect++;
											tok = toks->tokens[++toks->idx];
										}

										if (tok->type == COMMA) {
											toks->idx++;
											is_comma = 1;
										}
									}

									if (!*global_error) {
										if (fn->n_params == fn->capacity) {
											fn->params = realloc(fn->params, sizeof(struct _vardecl*) * fn->capacity * 2);
											fn->capacity *= 2;
										}
										struct _vardecl* param = fn->params[fn->n_params++] = malloc(sizeof(struct _vardecl));
										param->value = NULL;
										param->type = param_type;
										param->id = param_id;
										param->n_indirect = n_indirect;
									}
                                }
                            }
                        } while (is_comma && !*local_error);

						if (*local_error && fn != NULL) {
							for (int i = 0; i < fn->n_params; i++) {
								free(fn->params[i]);
							}
							free(fn->params);
							free(fn);
						}
					}



					if (!*local_error) {
						expect(RIGHT_PAREN, RIGHT_PAREN, toks->tokens[toks->idx], toks->tokens[toks->idx], toks, src, STRUCT, FUNCTION, local_error, global_error, "expected a ')'");
					}


					// RETURN TYPE
					if (!*local_error) {
						expect(ARROW, ARROW, toks->tokens[toks->idx], toks->tokens[toks->idx], toks, src, STRUCT, FUNCTION, local_error, global_error, "expected a '->'");

						if (!*local_error) {
							int n_indirect = 0;
							struct token* ret_type = toks->tokens[toks->idx];
							expect(IDENTIFIER, VOID, ret_type, ret_type, toks, src, STRUCT, FUNCTION, local_error, global_error, "invalid return type");

							if (!*local_error) {
								tok = toks->tokens[toks->idx];
								while (tok->type == STAR) {
									n_indirect++;
									tok = toks->tokens[++toks->idx];
								}
							}

							if (!*global_error) {
								fn->ret_type = ret_type;
								fn->n_indirect = n_indirect;
							}
						}
					}

					// BODY
                    struct stmt* body = NULL;
                    if (!*local_error) {
                        body = parse_stmt(toks, src, local_error, global_error, EXPECT_BLOCK);
                    }


					if (!*global_error) {
						fn->body = body;
						stmt = malloc(sizeof(struct stmt));
						stmt->content = fn;
						stmt->type = _STRUCT;
					}
                }
            }
        }
		break;

		case LEFT_BRACE: {
            struct token* left_brace = tok;
            if (!expect_block) {
                print_error(left_brace, toks, src, "invalid placement of '{'");
                panic(toks, RIGHT_BRACE, RIGHT_BRACE, local_error, global_error);

                tok = toks->tokens[toks->idx];
                if (tok->type != RIGHT_BRACE) {
                    print_error(left_brace, toks, src, "unclosed '{'");
                }
                return NULL;
            }

            struct _block* blk = NULL;
            if (!*global_error) {
                blk = malloc(sizeof(struct _block));
                blk->stmts = malloc(sizeof(struct stmt*) * 16);
                blk->n_stmts = 0;
                blk->capacity = 16;
            }

            tok = toks->tokens[toks->idx];
            if (tok->type != RIGHT_BRACE && tok->type != EOFF) {
                do {
                    struct stmt* parsed_stmt = parse_stmt(toks, src, local_error, global_error, DONT_EXPECT_BLOCK);
                    if (!*global_error) {
                        if (blk->n_stmts == blk->capacity) {
                            blk->stmts = realloc(blk->stmts, sizeof(struct stmt*) * blk->capacity * 2);
                            blk->capacity *= 2;
                        }

                        blk->stmts[blk->n_stmts++] = parsed_stmt;
                    }
                } while (tok->type != RIGHT_BRACE && tok->type != EOFF);
            }
            expect(RIGHT_BRACE, RIGHT_BRACE, tok, left_brace, toks, src, EOFF, EOFF, local_error, global_error, "unclosed '{'");

            if (*local_error && blk != NULL) {
                for (int i = 0; i < blk->n_stmts; i++) {
                    free_stmt(blk->stmts[i]);
                }
                free(blk);
            }

            if (!*global_error) {
                stmt = malloc(sizeof(struct stmt));
                stmt->content = blk;
                stmt->type = _BLOCK;
            }
        }
		break;

        // TODO 4
		case LET: {
            toks->idx++;
            struct _assign* assign = NULL;

            struct expr* lhv = parse_expr(toks, 0, src, local_error, global_error);




        }

            /*
                struct expr* lhv = parse_expr(...);
                if (!*local_error) {
                    expect(
                }



                expect(ASSIGN)
                struct expr* rhv = parse_expr(...);
                expect(SEMI_COLON);
                if (!*global_error) {
                    assign->rhv = rhv
                    assign->lhv = lhv
                    struct->content = assign
                    ...
                }
                struct _assign* assign = malloc(sizeof(struct _assign));
            */
        }
		break;

		case DECL:
        // this must be either stack array or id token -> can be done normally with tokens (LHV)
        // no stack struct or array
		break;

		case IDENTIFIER:
		break;

        case IF:
        // ELSE ELSE IF
        case WHILE:
        case RETURN:

		default: {
            print_error(tok, toks, src, "invalid statement");
            panic(toks, C8, FUNCTION, local_error, global_error);
        }
		break;
	}


	return stmt;
}



struct expr* parse_nud(struct tokens* toks, struct token* tok, char* src, int* local_error, int* global_error) {
    struct expr* expr = NULL;

    switch (tok->type) {
        case BIT_NOT:
        case LOG_NOT:
        case BIT_AND:
        case STAR: {
            toks->idx++;
            struct unary* unary = NULL;
            struct expr* right = parse_expr(toks, 0, src, local_error, global_error);

            if (!*global_error) {
                unary = malloc(sizeof(struct unary));
                unary->op = tok;
                unary->right = right;

                expr = malloc(sizeof(struct expr));
                expr->type = UNARY;
                expr->content = unary;
            }
        }
        break;

        case CHARACTER:
        case INTEGER:
        case LONG:
        case FLOAT:
        case DOUBLE:
        case STRING_LITERAL:
        case NONE: {
            toks->idx++;
            if (!*global_error) {
                struct literal* literal = malloc(sizeof(struct literal));
                literal->value = tok;
                expr = malloc(sizeof(struct expr));
                expr->type = LITERAL;
                expr->content = literal;
            }
        }
        break;


        case LEFT_PAREN: {
            toks->idx++;
            struct expr* group = parse_expr(toks, 0, local_error, global_error);
            if (!*local_error) {
                // this is a problem.
                expect(RIGHT_PAREN, RIGHT_PAREN, toks->tokens[toks->idx], tok, toks, src, local_error, global_error, "expected a ')'");
                if (!*global_error) {
                    struct unary* unary = malloc(sizeof(struct unary));
                    unary->op = tok;
                    unary->right = group;

                    expr = malloc(sizeof(struct expr));
                    expr->type = UNARY;
                    expr->content = group
                }
            }

            


            // either a { or 
            // if () , while (), call (),  F
            // to the next statement?

            // what do I panic to?
            // else  it would have panicked to a terminating symbol
            // in the case of if-stmt, error in condition errors to ) but then what does the if error to ?? to left brace?? yea it does because it is still part of the "stmt"
        }
        break;



        case LT: // type-cast
        break;

        case CHARACTER:
        case INTEGER:
        case LONG:
        case FLOAT:
        case DOUBLE:
        case STRING_LITERAL:
        case NONE:
        break;

        case IDENTIFIER:
        // call
        // variable
        break;


        case LEFT_BRACE:
        // array literal - dont attempt for now
        break;

        default:
        break;
    }


    return expr;
}


// TODO 2
struct expr* parse_led(struct expr* left, struct tokens* toks, struct token* operator, char* src, int* local_error, int* global_error) {
}

// TODO 3
struct expr* parse_expr(struct tokens* toks, int rbp, char* src, int* local_error, int* global_error) {
    struct expr* left = parse_nud(toks, toks->tokens[toks->idx], src, local_error, global_error);

}













