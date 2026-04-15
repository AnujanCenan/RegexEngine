#include "../include/parser_print_tree.h"

void print_indent(int generation)
{
    for (int i = 0; i < generation; ++i)
    {
        printf("  ");
    }
    printf("%d: ", generation + 1);
}

void print_node(
    void* node, NodeType nt, int generation,
    void** node_stack, 
    NodeType* label_stack, 
    int* generation_stack, 
    int* size
)
{
    print_indent(generation);
    switch (nt)
    {
    case NT_Expr:
    {
        Expr* e = (Expr*) node;

        printf("<Expression>\n");
        node_stack[*size] = e->or_expr;
        generation_stack[*size] = generation + 1;
        label_stack[(*size)++] = NT_OrExpr;

        node_stack[*size] = e->sub;
        generation_stack[*size] = generation + 1;
        label_stack[(*size)++] = NT_SubExpr;

        return;
    }
    case NT_OrExpr:
    {
        OrExpr* or = (OrExpr*) node;
        printf("<OrExpr>\n");
        if (!or->is_empty)
        {
            node_stack[*size] = or->expr;
            generation_stack[*size] = generation + 1;
            label_stack[(*size)++] = NT_Expr;
        }
        return;
    }
    case NT_SubExpr:
    {
        SubExpr* sub = (SubExpr*) node;
        printf("<SubExpr>\n");
        if (!sub->is_empty)
        {
            node_stack[*size] = sub->sub_expr;
            generation_stack[*size] = generation + 1;
            label_stack[(*size)++] = NT_SubExpr;

            node_stack[*size] = sub->repeated_block;
            generation_stack[*size] = generation + 1;
            label_stack[(*size)++] = NT_RepBlock;
        }
        return;
    }
    case NT_RepBlock:
    {
        RepeatedBlock* rep = (RepeatedBlock*) node;
        printf("<RepeatedBlock>\n");

        node_stack[*size] = rep->star;
        generation_stack[*size] = generation + 1;
        label_stack[(*size)++] = NT_Star;

        node_stack[*size] = rep->block;
        generation_stack[*size] = generation + 1;
        label_stack[(*size)++] = NT_Block;
        return;
    }
    case NT_Block:
    {
        Block* block = (Block*) node;
        printf("<Block>\n");
        if (block->type == BT_SingleCharType)
        {
            node_stack[*size] = (SingleChar*) block->data;
            generation_stack[*size] = generation + 1;
            label_stack[(*size)++] = NT_SingleChar;
        } else if (block->type == BT_ExpressionType)
        {
            node_stack[*size] = (Expr*) block->data;
            generation_stack[*size] = generation + 1;
            label_stack[(*size)++] = NT_Expr;
        }
        return;
    }
    case NT_SingleChar:
    {
        SingleChar* single_char = (SingleChar*) node;
        printf("<SingleChar>");
        if (single_char->is_escape)
        {
            printf("\n");
            node_stack[*size] = single_char->escape_char;
            generation_stack[*size] = generation + 1;
            label_stack[(*size)++] = NT_Escape;
        } else
        {
            printf(" ==> %c\n", single_char->char_data);
        }
        return;

    }
    case NT_Star:
    {
        Star* star = (Star*) node;
        printf("<Star>");
        if (!star->is_empty)
        {
            printf(" ==> *\n");

        } else
        {
            printf("\n");
        }
        return;
    }
    case NT_Escape:
    {
        EscapeChar* esc = (EscapeChar*) node;
        printf("<EscapeChar> ==> \\%c\n", esc->char_data);
        return;
    }
    default:
    {
        fprintf(stderr, "In print_node: found a weird value for <nt>; %d\n", nt);
        exit(1);
    }
    }
}

void print_tree(Expr* expr)
{

    void** node_stack = malloc(MAX_STACK_SIZE * sizeof(void*));
    NodeType* label_stack = malloc(MAX_STACK_SIZE * sizeof(char*));
    int* generation_stack = malloc(MAX_STACK_SIZE * sizeof(int));
    node_stack[0] = expr;
    label_stack[0] = NT_Expr;

    int size = 1;
    
    while (size > 0)
    {
        void* curr = node_stack[size - 1];
        NodeType nt = label_stack[size - 1];
        int generation = generation_stack[size - 1];
        size--;
        print_node(curr, nt, generation, 
            node_stack, label_stack, generation_stack, &size);
    }

    free(node_stack);
    free(label_stack);
    free(generation_stack);
}