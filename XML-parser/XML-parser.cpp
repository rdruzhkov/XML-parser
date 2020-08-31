
#include <stdio.h>

#include "C_Structs.h"
#include "y.tab.h"

bool g_errorOccured = false;

extern "C"
{
    extern FILE* yyin;

    extern int yyparse();

    void Exit(int exitCode);

    void yyerror(const char* s)
    {
        printf("[E]: %s.\n", s);
        g_errorOccured = true;
    }
}

int main(int argc, char ** argv)
{
    errno_t error = fopen_s (&yyin, argv[1], "r");
    if (error != 0)
    {
        printf("[E]: Can't open file: %s\n", argv[1]);
        return -1;
    }

    yyparse();

    fclose(yyin);

    if (g_errorOccured)
    {
        printf("[E]: Syntax error(s) were spotted out.\n");
    }
    else
    {
        printf("[I]: Document \"%s\" is valid.\n", argv[1]);
    }
    
    Exit(1);
}