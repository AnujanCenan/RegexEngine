#include "./match_v2.h"

/**
 * Ensure you are in the root directory
 * 
 * Compilation: 
 * Compile using the CMAKE file or using the command
 * Specifically run:
 *      mkdir build && cd build
 *      cmake ..
 *      make
 * The exectuable named "regex_engine" will be built in the build directory
 * Running ./regex_engine will cause the program to run
 * 
 * Alternatively, you can run the command found in "compilation.sh" while in 
 * the root directory. If executable permissions are denied, run
 *      chmod +x ./compile.sh
 * With this method, the "regex_engine" executable to be built
 * in the root directory. NOTE that you may need to change the compiler from clang
 * to the compiler you have installed on your computer if you use this method.
 *     
 * Executing: 
 * When running the resulting executable, the command line arguments are as follows
 *      0: program name (default=regex_engine)
 *      1: text
 *      2: regex pattern 
 */
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "USAGE: <program> <text> <regex>\n");
        exit(1);
    }

    bool result = run_regex(argv[1], argv[2]);
    
    result ? printf("MATCH\n") : printf("NO MATCH\n");
    return 0;
}