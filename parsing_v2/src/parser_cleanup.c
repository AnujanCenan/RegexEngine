#include "../include/parser.h"

void free_or_expr(Or_Expr* or_expr);
void free_sub_expr(Sub_Expr* sub_expr);
void free_star_expr(Star_Expr* star_expr);
void free_simple_expr(Simple_Expr* simple_expr);

void free_expr(Expr* expr)
{
    free_sub_expr(expr->sub);
    free_or_expr(expr->or_expr);
    free(expr);
}

void free_or_expr(Or_Expr* or_expr)
{
    if (!or_expr->is_empty)
    {
        free_expr(or_expr->expr);
    }

    free(or_expr);
}

void free_sub_expr(Sub_Expr* sub_expr)
{
    if (!sub_expr->is_empty)
    {
        free_star_expr(sub_expr->star);
        free_sub_expr(sub_expr->sub);
    }

    free(sub_expr);
}

void free_star_expr(Star_Expr* star_expr)
{
    free_simple_expr(star_expr->simple);
    free(star_expr);
}

void free_simple_expr(Simple_Expr* simple_expr)
{
    switch (simple_expr->type)
    {
        case ST_Escape_Char:
        case ST_Single_Char:
        {
            free(simple_expr->spelling);
            break;
        }
        case ST_Grouped_Expr:
        {
            free_expr(simple_expr->expr);
            break;
        }
        default:
        {
            fprintf(stderr, "free_simple_expr: Expression Type not recognised\n");
            exit(1);
        }
    }

    free(simple_expr);
}
