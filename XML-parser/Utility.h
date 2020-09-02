
#include <string>
#include <list>

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "C_Structs.h"

using namespace std;

extern bool g_errorOccured;



class Tag
{
public:
	Tag(string name, TagType type, int line) : name(name), type(type), line(line) {}
	bool isPair(const Tag & other);
	TagType Type();
	string Name();
	int Line();
	bool operator==(const Tag& other);

private:
	string name;
	TagType type;
	int line;
};

extern list<Tag> tags;

extern "C" void PrintError(char const* const _Format, ...);

extern "C" int CountChar(char* str, char c);

extern "C" bool IsWhiteSpace(char* str);

extern "C" void Exit(int exitCode);

extern "C" void ValidateXmlProlog(str xml_keyword, Attribute _1st_attribute, Attribute _2d_attribute);

extern "C" void AddToTagList(str name, TagType type);

void ValidateTagsNesting(list<Tag>& tags);