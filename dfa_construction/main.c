#include "dfa.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "USAGE: <program_name> <regex>\n");
        exit(1);
    }

    char* regex = argv[1];

    int ptr = 0;
    Expr* expr = parse_expr(regex, strlen(regex), &ptr);

    int num_nodes = 0;
    NFA* nfa = translate_expr(expr, &num_nodes);

    DFA* dfa = dfa_construct(nfa);

    dfa_print(dfa);

    free_expr(expr);
    nfa_free(nfa);
    dfa_free(dfa);
    
    return 0;
}

