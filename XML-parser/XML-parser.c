
#include <stdio.h>

#include "y.tab.h"

extern FILE* yyin;

extern int yyparse();

void yyerror(const char* s)
{
    printf("[E]: %s.\n", s);
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
    
    return 0;
}