
#include <string>

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdarg.h>

extern bool g_errorOccured;

extern "C" void PrintError(char const* const _Format, ...);

extern "C" int CountChar(char* str, char c);

extern "C" bool IsWhiteSpace(char* str);

extern "C" void Exit(int exitCode);

extern "C" void ValidateXmlProlog(str xml_keyword, Attribute _1st_attribute, Attribute _2d_attribute);