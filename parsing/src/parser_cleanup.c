#include "../include/parser_cleanup.h"


// Private Function Declarations
void free_repeated_block(RepeatedBlock* node);
void free_or_expression(OrExpr* node);
void free_sub_expression(SubExpr* node);
void free_block(Block* node);
void free_single_char(SingleChar* node);
void free_star(Star* node);
void free_escape_char(EscapeChar* node);

// Function Definitions
void free_expression(Expr* node)
{
    free_sub_expression(node->sub);
    free_or_expression(node->or_expr);
    free(node);
}

void free_or_expression(OrExpr* node)
{
    if (!node->is_empty)
    {
        free_expression(node->expr);
    }
    free(node);
}

void free_sub_expression(SubExpr* node)
{
    if (!node->is_empty)
    {
        free_repeated_block(node->repeated_block);
        free_sub_expression(node->sub_expr);
    }

    free(node);
}

void free_repeated_block(RepeatedBlock* node)
{
    free_block(node->block);
    free_star(node->star);
    free(node);

}
void free_block(Block* node)
{
    if (node->type == BT_ExpressionType)
    {
        free_expression((Expr*) node->data);
    } else if (node->type == BT_SingleCharType)
    {
        free_single_char((SingleChar*) node->data);
    }

    free(node);

}
void free_single_char(SingleChar* node)
{
    if (node->is_escape)
    {
        free_escape_char(node->escape_char);
    }

    free(node);

}
void free_star(Star* node)
{
    free(node);
}

void free_escape_char(EscapeChar* node)
{
    free(node);
}

