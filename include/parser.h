
#ifndef PARSER_H
#define PARSER_H




#include "lexer.h"


#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"


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
	_FUNCTION 
};

struct stmt {
	enum stmt_type type;
	void* content;
};

struct _struct {
	struct token* id;
	struct stmt** fields;
	int n_fields;
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

struct _block {
	struct stmt** stmts;
	int n_stmts;
};


struct _if {
	struct expr* cond;
	struct stmt* body;
	struct _if** _elifs;
	int n_elifs;
	struct stmt* _else;
};

struct _return {
	struct token* lexeme;
	struct expr* value;
};

struct _function {
	struct token* id;
	struct token* return_type;
	struct stmt** params;
	int n_params;
	struct stmt* body;

	// information for type checker 
	//char* ret_type_repr;
};

struct _vardecl {
	struct token* type;
	struct token* id;
	struct expr* value;
	int n_indirect;
	int array_literal;
	// information for type checker
};

struct program {
	struct stmt** stmts;
	int n_stmts;
};



struct program* syntax_analysis(struct tokens* tokens, char* src);
void print_err(struct token* err_token, char* src, char* err_msg);
void panic_mode(struct tokens* toks);
struct stmt* parse_def(struct tokens* toks, char* src);










































#endif
