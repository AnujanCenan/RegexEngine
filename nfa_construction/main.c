#include "include/nfa.h"

int main(int argc, char* argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "USAGE: <PROGRAM> <REGEX>\n");
        exit(1);
    }

    char* regex = argv[1];
    int regex_len = strlen(regex);
    int ptr = 0;

    Expr* expr = parse_expr(regex, regex_len, &ptr);
    int num_nodes = 0;
    NFA* nfa = translate_expr(expr, &num_nodes);
    printf("NFA size = %d\n", nfa->size);
    
    nfa_free(nfa);

    free_expr(expr);

    return 0;
}