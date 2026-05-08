#include "../include/parser.h"

Or_Expr* parse_or(char* regex, int regex_len, int* ptr);
Sub_Expr* parse_sub(char* regex, int regex_len, int* ptr);
Star_Expr* parse_star(char* regex, int regex_len, int* ptr);
Simple_Expr* parse_simple(char* regex, int regex_len, int* ptr);

Expr* parse_expr(char* regex, int regex_len, int* ptr)
{
    Expr* expr = malloc(sizeof(Expr));
    expr->sub = parse_sub(regex, regex_len, ptr);
    expr->or_expr = parse_or(regex, regex_len, ptr);

    return expr;
}

Or_Expr* parse_or(char* regex, int regex_len, int* ptr)
{
    Or_Expr* expr = malloc(sizeof(Or_Expr));
    if (*ptr < regex_len && regex[*ptr] == '|')
    {
        expr->is_empty = false;
        (*ptr)++;
        expr->expr = parse_expr(regex, regex_len, ptr);
    } else
    {
        expr->is_empty = true;
    }

    return expr;
}

bool valid_sub_start(char* regex, int regex_len, int *ptr)
{
    if (*ptr >= regex_len) return false;
    if (regex[*ptr] == '|' || regex[*ptr] == ')') return false;

    if (regex[*ptr] == '*')
    {
        printf("valid_sub_start: WARNING: sub start attempting to start with a *; returning false...\n");
        return false;
    }

    return true;
}

Sub_Expr* parse_sub(char* regex, int regex_len, int* ptr)
{
    Sub_Expr* expr = malloc(sizeof(Sub_Expr));
    if (valid_sub_start(regex, regex_len, ptr)) 
    {
        expr->is_empty = false;
        expr->star = parse_star(regex, regex_len, ptr);
        expr->sub = parse_sub(regex, regex_len, ptr);
    } else
    {
        expr->is_empty = true;
        expr->star = NULL;
        expr->sub = NULL;
    }

    return expr;
}

Star_Expr* parse_star(char* regex, int regex_len, int* ptr)
{
    Star_Expr* expr = malloc(sizeof(Star_Expr));
    expr->simple = parse_simple(regex, regex_len, ptr);
    if (*ptr < regex_len && regex[*ptr] == '*')
    {
        expr->has_star = true;
        (*ptr)++;
    } else
    {
        expr->has_star = false;
    }

    return expr;
}

Simple_Expr* parse_simple(char* regex, int regex_len, int* ptr)
{
    Simple_Expr* expr = malloc(sizeof(Simple_Expr));
    
    if (*ptr < regex_len && regex[*ptr] == '(')
    {
        expr->type = ST_Grouped_Expr;
        (*ptr)++;
        expr->expr = parse_expr(regex, regex_len, ptr);
        expr->spelling = NULL;
        if (*ptr >= regex_len || regex[*ptr] != ')')
        {
            fprintf(stderr, "parse_simple:: tried to parse a grouped expression ending at %d but ( seems to be unclosed\n", *ptr - 1);
            exit(1);
        }
        (*ptr)++;
        
    } else if (*ptr < regex_len && regex[*ptr] == '\\')
    {
        expr->type = ST_Escape_Char;
        (*ptr)++;
        expr->expr = NULL;

        expr->spelling = malloc(3 * sizeof(char));
        expr->spelling[0] = '\\';
        if (*ptr >= regex_len)
        {
            fprintf(stderr, "parse_simple:: tried to parse an escape char at index %d but we seem to have reached the end of the string\n", *ptr - 1);
            exit(1);
        }

        expr->spelling[1] = regex[*ptr];
        expr->spelling[2] = '\0';
        (*ptr)++;

    } else if (*ptr < regex_len)
    {
        expr->type = ST_Single_Char;
        expr->spelling = malloc(2 * sizeof(char));
        expr->spelling[0] = regex[*ptr];
        expr->spelling[1] = '\0';
        (*ptr)++;
    } else
    {
        fprintf(stderr, "parse_simple: Tried to parse a simple expression but ptr = %d appears to be an invalid index\n", *ptr);
        exit(1);
    }

    return expr;
}
