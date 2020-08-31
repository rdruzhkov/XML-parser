
#include <string>

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#include "C_Structs.h"

using namespace std;

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
	printf("[I]: Exiting parser. Press any key to continue...");
	_getch();
	exit(exitCode);
}

extern "C" void ValidateXmlProlog( Name xml_keyword, Attribute _1st_attribute, Attribute _2d_attribute )
{
	string xmlKeyWord( * reinterpret_cast<string*>(xml_keyword.name.p_string));
	string xmlKeyWordNamespace(*reinterpret_cast<string*>(xml_keyword._namespace.p_string));

	string _1stAttributeName( * reinterpret_cast<string*>(_1st_attribute.name.name.p_string));
	string _1stAttributeNameNamespace(*reinterpret_cast<string*>(_1st_attribute.name._namespace.p_string));
	string _1stAttributeValue( * reinterpret_cast<string*>(_1st_attribute.value.p_string));

	string _2dAttributeName( * reinterpret_cast<string*>(_2d_attribute.name.name.p_string));
	string _2dAttributeNameNamespace(*reinterpret_cast<string*>(_2d_attribute.name._namespace.p_string));
	string _2dAttributeValue( * reinterpret_cast<string*>(_2d_attribute.value.p_string));

	int line = xml_keyword.name.line;

	if (xmlKeyWord != "xml")
	{
		printf("[E,%d]: XML prolog starts not with \"xml\" keyword.\n", xml_keyword.name.line);
	}
	else if (xmlKeyWordNamespace != "")
	{
		printf("[E,%d]: \"xml\" keyword can't have namespaces.\n", xml_keyword._namespace.line);
	}

	if (_1stAttributeName != "version")
	{
		printf("[E,%d]: XML prolog 1st atribute must be \"version\".\n", _1st_attribute.name.name.line);
	}
	else if (_1stAttributeValue != "1.0")
	{
		printf("[E,%d]: XML prolog incorrect version number. Only \"1.0\" is supported.\n", _1st_attribute.value.line);
	}
	else if (_1stAttributeNameNamespace != "")
	{
		printf("[E,%d]: XML prolog \"version\" atribute can't have namespace.\n", _1st_attribute.name._namespace.line);
	}

	if (_2dAttributeName != "encoding")
	{
		printf("[E,%d]: XML prolog 2d attribute must be \"encoding\".\n", _2d_attribute.name.name.line);
	}
	else if ( _2dAttributeValue != "ASCII" &&
			  _2dAttributeValue != "Windows-1252" && 
			  _2dAttributeValue != "UTF-8" && 
			  _2dAttributeValue != "ISO-8859-1" )
	{
		printf("[E,%d]: XML prolog incorrect encoding value. Supported encodings are: \"ASCII\", \
\"Windows-1252\", \"UTF-8\", \"ISO-8859-1\".\n", _2d_attribute.value.line);
	}
	else if (_2dAttributeNameNamespace != "")
	{
		printf("[E,%d]: XML prolog \"encoding\" atribute can't have namespace.\n", _2d_attribute.name._namespace.line);
	}

}