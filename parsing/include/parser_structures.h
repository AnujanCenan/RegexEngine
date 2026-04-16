#ifndef PARSER_STRUCTURES_H
#define PARSER_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ARGV_REGEX_PATTERN 1
#define MAX_STACK_SIZE 500

typedef struct Expr Expr;
typedef struct OrExpr OrExpr;
typedef struct SubExpr SubExpr;
typedef struct RepeatedBlock RepeatedBlock;
typedef struct Block Block;
typedef struct SingleChar SingleChar; 
typedef struct Star Star;
typedef struct EscapeChar EscapeChar;

typedef enum BlockType BlockType;
typedef enum NodeType NodeType;

enum BlockType
{
    BT_SingleCharType, 
    BT_ExpressionType
};


enum NodeType
{
    NT_Expr,
    NT_OrExpr,
    NT_SubExpr,
    NT_RepBlock,
    NT_Block,
    NT_SingleChar,
    NT_Star,
    NT_Escape
};

struct Expr
{
    SubExpr* sub;
    OrExpr* or_expr;
};

struct OrExpr
{
    bool is_empty;
    Expr* expr;
};

struct SubExpr
{
    bool is_empty;
    RepeatedBlock* repeated_block;
    SubExpr* sub_expr;
};

struct RepeatedBlock
{
    Block* block;
    Star* star;
};


struct Block
{
    BlockType type;
    void* data;
};

struct SingleChar
{
    bool is_escape;
    char char_data;
    EscapeChar* escape_char;
};

struct Star
{
    bool is_empty;
};

struct EscapeChar
{
    char char_data;
};

#endif