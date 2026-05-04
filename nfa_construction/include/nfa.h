#ifndef NFA_H
#define NFA_H

#include "../parsing_v2/include/parser.h"
#include "../../dynamic_array/dynamic_array.h"

#define NUM_EDGES_PER_NODE 4

typedef struct Node Node;
typedef struct Edge Edge;
typedef struct NFA NFA;

DECLARE_DYNAMIC_ARRAY(Edge*, Edges);
DECLARE_DYNAMIC_ARRAY(Node*, Node_Queue);

struct Edge
{
    Node* neigh;
    char* spelling;
};

struct Node
{
    Edges* edges;
    int id;    
};

struct NFA
{
    Node* initial;
    Node* terminal;
    int size;
};


NFA* translate_expr(Expr* expr, int* num_nodes);
void nfa_free(NFA* nfa);

#endif