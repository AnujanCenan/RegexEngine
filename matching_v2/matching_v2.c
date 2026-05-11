#include "dfa.h"

DFA_Edge* find_matching_edge(char c, DFA_Node* node)
{
    for (int i = 0; i < node->edges->size; ++i)
    {
        DFA_Edge* curr_edge = node->edges->data[i];
        printf("\t\tCurrent edge spelling = %c | text_char = %c\n", curr_edge->spelling, c);
        if ((curr_edge->spelling) == c) return curr_edge;
    }

    return NULL;
}

/**
 * We start at the DFA start node
 * For each character in the text, we ask if our current node has an edge labelled with that character
 * If it does, we move along that edge to the next node; if not , we return false
 * 
 * If we run out of characters, we check if the node we are standing on is a terminal node
 * If it is then return true, otherwise return false
 */
bool regex_match(DFA* dfa, char* text)
{
    int text_len = strlen(text);
    DFA_Node* curr_node = dfa->start;
    printf("Src node = %d\n", curr_node->id);
    for (int i = 0; i < text_len; ++i)
    {
        printf("Current node = %d\n", curr_node->id);
        char c = text[i];
        DFA_Edge* edge_to_traverse = find_matching_edge(c, curr_node);
        if (!edge_to_traverse)
        {
            printf("\tCould not find a valid edge\n");
            return false;
        } else
        {
            curr_node = edge_to_traverse->neigh;
        }
    }

    return curr_node->terminal;
}


bool run_regex(char* text, char* regex)
{
    int ptr = 0;
    Expr* expr = parse_expr(regex, strlen(regex), &ptr);

    int num_nodes = 0;
    NFA* nfa = translate_expr(expr, &num_nodes);

    DFA* dfa = dfa_construct(nfa);


    dfa_print(dfa);
    bool result = regex_match(dfa, text);

    free_expr(expr);
    nfa_free(nfa);
    dfa_free(dfa);

    return result;
}

