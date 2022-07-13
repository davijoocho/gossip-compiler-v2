
#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int compute_hash(char* s_idx, int len)
{
    int hash = 0;
    for (int i = 0; i < len; i++) {
        hash += s_idx[i];
    }
    return hash % KEYWORD_TABLE_SIZE;
}

void add_token(struct tokens* tokens, enum token_type type, int s_idx, int e_idx, int line, int lbp) {
    if (tokens->n_tokens == tokens->capacity) {
        tokens->tokens = realloc(tokens->tokens, sizeof(struct tokens*) * tokens->capacity * 2);
        tokens->capacity *= 2;
    }

    struct token* token = malloc(sizeof(struct token));
    token->type = type;
    token->lbp = lbp;
    token->line = line;
    token->start_idx = s_idx;
    token->end_idx = e_idx;

    tokens->tokens[tokens->n_tokens++] = token;
}


void read_token(struct tokens* tokens, struct keyword_entry* keyword_table, char* src, int* s_idx, int* e_idx, int* line)
{
    char c = src[*e_idx];
	*e_idx += 1;

    switch (c)
    {
        case '.': add_token(tokens, DOT, *s_idx, *e_idx, *line, 110); break;
        case '*': add_token(tokens, STAR, *s_idx, *e_idx, *line, 100); break;
        case '%': add_token(tokens, MODULUS, *s_idx, *e_idx, *line, 100); break;
        case '[': add_token(tokens, LEFT_BRACK, *s_idx, *e_idx, *line, 110); break;
        case ']': add_token(tokens, RIGHT_BRACK, *s_idx, *e_idx, *line, 0); break;
        case '(': add_token(tokens, LEFT_PAREN, *s_idx, *e_idx, *line, 0); break;
        case ')': add_token(tokens, RIGHT_PAREN, *s_idx, *e_idx, *line, 0); break;
        case '{': add_token(tokens, LEFT_BRACE, *s_idx, *e_idx, *line, 0); break;
        case '}': add_token(tokens, RIGHT_BRACE, *s_idx, *e_idx, *line, 0); break;
        case ',': add_token(tokens, COMMA, *s_idx, *e_idx, *line, 0); break;
        case '~': add_token(tokens, BIT_NOT, *s_idx, *e_idx, *line, 0); break;
        case '^': add_token(tokens, BIT_XOR, *s_idx, *e_idx, *line, 30); break;
        case ';': add_token(tokens, SEMI_COLON, *s_idx, *e_idx, *line, 0); break;
        case '+': add_token(tokens, PLUS, *s_idx, *e_idx, *line, 90); break;
        case '\n': 
			*line += 1; 
			break;
        case ' ':
        case '\t':
		case '\r':
            break;

        case '|': 
            if (src[*e_idx] == '|') {
				*e_idx += 1;
                add_token(tokens, LOG_OR, *s_idx, *e_idx, *line, 10);
            } else {
                add_token(tokens, BIT_OR, *s_idx, *e_idx, *line, 40);
            }
            break;

        case '&':
            if (src[*e_idx] == '&') {
				*e_idx += 1;
                add_token(tokens, LOG_AND, *s_idx, *e_idx, *line, 20);
            } else {
                add_token(tokens, BIT_AND, *s_idx, *e_idx, *line, 50);
            }
            break;

        case '!':
            if (src[*e_idx] == '=') {
				*e_idx += 1;
                add_token(tokens, NOT_EQUAL, *s_idx, *e_idx, *line, 60);
            } else {
                add_token(tokens, LOG_NOT, *s_idx, *e_idx, *line, 0);
            }
            break;
            
        case '=':
            if (src[*e_idx] == '=') {
				*e_idx += 1;
                add_token(tokens, EQUAL, *s_idx, *e_idx, *line, 60);
            } else {
                add_token(tokens, ASSIGN, *s_idx, *e_idx, *line, 0);
            }
            break;

        case '<':
            if (src[*e_idx] == '=') {
				*e_idx += 1;
                add_token(tokens, LTEQ, *s_idx, *e_idx, *line, 70);
            } else if (src[*e_idx] == '<') {
				*e_idx += 1;
                add_token(tokens, BIT_SHIFTL, *s_idx, *e_idx, *line, 80);
            } else {
                add_token(tokens, LT, *s_idx, *e_idx, *line, 70);
            }
            break;

        case '>':
            if (src[*e_idx] == '=') {
				*e_idx += 1;
                add_token(tokens, GTEQ, *s_idx, *e_idx, *line, 70);
            } else if (src[*e_idx] == '>') {
				*e_idx += 1;
                add_token(tokens, BIT_SHIFTR, *s_idx, *e_idx, *line, 80);
            } else {
                add_token(tokens, GT, *s_idx, *e_idx, *line, 70);
            }
            break;



        case '\'':
            // potential error: unclosed character
            while (src[*e_idx] != '\'') {
                if (src[*e_idx] == '\\') {
                    *e_idx += 1;
                }
                *e_idx += 1;
            }
            *e_idx += 1;
            add_token(tokens, CHARACTER, *s_idx, *e_idx, *line, 0);
            break;

        case '"':
            // potential error: unclosed string
            while (src[*e_idx] != '"') {
                if (src[*e_idx] == '\\') {
                    *e_idx += 1;
                }
                *e_idx += 1;
            }
            *e_idx += 1;
            add_token(tokens, STRING_LITERAL, *s_idx, *e_idx, *line, 0);
            break;

        case '/':
            if (src[*e_idx] == '/') {
                *e_idx += 1;
                while (src[*e_idx] != '\n') {
                    *e_idx += 1;
                }
            } else if (src[*e_idx] == '*') {
                *e_idx += 1;
                int stop = 0;

                while (!stop) {
                    if (src[*e_idx] == '*') {
                        *e_idx += 1;
                        if (src[*e_idx] == '/') {
                            *e_idx += 1;
                            stop = 1;
                        }
                    } else {
                        if (src[*e_idx] == '\n') {
                            *line += 1;
                        }
                        *e_idx += 1;
                    }
                }

            } else {
                add_token(tokens, SLASH, *s_idx, *e_idx, *line, 100);
            }
            break;

        case '-':
            if (src[*e_idx] == '>') { 
				*e_idx += 1;
                add_token(tokens, ARROW, *s_idx, *e_idx, *line, 110);
            } else if (IS_NUMERIC(src[*e_idx])) {
                // potential error: more than 1 decimal in a number, decimal but no F or D, floating-point but L
                
                while (IS_NUMERIC(src[*e_idx]) || (src[*e_idx] == '.')) {
                    *e_idx += 1;
                }

                if (src[*e_idx] == 'F') {
					*e_idx += 1;
                    add_token(tokens, FLOAT, *s_idx, *e_idx, *line, 0);
                } else if (src[*e_idx] == 'D') {
					*e_idx += 1;
                    add_token(tokens, DOUBLE, *s_idx, *e_idx, *line, 0);
                } else if (src[*e_idx] == 'L') {
					*e_idx += 1;
                    add_token(tokens, LONG, *s_idx, *e_idx, *line, 0);
                } else {
                    add_token(tokens, INTEGER, *s_idx, *e_idx, *line, 0);
                }

            } else {
                add_token(tokens, MINUS, *s_idx, *e_idx, *line, 90);
            }

            break;

        default:
            if (IS_NUMERIC(c)) {
                // potential error: more than 1 decimal in a number, decimal but no F or D, floating-point but L
                while (IS_NUMERIC(src[*e_idx]) || (src[*e_idx] == '.')) {
                    *e_idx += 1;
                }

                if (src[*e_idx] == 'F') {
					*e_idx += 1;
                    add_token(tokens, FLOAT, *s_idx, *e_idx, *line, 0);
                } else if (src[*e_idx] == 'D') {
					*e_idx += 1;
                    add_token(tokens, DOUBLE, *s_idx, *e_idx, *line, 0);
                } else if (src[*e_idx] == 'L') {
					*e_idx += 1;
                    add_token(tokens, LONG, *s_idx, *e_idx, *line, 0);
                } else {
					*e_idx += 1;
                    add_token(tokens, INTEGER, *s_idx, *e_idx, *line, 0);
                }

            } else if (IS_ALPHA(c)) {

                while (IS_NUMERIC(src[*e_idx]) || IS_ALPHA(src[*e_idx])) {
                    *e_idx += 1;
                }

                int idx = compute_hash(src + *s_idx, *e_idx - *s_idx);
                struct keyword_entry* entry = &keyword_table[idx];

                while (entry->occupied) {
                    if (strncmp(entry->keyword, src + *s_idx, *e_idx - *s_idx) == 0) {
                        add_token(tokens, entry->type, *s_idx, *e_idx, *line, 0);
                        return;
                    }

                    if (++idx == KEYWORD_TABLE_SIZE) {
                        idx = 0;
                    }
                    entry = &keyword_table[idx];
                }

                add_token(tokens, IDENTIFIER, *s_idx, *e_idx, *line, 0);
            }
            break;
    }
}

