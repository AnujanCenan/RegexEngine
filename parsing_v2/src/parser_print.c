#include "../include/parser.h"

void print_indentation(int generation);
void print_sub_expr(Sub_Expr* sub, int generation);
void print_or_expr(Or_Expr* or, int generation);
void print_star_expr(Star_Expr* star, int generation);
void print_simple_expr(Simple_Expr* simple, int generation);


void print_indentation(int generation)
{

    for (int i = 0; i < generation; ++i)
    {
        printf(" ");
    }

    printf("%d: ", generation + 1);

}

void print_expr(Expr* expr, int generation)
{

    print_indentation(generation);
    printf("<Expr>\n");
    print_sub_expr(expr->sub, generation + 1);
    print_or_expr(expr->or_expr, generation + 1);
}

void print_sub_expr(Sub_Expr* sub, int generation)
{
    print_indentation(generation);
    printf("<Sub>\n");
    if (sub->is_empty) return;
    print_star_expr(sub->star, generation + 1);
    print_sub_expr(sub->sub, generation + 1);
}

void print_or_expr(Or_Expr* or, int generation)
{
    print_indentation(generation);
    printf("<Or>\n");
    if (or->is_empty) return;
    print_expr(or->expr, generation + 1);
}

void print_star_expr(Star_Expr* star, int generation)
{
    print_indentation(generation);

    printf("<Star>");
    if (star->has_star)
    {
        printf(" *");
    } else
    {
        printf(" - None");
    }
    printf("\n");

    print_simple_expr(star->simple, generation + 1);
}

void print_simple_expr(Simple_Expr* simple, int generation)
{
    print_indentation(generation);
    printf("<Simple>");
    switch (simple->type)
    {
        case ST_Escape_Char:
        case ST_Single_Char:
        {
            printf(" - ");
            fputs(simple->spelling, stdout);
            printf("\n");
            break;
        }
        case ST_Grouped_Expr:
        {
            printf("\n");
            print_expr(simple->expr, generation + 1);
            break;
        }
        default:
        {
            fprintf(stderr, "print_simple_expr: Expression Type not recognised\n");
            exit(1);
        }
    }
}