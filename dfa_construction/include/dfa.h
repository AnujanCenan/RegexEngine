#ifndef DFA_H
#define DFA_H

#include <string.h>

#include "nfa.h"
#include "dynamic_array.h"
#include "hash_set_integer.h"
#include "zobrist_hashing.h"

#define NUM_EDGES_PER_DFA_NODE 70
#define ASCII_LOWER 32
#define ASCII_UPPER 126

typedef struct DFA_Edge DFA_Edge;
typedef struct DFA_Node DFA_Node;
typedef struct DFA DFA;

DECLARE_DYNAMIC_ARRAY(DFA_Edge*, DFA_Edges);
DECLARE_DYNAMIC_ARRAY(DFA_Node*, DFA_Node_Queue);

struct DFA_Edge
{
    DFA_Node* neigh;
    char spelling;
};

struct DFA_Node
{
    Hash_Set* set;
    DFA_Edges* edges;
    int id;
    bool terminal;
};

struct DFA
{
    DFA_Node* start;
    int num_nodes;
};



DFA* dfa_construct(NFA* nfa);
void dfa_print(DFA* dfa);
void dfa_free(DFA* dfa);

#endif