#include "dfa.h"

void get_nfa_lookup_dfs(NFA_Node* src, NFA_Node** lookup, bool* visited)
{
    if (visited[src->id]) return;
    visited[src->id] = true;
    lookup[src->id] = src;

    for (int i = 0; i < src->edges->size; ++i)
    {
        Edge* curr_edge = src->edges->data[i];

        get_nfa_lookup_dfs(curr_edge->neigh, lookup, visited);
    }
}

NFA_Node** get_nfa_lookup(NFA* nfa)
{
    NFA_Node** lookup = malloc(nfa->size * sizeof(NFA_Node *));
    bool* visited = calloc(nfa->size, sizeof(bool));

    get_nfa_lookup_dfs(nfa->initial, lookup, visited);

    free(visited);
    return lookup;
}


/**
 * [Private] helper function for epsilon closure, which traverses a subset of the 
 * NFA to find epsilon edges and the corresponding neighbour nodes
 * 
 * Params
 * src: pointer to an NFA_Node where the DFS will begin
 * nfa_nodes: the address of a pointer that points to a single Hash_Set
 * visited: the address to a pointer that points to a Hash_Set that contains the ids
 * of visited NFA nodes
 */
void epsilon_closure_dfs(NFA_Node* src, Hash_Set** nfa_nodes, Hash_Set** visited) // MADE THIS A DOUBLE POINTER FOR VISITED -- if single pointer, during resize, the visited pointer would change but on the call stack, the parent dfs call would still have the stale visited object
{
    if (hash_set_exists(*visited, src->id)) return;
    hash_set_add(visited, src->id);
    hash_set_add(nfa_nodes, src->id);    

   for (int i = 0; i < src->edges->size; ++i)
    {
        Edge* curr_edge = src->edges->data[i];
        if (strcmp(curr_edge->spelling, "") == 0)
        {
            // hash_set_add(nfa_nodes, curr_edge->neigh->id);      // NO NEED TO DO THIS? YOU WILL VISIT THIS NODE AND ADD IT, do it as you mark as visited instead
            epsilon_closure_dfs(curr_edge->neigh, nfa_nodes, visited);
        }

    }
}


/**
 * Given a hash set of nfa nodes, edits this hash set in place to obtain the total
 * epsilon closure of this set
 * 
 * Params:
 * nfa_nodes: the address of a pointer that points to a single Hash_Set
 * representing a collection of NFA node ids
 * 
 * lookup: a 1D array consisting of NFA_Nodes where lookup[i] = N where N.id == i
 */
void epsilon_closure(Hash_Set** nfa_nodes, NFA_Node** lookup)
{
    Hash_Set* visited = hash_set_init();

    HS_Iterable* iter = hash_set_get_iterable(*nfa_nodes);
    for (int i = 0; i < iter->size; ++i)
    {
        NFA_Node* curr = lookup[ iter->iterable[i] ];
        epsilon_closure_dfs(curr, nfa_nodes, &visited);
    }
    hash_set_iterable_free(iter);
    hash_set_free(visited);
}


bool move_spelling_matches(char c, char* spelling)
{
    if (strcmp(spelling, ".") == 0) return true;

    switch (c)
    {
        case '.':
        {
            if (strcmp(spelling, "\\.") == 0) return true;
            return false;
        } default:
        {
            if (strlen(spelling) == 1 && spelling[0] == c) return true;
            return false;
        }
    }
}

/**
 * src: the pointer that points to the NFA_Node where the the immediate move operation
 * should be applied
 * c: the char with which the move operation should be applied
 * res is the address of a pointer that points to the hash set that stores the 
 * result of a move operation
 */
void move_search_edges(NFA_Node* src, char c, Hash_Set** res)
{
    for (int i = 0; i < src->edges->size; ++i)
    {
        Edge* curr_edge = src->edges->data[i];
        if (move_spelling_matches(c, curr_edge->spelling))
        {
            hash_set_add(res, curr_edge->neigh->id);
        }
    }
}

/**
 * Using char c (1 byte) here assumes no unicode which may be unfair
 */
Hash_Set* move(Hash_Set* nfa_nodes, NFA_Node** lookup, char c)
{
    Hash_Set* res = hash_set_init();
    HS_Iterable* iter = hash_set_get_iterable(nfa_nodes);
    for (int i = 0; i < iter->size; ++i)
    {
        NFA_Node* curr = lookup[ iter->iterable[i] ];
        move_search_edges(curr, c, &res);
    }

    epsilon_closure(&res, lookup);

    hash_set_iterable_free(iter);
    return res;
}


