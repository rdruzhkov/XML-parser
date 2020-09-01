
#include <string>

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "C_Structs.h"

#include "Utility.h"
using namespace std;

extern "C" void PrintError(char const* const _Format, ...)
{
	va_list args;
	va_start(args, _Format);
	vfprintf(stderr, _Format, args);

	g_errorOccured = true;
}

extern "C" int CountChar(char* str, char c)
{
	int occurencyCounter = 0;

	for (int i = 0; str[i] != 0; i++)
	{
		if (str[i] == c)
		{
			occurencyCounter++;
		}
	}

	return occurencyCounter;
}

extern "C" bool IsWhiteSpace(char* str)
{
	for (int i = 0; str[i] != 0; i++)
	{
		if ((str[i] != ' ') &&
			(str[i] != '\t') &&
			(str[i] != '\n') &&
			(str[i] != '\r'))
		{
			return false;
		}
	}

	return true;
}

extern "C" void Exit(int exitCode)
{
	printf("[I]: Exiting parser. Press any key to continue...\n");
	_getch();
	exit(exitCode);
}

extern "C" void ValidateXmlProlog( str xml_keyword, Attribute _1st_attribute, Attribute _2d_attribute )
{
	string xmlKeyWord( * reinterpret_cast<string*>(xml_keyword.p_string));

	string _1stAttributeName( * reinterpret_cast<string*>(_1st_attribute.name.p_string));
	string _1stAttributeValue( * reinterpret_cast<string*>(_1st_attribute.value.p_string));

	string _2dAttributeName( * reinterpret_cast<string*>(_2d_attribute.name.p_string));
	string _2dAttributeValue( * reinterpret_cast<string*>(_2d_attribute.value.p_string));

	int line = xml_keyword.line;

	if (xmlKeyWord != "xml")
	{	
		PrintError("[E,%d]: XML prolog starts not with \"xml\" keyword.\n", xml_keyword.line);
	}

	if (_1stAttributeName != "version")
	{
		PrintError("[E,%d]: XML prolog 1st atribute must be \"version\".\n", _1st_attribute.name.line);
	}
	else if (_1stAttributeValue != "1.0")
	{
		PrintError("[E,%d]: XML prolog incorrect version number. Only \"1.0\" is supported.\n", _1st_attribute.value.line);
	}

	if (_2dAttributeName != "encoding")
	{
		PrintError("[E,%d]: XML prolog 2d attribute must be \"encoding\".\n", _2d_attribute.name.line);
	}
	else if ( _strcmpi(_2dAttributeValue.c_str(), "ASCII") != 0		  &&
			  _strcmpi(_2dAttributeValue.c_str(), "Windows-1252") != 0 &&
			  _strcmpi(_2dAttributeValue.c_str(), "UTF-8") != 0		  &&
			  _strcmpi(_2dAttributeValue.c_str(), "ISO-8859-1") != 0 )
	{
		PrintError("[E,%d]: XML prolog incorrect encoding value. Supported encodings are: \"ASCII\", \
\"Windows-1252\", \"UTF-8\", \"ISO-8859-1\".\n", _2d_attribute.value.line);
	}
}