void construct_keyword_table(struct keyword_entry* keyword_table)
{
    memset(keyword_table, 0, sizeof(struct keyword_entry) * KEYWORD_TABLE_SIZE);
    char* keywords[] = {"None", "c8", "i32", "i64", "f32", "f64", "void", "string", "fn", "if", "elif", "else", "while", "ret", "struct"};
    enum token_type types[] = {NONE, C8, I32, I64, F32, F64, VOID, STRING, FUNCTION, IF, ELIF, ELSE, WHILE, RETURN, STRUCT}; 

    for (int i = 0; i < N_KEYWORDS; i++) {
        char* keyword = keywords[i];
        int idx = compute_hash(keyword, strlen(keyword));

        struct keyword_entry* entry = &keyword_table[idx];
        while (entry->occupied) {
            if (++idx == KEYWORD_TABLE_SIZE) {
                idx = 0;
            }
            entry = &keyword_table[idx];
        }

        entry->occupied = 1;
        entry->type = types[i];
        entry->keyword = keyword;
    }
}


struct tokens* lexical_analysis(char* src, long len)
{
    struct tokens* tokens = malloc(sizeof(struct tokens));
    tokens->tokens = malloc(sizeof(struct token*) * 1024);
    tokens->n_tokens = 0;
    tokens->capacity = 1024;
	tokens->idx = 0;

    struct keyword_entry keyword_table[KEYWORD_TABLE_SIZE];
    construct_keyword_table(keyword_table);

    int s_idx = 0;
    int e_idx = 0;
    int line = 1;

    while (s_idx < len) {
        read_token(tokens, keyword_table, src, &s_idx, &e_idx, &line);
        s_idx = e_idx;
    }

    add_token(tokens, EOFF, 0, 0, 0, 0);

    return tokens;
}

