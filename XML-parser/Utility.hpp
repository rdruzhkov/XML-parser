
#include <string>
#include <list>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "TagType.h"
#include "C_str.h"
#include "C_Attribute.h"
#include "C_Attributes.h"

#include "Attribute.hpp"

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

extern "C" void AddToTagList(C_str name, TagType type);

extern "C" bool ValidateName(C_str name);

extern "C" bool ValidateProlog(C_str name, C_Attributes * c_attributes);

void ValidateTagsNesting(list<Tag>& tags);

bool ValidateAtrributeList(list<Attribute> & attributes);

bool ValidateName(string name, int line);