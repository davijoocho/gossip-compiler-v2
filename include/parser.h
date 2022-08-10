
#ifndef PARSER_H
#define PARSER_H




#include "lexer.h"

#define BOLD "\x1B[1m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"


#define EXPECT_BLOCK 1
#define DONT_EXPECT_BLOCK 0

// SUBJECT TO CHANGE BECAUSE OF RELOCATION INFORMATION IN COMPILING STAGE AND OTHER INFORMATION IN THE TYPE CHECKING PHASE

enum expr_type {
	BINARY,
	UNARY,
	LITERAL,
	CALL,
	VARIABLE,
	ARRAY_LITERAL
};

struct expr {
	enum expr_type type;
	void* content;

	// information for type checker
	char* type_repr;
	char* struct_id;
	enum token_type evaluate_to;
	int error_detected;
	int n_indirect;
	int reg_occupied;
};


struct binary {
	struct token* op;
	struct expr* left;
	struct expr* right;
};

struct unary {
	struct token* op;
	struct expr* right;
    int n_indirect;       // for type casts
};

struct literal {
	struct token* value;
};

struct array_literal {
	struct expr** literals;
	int n_literals;
};

struct variable {
	struct token* id;
};

struct call {
	struct token* id;
	struct expr** args;
	int n_args;
};


enum stmt_type { 
	_STRUCT, 
	_WHILE, 
	_ASSIGN, 
	_PROCEDURE, 
	_BLOCK, 
	_IF, 
	_RETURN, 
	_FUNCTION,
	_VAR_DECL
};

struct stmt {
	enum stmt_type type;
	void* content;
};

struct _block {
	struct stmt** stmts;
	int n_stmts;
    int capacity;
};

struct _struct {
	struct token* id;
	struct _vardecl** fields;
	int n_fields;
	int capacity;
};

struct _while {
	struct expr* cond;
	struct stmt* body;
};

struct _assign {
	struct expr* lhv;
	struct expr* rhv;
};

struct _procedure {
	struct token* id;
	struct expr** args;
	int n_args;
};

struct _if {
	struct expr* cond;
	struct stmt* body;
	struct _if** branches;
	int n_branches;
	struct stmt* _else;
};

struct _return {
	struct expr* value;
};

struct _function {
	struct token* id;
	struct token* ret_type;
	int n_indirect;

	struct _vardecl** params;
	int n_params;
    int capacity;

	struct stmt* body;

	// information for type checker + compiler
};

struct _vardecl {
	struct token* type;
	struct token* id;
	struct expr* value;
	int n_indirect;
	int array_literal;
	// information for type checker + compiler
};


struct program {
	struct stmt** stmts;
	int n_stmts;
};


struct program* syntax_analysis(struct tokens* tokens, char* src);
void print_error(struct token* err_token, struct tokens* toks, char* src, char* err_msg);
void panic(struct tokens* toks, enum token_type lbnd, enum token_type ubnd, int* local_error, int* global_error);
void expect(enum token_type expect_lbnd, enum token_type expect_ubnd, struct token* cur_tok, struct token* err_token, struct tokens* toks, char* src, 
	enum token_type err_lbnd, enum token_type err_ubnd, int* local_error, int* global_error, char* err_msg);

struct stmt* parse_stmt(struct tokens* toks, char* src, int* local_error, int* global_error, int expect_block);
void free_stmt(struct stmt* stmt);
void free_expr(struct expr* expr);



#endif
