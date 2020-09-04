
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string>
#include <list>

#include "TagType.h"
#include "Utility.hpp"    

using namespace std;

bool g_errorOccured = false;

bool g_unknownSyntaxErrorOccured = false;

list<Tag> tags;

string fileToParse;

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
        g_unknownSyntaxErrorOccured = true;
    }
}

int main(int argc, char ** argv)
{
    fileToParse.assign(argv[1]);

    yyin = fopen(fileToParse.c_str(), "r");
    if (yyin == NULL)
    {
        printf("[E]: Can't open file: %s\n", fileToParse.c_str());
        Exit(-1);
    }

    yyparse();
    fclose(yyin);

    ValidateTagsNesting(tags);

    Exit(1);
}