DFA_Node* create_dfa_node(Hash_Set* hs, DFA* dfa, bool terminal, Zobrist* zobrist_set)
{
    DFA_Node* node = malloc(sizeof(DFA_Node));

    node->terminal = terminal;
    node->id = dfa->num_nodes++;
    node->set = hs;
    node->edges = DFA_Edges_init(NUM_EDGES_PER_DFA_NODE);       // FORGOT TO DO THIS

    zobrist_add(zobrist_set, hs, node);      // DIDNT DO THIS BEFORE
    return node;
}

DFA_Edge* create_dfa_edge(DFA_Node* neigh, char spelling)
{
    DFA_Edge* edge = malloc(sizeof(DFA_Edge));
    edge->neigh = neigh;
    edge->spelling = spelling;

    return edge;
}

DFA* dfa_init()
{
    DFA* dfa = malloc(sizeof(DFA));
    dfa->num_nodes = 0;
    dfa->start = NULL;
    return dfa;
}

DFA* dfa_construct(NFA* nfa)
{
    NFA_Node** lookup = get_nfa_lookup(nfa);
    DFA* dfa = dfa_init();

    Zobrist* zobrist_set = zobrist_init(nfa->size);

    DFA_Node_Queue* queue = DFA_Node_Queue_init(nfa->size);
    int queue_head = 0;

    Hash_Set* src_set = hash_set_init();
    hash_set_add(&src_set, nfa->initial->id);
    epsilon_closure(&src_set, lookup);

    DFA_Node* src_node = create_dfa_node(src_set, dfa, hash_set_exists(src_set, nfa->terminal->id), zobrist_set);       // can't be sure if it terminal == false
    dfa->start = src_node;


    DFA_Node_Queue_append(queue, src_node);

    while (queue_head < queue->size)
    {
        DFA_Node* curr_node = queue->data[queue_head++];
        for (char c = ASCII_LOWER; c < ASCII_UPPER; ++c)
        {
            Hash_Set* new_nodes = move(curr_node->set, lookup, c);
            if (new_nodes->num_active == 0)
            {
                hash_set_free(new_nodes);
                continue;
            }
            DFA_Node* node = NULL;
            if (!zobrist_exists(zobrist_set, new_nodes))
            {
                node = create_dfa_node(new_nodes, dfa, hash_set_exists(new_nodes, nfa->terminal->id), zobrist_set);
                DFA_Node_Queue_append(queue, node);
            } else
            {
                node = (DFA_Node*) zobrist_get(zobrist_set, new_nodes);
                hash_set_free(new_nodes);
            }
            DFA_Edge* edge = create_dfa_edge(node, c);
            DFA_Edges_append(curr_node->edges, edge);
        }
    }

    free(lookup);
    zobrist_free(zobrist_set, false);
    DFA_Node_Queue_free(queue);
    return dfa;
}


void dfa_print_dfs(DFA_Node* src, bool* visited)
{
    if (visited[src->id]) return;
    visited[src->id] = true;
    
    if (src->terminal) printf("Node %d is a finishing node\n", src->id);

    for (int i = 0; i < src->edges->size; ++i)
    {
        DFA_Edge* edge = src->edges->data[i];
        printf("Edge from %d to %d; spelling = %c\n", src->id, edge->neigh->id, edge->spelling);
        dfa_print_dfs(edge->neigh, visited);
    }
}

void dfa_print(DFA* dfa)
{
    bool* visited = calloc(dfa->num_nodes, sizeof(bool));
    dfa_print_dfs(dfa->start, visited);

    free(visited);
}


void dfa_free(DFA* dfa)
{
    bool* visited = calloc(dfa->num_nodes, sizeof(bool));
    DFA_Node_Queue* q = DFA_Node_Queue_init(dfa->num_nodes);
    
    DFA_Node_Queue_append(q, dfa->start);
    int queue_head = 0;
    while (queue_head < q->size)
    {
        DFA_Node* curr = q->data[queue_head++];
        for (int i = 0; i < curr->edges->size; ++i)
        {
            DFA_Node* neigh = curr->edges->data[i]->neigh;
            if (!visited[neigh->id])
            {
                visited[neigh->id] = true;
                DFA_Node_Queue_append(q, neigh);
            }
            free(curr->edges->data[i]);
        }

        DFA_Edges_free(curr->edges);
    }

    free(visited);

    // freeing the nodes
    for (int i = 0; i < q->size; ++i)
    {
        hash_set_free(q->data[i]->set);
        free(q->data[i]);
    }

    DFA_Node_Queue_free(q);
    free(dfa);
}


// TODO: 

// the public parse_expr function shouldnt need a ptr i think (because we know we start at 0)
// the public translate_expr function shouldnt need a num_nodes ptr I think (this information is stored in the NFA struct)

