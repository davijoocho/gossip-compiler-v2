#ifndef LEXER_H
#define LEXER_H

#define N_KEYWORDS 14
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
    COMMA, LEFT_BRACE = 115, RIGHT_BRACE, SEMI_COLON = 113, 

    // LITERALS
    CHARACTER, INTEGER, LONG, FLOAT, DOUBLE, IDENTIFIER = 116,
    STRING_LITERAL, NONE,

    // TYPES
    C8 = 100, I32 = 101, I64 = 102, F32 = 103, F64 = 104, STRING = 105, VOID = 106,

    // STATEMENTS
    STRUCT = 107, IF = 108, ELIF = 109, ELSE = 110, WHILE = 111, RETURN = 112, ASSIGN = 200, // it was just important to assign ASSIGN a number not within the range 100 - 115 because it isn't a terminating symbol

    EOFF = 114
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



