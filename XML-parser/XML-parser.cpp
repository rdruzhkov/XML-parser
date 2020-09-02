
#include <stdio.h>
#include <string>
#include <list>

#include "C_Structs.h"
#include "Utility.h"    
#include "y.tab.h"

using namespace std;

bool g_errorOccured = false;

list<Tag> tags;

extern "C"
{
    extern int g_line;

    extern FILE* yyin;

    extern int yyparse();

    void Exit(int exitCode);

    void yyerror(const char* s)
    {
        printf("[E,%d]: %s.\n", g_line, s);
        g_errorOccured = true;
    }
}

int main(int argc, char ** argv)
{
    char* fileToParse = argv[1];

    errno_t error = fopen_s(&yyin, fileToParse, "r");
    if (error != 0)
    {
        printf("[E]: Can't open file: %s\n", fileToParse);
        return false;
    }

    yyparse();
    fclose(yyin);

    ValidateTagsNesting(tags);

    if (g_errorOccured)
    {
        printf("[E]: Document \"%s\" is not valid.\n", fileToParse);
        Exit(-1);
    }
    else
    {
        printf("[I]: Document \"%s\" is valid.\n", fileToParse);
        Exit(1);
    } 
}