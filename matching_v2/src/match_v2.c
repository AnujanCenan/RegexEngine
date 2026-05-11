#include "match_v2.h"

DFA_Edge* find_matching_edge(char c, DFA_Node* node);
bool regex_match(DFA* dfa, char* text);



DFA_Edge* find_matching_edge(char c, DFA_Node* node)
{
    for (int i = 0; i < node->edges->size; ++i)
    {
        DFA_Edge* curr_edge = node->edges->data[i];
        if ((curr_edge->spelling) == c) return curr_edge;
    }

    return NULL;
}

bool regex_match(DFA* dfa, char* text)
{
    int text_len = strlen(text);
    DFA_Node* curr_node = dfa->start;
    for (int i = 0; i < text_len; ++i)
    {
        char c = text[i];
        DFA_Edge* edge_to_traverse = find_matching_edge(c, curr_node);
        if (!edge_to_traverse)
        {
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


    bool result = regex_match(dfa, text);

    free_expr(expr);
    nfa_free(nfa);
    dfa_free(dfa);

    return result;
}
