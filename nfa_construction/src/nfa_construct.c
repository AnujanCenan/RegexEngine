#include "../include/nfa.h"

NFA* translate_or_expr(Or_Expr* or_expr, int* num_nodes);
NFA* translate_sub_expr(Sub_Expr* sub, int* num_nodes);
NFA* translate_star_expr(Star_Expr* star, int* num_nodes);
NFA* translate_simple_expr(Simple_Expr* simple, int* num_nodes);


void create_edge(Node* from_node, Node* to_node, char* spelling)
{
    Edge* edge = malloc(sizeof(Edge));
    edge->neigh = to_node;
    
    if (spelling == NULL)
    {
        edge->spelling = calloc(1, sizeof(char));
    } else
    {
        edge->spelling = strdup(spelling);
    }

    Edges_append(from_node->edges, edge);
}

Node* create_node(int* num_nodes)
{
    Node* node = malloc(sizeof(Node));
    node->id = *num_nodes;
    *num_nodes += 1;
    node->edges = Edges_init(NUM_EDGES_PER_NODE);


    return node;
}

NFA* translate_expr(Expr* expr, int* num_nodes)
{
    if (expr->sub->is_empty)
    {
        NFA* base = malloc(sizeof(NFA));
        base->initial = create_node(num_nodes);

        base->terminal = create_node(num_nodes);
        
        create_edge(base->initial, base->terminal, NULL);

        base->size = 2;
        return base;
    } else 
    {
        NFA* sub_translate = translate_sub_expr(expr->sub, num_nodes);
        if (expr->or_expr->is_empty) return sub_translate;

        NFA* or_translate = translate_or_expr(expr->or_expr, num_nodes);

        NFA* base = malloc(sizeof(NFA));
        base->initial = create_node(num_nodes);

        base->terminal = create_node(num_nodes);

        create_edge(base->initial, sub_translate->initial, NULL);
        create_edge(base->initial, or_translate->initial, NULL);

        create_edge(sub_translate->terminal, base->terminal, NULL);
        create_edge(or_translate->terminal, base->terminal, NULL);

        base->size = sub_translate->size + or_translate->size + 2;

        free(sub_translate);
        free(or_translate);
        
        return base;
    }
}

NFA* translate_or_expr(Or_Expr* or_expr, int* num_nodes)
{
    if (or_expr->is_empty)
    {
        printf("NFA_Construction::translate_or_expr - WARNING: translate_or_expr should not have to deal with an empty or expr\n");
        return NULL;
    }
    return translate_expr(or_expr->expr, num_nodes);
}

NFA* translate_sub_expr(Sub_Expr* sub, int* num_nodes)
{
    if (sub->is_empty)
    {
        // TODO: return an 'empty NFA'
        printf("NFA_Construction::translate_sub_expr - WARNING: translate_sub_expr should not have to deal with an empty sub expr\n");
        return NULL;
    }

    NFA* star_translate = translate_star_expr(sub->star, num_nodes);
    if (!sub->sub->is_empty)
    {
        NFA* child_sub_translate = translate_sub_expr(sub->sub, num_nodes);

        NFA* base = malloc(sizeof(NFA));

        base->initial = star_translate->initial;
        base->terminal = child_sub_translate->terminal;

        Node* merge = create_node(num_nodes);

        create_edge(star_translate->terminal, merge, NULL);
        create_edge(merge, child_sub_translate->initial, NULL);

        base->size = star_translate->size + child_sub_translate->size + 1;

        free(star_translate);
        free(child_sub_translate);
        return base;

    } else
    {
        return star_translate;
    }
}


NFA* translate_star_expr(Star_Expr* star, int* num_nodes)
{
    NFA* simple_translate = translate_simple_expr(star->simple, num_nodes);
    if (star->has_star)
    {

        NFA* base = malloc(sizeof(NFA));
        base->initial = create_node(num_nodes);

        base->terminal = create_node(num_nodes);

        create_edge(base->initial, simple_translate->initial, NULL);
        create_edge(base->initial, base->terminal, NULL);

        create_edge(simple_translate->terminal, simple_translate->initial, NULL);
        create_edge(simple_translate->terminal, base->terminal, NULL);

        base->size = simple_translate->size + 2;
        free(simple_translate);

        return base;
    } else
    {
        return simple_translate;
    }
}


NFA* translate_simple_expr(Simple_Expr* simple, int* num_nodes)
{
    switch (simple->type)
    {
        case ST_Escape_Char:
        case ST_Single_Char:
        {
            NFA* base = malloc(sizeof(NFA));
            base->initial = create_node(num_nodes);
            base->terminal = create_node(num_nodes);

            create_edge(base->initial, base->terminal, simple->spelling);

            base->size = 2;
            return base;
        }
        case ST_Grouped_Expr:
        {
            return translate_expr(simple->expr, num_nodes);
        }
        default:
        {
            fprintf(stderr, "NFA_CONSTRUCTION::translate_simple_expr - Expression Type not recognised\n");
            exit(1);
        }
    }
}