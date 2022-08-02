#ifndef LEXER_H
#define LEXER_H

#define N_KEYWORDS 17
#define KEYWORD_TABLE_SIZE (N_KEYWORDS* 2)
#define IS_NUMERIC(c) ('0' <= c && c <= '9')
#define IS_ALPHA(c) (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_')


enum token_type
{

    // BINARY 

    ARROW, DOT, LEFT_BRACK,  // 110

    STAR, SLASH, MODULUS,    // 100

    PLUS, MINUS,             // 90

    BIT_SHIFTR, BIT_SHIFTL,  // 80

    GT, LT, GTEQ, LTEQ,      // 70

    EQUAL, NOT_EQUAL,        // 60

    BIT_AND,                 // 50

    BIT_OR,                  // 40

    BIT_XOR,                 // 30

    LOG_AND,                 // 20

    LOG_OR,                  // 10


    // UNARY (ADDRESS_OF, VALUE_AT_ADDRESS)
    BIT_NOT,
    LOG_NOT,

    // SYMBOLS
    LEFT_PAREN, RIGHT_PAREN, RIGHT_BRACK,
    COMMA, LEFT_BRACE, SEMI_COLON, COLON, ASSIGN, RIGHT_BRACE = 115,

    // LITERALS
    CHARACTER, INTEGER, LONG, FLOAT, DOUBLE,
    STRING_LITERAL, NONE, IDENTIFIER = 100,

    // TYPES
    C8 = 101, I32 = 102, I64 = 103, F32 = 104, F64 = 105, STRING = 106, VOID = 107,

    // STATEMENTS
	LET = 108, DECL = 109, IF = 110, ELIF = 111, ELSE = 112, WHILE = 113, RETURN = 114, STRUCT = 116, FUNCTION = 117,
    EOFF
};


struct token
{
    enum token_type type;
    int line;
    int start_idx;
    int end_idx;
    int lbp;
};


struct tokens
{
    struct token** tokens;
    int n_tokens;
    int capacity;

	int idx;
};

struct keyword_entry
{
    int occupied;
    enum token_type type;
    char* keyword;
};


void construct_keyword_table(struct keyword_entry* keyword_table);
struct tokens* lexical_analysis(char* src, long len);
int compute_hash(char* s_idx, int len);
void add_token(struct tokens* tokens, enum token_type type, int s_idx, int e_idx, int line, int lbp);
void read_token(struct tokens* tokens, struct keyword_entry* keyword_table, char* src, int* s_idx, int* e_idx, int* line);





#endif



