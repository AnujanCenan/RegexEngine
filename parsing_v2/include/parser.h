#ifndef PARSER_H
#define PARSER_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Structures
typedef enum  
{
    ST_Grouped_Expr,
    ST_Single_Char,
    ST_Escape_Char
} Simple_Expr_Type;

typedef struct Expr Expr;
typedef struct Or_Expr Or_Expr;
typedef struct Sub_Expr Sub_Expr;
typedef struct Star_Expr Star_Expr;
typedef struct Simple_Expr Simple_Expr;

struct Expr
{
    Sub_Expr* sub;
    Or_Expr* or_expr;
};

struct Or_Expr
{
    bool is_empty;
    Expr* expr;

};

struct Sub_Expr
{
    bool is_empty;
    Star_Expr* star;
    Sub_Expr* sub;
};

struct Star_Expr
{
    Simple_Expr* simple;
    bool has_star;
};

struct Simple_Expr
{
    Simple_Expr_Type type;
    Expr* expr;
    char* spelling;
};

// Parsing function
Expr* parse_expr(char* regex, int regex_len, int* ptr);

// Printing function
void print_expr(Expr* expr, int generation);

// Clean up 
void free_expr(Expr* expr);

#endif