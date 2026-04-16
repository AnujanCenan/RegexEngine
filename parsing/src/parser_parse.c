#include "../include/parser_parse.h"

// Function Declarations
OrExpr* parse_or_expression(char** regex_pattern);
SubExpr* parse_sub_expression(char** regex_pattern);
RepeatedBlock* parse_repeated_block(char** regex_pattern);
Block* parse_block(char** regex_pattern);
SingleChar* parse_single_char(char** regex_pattern);
Star* parse_star(char** regex_pattern);
EscapeChar* parse_escape_char(char** regex_pattern);

// Function Definitions
Expr* parse_expression(char** regex_pattern)
{
    SubExpr* s = parse_sub_expression(regex_pattern);
    OrExpr* o = parse_or_expression(regex_pattern);

    Expr* e = malloc(sizeof(Expr));
    
    e->sub = s;
    e->or_expr = o;
    
    return e;
}

OrExpr* parse_or_expression(char** regex_pattern)
{
    OrExpr* o = malloc(sizeof(OrExpr));
    if (**regex_pattern == '|')
    {
        o->is_empty = false;
        (*regex_pattern)++;
        Expr* e = parse_expression(regex_pattern);
        o->expr = e;
    } else
    {
        o->is_empty = true;
        o->expr = NULL;
    }

    return o;
}


SubExpr* parse_sub_expression(char** regex_pattern)
{
    // Recursive version (initial version)
    SubExpr* sub = malloc(sizeof(SubExpr));
    // Had to add \0 to get rid of non-terminating error
    if (**regex_pattern == '|' || **regex_pattern == ')' ||  **regex_pattern == '*' || **regex_pattern == '\0')      
    // i.e if regex_pattern points to a character that a subExpr could not possibly start with
    {
        sub->is_empty = true;
        sub->repeated_block = NULL;
        sub->sub_expr = NULL;
    } else
    {
        sub->is_empty = false;
        RepeatedBlock* r = parse_repeated_block(regex_pattern);
        SubExpr* child_sub = parse_sub_expression(regex_pattern);
        sub->repeated_block = r;
        sub->sub_expr = child_sub;
    }

    return sub;
}

RepeatedBlock* parse_repeated_block(char** regex_pattern)
{
    RepeatedBlock* r = malloc(sizeof(RepeatedBlock));
    Block* b = parse_block(regex_pattern);
    Star* s = parse_star(regex_pattern);

    r->block = b;
    r->star = s;

    return r;
}

Block* parse_block(char** regex_pattern)
{
    Block* b = malloc(sizeof(Block));
    if (**regex_pattern == '(')
    {
        b->type = BT_ExpressionType;
        (*regex_pattern)++;
        Expr* e = parse_expression(regex_pattern);
        if (**regex_pattern != ')')
        {
            fprintf(stderr, "I believe that the brackets \
were not closed mate\n");
            exit(1);
        }
        // ensure regex_pattern points to the closing bracket ')'
        (*regex_pattern)++;
        b->data = e;
        
    } else
    {
        b->type = BT_SingleCharType;
        b->data = parse_single_char(regex_pattern);
    }

    return b;
}

SingleChar* parse_single_char(char** regex_pattern)
{
    SingleChar* s = malloc(sizeof(SingleChar));
    if (**regex_pattern == '\\')
    {
        s->is_escape = true;
        (*regex_pattern)++;
        s->escape_char = parse_escape_char(regex_pattern);
        // s->char_data = 0;
    } else
    {
        s->is_escape = false;
        s->char_data = **regex_pattern;
        (*regex_pattern)++;
        // s->escape_char = NULL;
    }

    return s;
}

Star* parse_star(char** regex_pattern)
{
    Star* star = malloc(sizeof(Star));
    if (**regex_pattern == '*')
    {
        star->is_empty = false;
        (*regex_pattern)++;
    } else
    {
        star->is_empty = true;
    }

    return star;
}

EscapeChar* parse_escape_char(char** regex_pattern)
{
    switch (**regex_pattern)
    {
        case '.': case '*': case 'n':
        case 't': case 's':
        {
            EscapeChar* e = malloc(sizeof(EscapeChar));
            e->char_data = **regex_pattern;
            (*regex_pattern)++;
            return e;
        }
        default:
        {
            fprintf(stderr, "I believe there is an invalid escape sequence\n");
            exit(1);
        }
    }
}