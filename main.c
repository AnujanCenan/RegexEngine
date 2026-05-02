#include "./parser.h" 

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
    print_expr(expr, 0);
    free_expr(expr);

    return 0;
}