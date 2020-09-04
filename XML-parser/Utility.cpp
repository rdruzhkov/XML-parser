
#ifndef unix
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string>
#include <utility>

#ifndef unix
#include <conio.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "C_str.h"
#include "C_Attributes.h"
#include "TagType.h"
#include "Utility.hpp"

#include "Attribute.hpp"

#ifdef unix
	#define _stricmp strcasecmp
#endif

using namespace std;

extern string fileToParse;
extern bool g_unknownSyntaxErrorOccured;

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
	int _exitCode = exitCode;

	if (g_errorOccured)
	{
		printf("[E]: Document \"%s\" is not valid.\n", fileToParse.c_str());

		if (g_unknownSyntaxErrorOccured)
		{
			printf("[E]: Unknown syntax error(s) ocured.\n");
		}

		_exitCode = -1;
	}
	else
	{
		printf("[I]: Document \"%s\" is valid.\n", fileToParse.c_str());
	}

#ifndef unix
	printf("[I]: Exiting parser. Press any key to continue...\n\n");
	_getch();
#else
	printf("\n");
#endif

	exit(_exitCode);
}

extern "C" void PrintStr(C_str s)
{
	printf("%s", reinterpret_cast<string*>(s.p_string)->c_str());
}

bool Tag::isPair(const Tag & other)
{
	if (this->type != other.type &&
		this->name == other.name)
	{
		return true;
	}

	return false;
}

TagType Tag::Type()
{
	return this->type;
}

string Tag::Name()
{
	return this->name;
}

bool Tag::operator==(const Tag& other)
{
	if (this->name == other.name &&
		this->type == other.type)
	{
		return true;
	}

	return false;
}

int Tag::Line()
{
	return this->line;
}

extern "C" void AddToTagList(C_str name, TagType type)
{
	tags.push_back(Tag( * reinterpret_cast<string*>(name.p_string), 
						type, name.line));
}

bool DeleteProperlyNestedTag(list<Tag>& tags)
{
	for (auto it = tags.begin(); it != tags.end(); it++)
	{
		auto nextIt = it;
		nextIt++;

		if (nextIt == tags.end())
		{
			break;
		}

		if ((*it).Type() == Open &&
			(*it).isPair(*nextIt) == true)
		{
			tags.erase(it); 
			tags.erase(nextIt);
			return true;
		}
	}

	return false;
}

bool DeleteProperlyNestedTags(list<Tag>& tags)
{
	bool deletedSome = false;
	while (1) 
	{ 
		if (DeleteProperlyNestedTag(tags))
		{
			deletedSome = true;
		}
		else
		{
			break;
		}
	}

	return deletedSome;
}

pair<list<Tag>::iterator, list<Tag>::iterator> 
FindImproperlyNestedOpenTag(list<Tag>& tags)
{
	if (tags.empty())
	{
		return make_pair(tags.end(), tags.end());
	}

	if (tags.size() == 1)
	{
		if ((*tags.begin()).Type() == Open)
		{
			return make_pair(tags.begin(), tags.end());
		}
		else
		{
			return make_pair(tags.end(), tags.end());
		}
	}

	auto it = tags.begin();
	auto nextIt = it;
	nextIt++;

	while (nextIt != tags.end())
	{
		if ( (*it).Type() == Open &&
			 (*nextIt).Type() == Close)
		{
			if (! (*it).isPair(*nextIt))
			{
				return make_pair(it, nextIt);
			}
		}

		it++;
		nextIt++;
	}

	return make_pair(tags.end(), tags.end());
}

pair<list<Tag>::iterator, list<Tag>::iterator> 
FindImproperlyNestedCloseTag(list<Tag>& tags)
{
	if (tags.empty())
	{
		return make_pair(tags.end(), tags.end());
	}

	if ((*tags.begin()).Type() == Close)
	{
		return make_pair(tags.begin(), tags.end());
	}

	if (tags.size() == 1)
	{
		return make_pair(tags.end(), tags.end());
	}

	auto previousIt = tags.end();
	previousIt--;
	auto it = previousIt;
	previousIt--;

	while (1)
	{
		if ((*it).Type() == Close &&
			(*previousIt).Type() == Open)
		{
			if (!(*it).isPair(*previousIt))
			{
				return make_pair(it, previousIt);
			}
		}

		if (previousIt == tags.begin())
		{
			break;
		}

		it--;
		previousIt--;
	}

	return make_pair(tags.end(), tags.end());
}

list<Tag>::iterator FindCloseTagPair(list<Tag>::iterator closeTagIt, list<Tag>& tags)
{
	if ( closeTagIt == tags.begin() ||
		 closeTagIt == tags.end() )
	{
		return tags.end();
	}

	auto expectantIt = closeTagIt;
	bool lastIteratorChecked = false;
	while (!lastIteratorChecked)
	{
		expectantIt--;

		if (*closeTagIt == *expectantIt)
		{
			break;
		}

		if ((*closeTagIt).isPair(*expectantIt))
		{
			return expectantIt;
		}

		if (expectantIt == tags.begin())
		{
			lastIteratorChecked = true;
		}
	}

	return tags.end();
}

list<Tag>::iterator FindOpenTagPair(list<Tag>::iterator openTagIt, list<Tag>& tags)
{
	if (openTagIt == tags.end())
	{
		return tags.end();
	}

	auto it = openTagIt;
	it++;
	for(auto expectantIt = it; expectantIt != tags.end(); expectantIt++)
	{
		if (*openTagIt == *expectantIt)
		{
			break;
		}

		if ((*openTagIt).isPair(*expectantIt))
		{
			return expectantIt;
		}
	}

	return tags.end();
}

extern "C" bool ValidateName(C_str name)
{
	return ValidateName( * reinterpret_cast<string *>(name.p_string), name.line);
}

void ValidateTagsNesting(list<Tag> & tags)
{
	while (!tags.empty())
	{
		DeleteProperlyNestedTags(tags);
		if (tags.empty())
		{
			break;
		}

		while (1)
		{
			auto pair = FindImproperlyNestedOpenTag(tags);
			auto improperlyNestedOpenTagIt = pair.first;
			if (improperlyNestedOpenTagIt == tags.end())
			{
				break;
			}

			auto closeTagIt = FindOpenTagPair(improperlyNestedOpenTagIt, tags);
			if (closeTagIt == tags.end())
			{
				PrintError("[E,%d]: Open tag of element \"%s\" have no pair.\n", (*improperlyNestedOpenTagIt).Line(), (*improperlyNestedOpenTagIt).Name().c_str());
				tags.erase(improperlyNestedOpenTagIt);	
			}
			else
			{
				if ( FindCloseTagPair(pair.second, tags) != tags.end())
				{
					PrintError("[E,%d,%d]: Tags of element \"%s\" are nested improperly.\n",
						(*improperlyNestedOpenTagIt).Line(), (*closeTagIt).Line(), (*closeTagIt).Name().c_str());
					tags.erase(improperlyNestedOpenTagIt);
					tags.erase(closeTagIt);
				}
				else
				{
					PrintError("[E,%d]: Close tag of element \"%s\" have no pair.\n", (*pair.second).Line(), (*pair.second).Name().c_str());
					tags.erase(pair.second);
				}
			}
		}

		while (1)
		{
			auto pair = FindImproperlyNestedCloseTag(tags);
			auto improperlyNestedCloseTagIt = pair.first;
			if (improperlyNestedCloseTagIt == tags.end())
			{
				break;
			}

			auto openTagIt = FindCloseTagPair(improperlyNestedCloseTagIt, tags);
			if (openTagIt == tags.end())
			{
				PrintError("[E,%d]: Close tag of element \"%s\" have no pair.\n", (*improperlyNestedCloseTagIt).Line(), (*improperlyNestedCloseTagIt).Name().c_str());
				tags.erase(improperlyNestedCloseTagIt);
			}
			else
			{
				if (FindOpenTagPair(pair.second, tags) != tags.end())
				{
					PrintError("[E,%d,%d]: Tags of element \"%s\" are nested improperly.\n",
						(*improperlyNestedCloseTagIt).Line(), (*openTagIt).Line(), (*openTagIt).Name().c_str());
					tags.erase(improperlyNestedCloseTagIt);
					tags.erase(openTagIt);
				}
				else
				{
					PrintError("[E,%d]: Open tag of element \"%s\" have no pair.\n", (*pair.second).Line(), (*pair.second).Name().c_str());
					tags.erase(pair.second);
				}
			}
		}
	}
}

bool ValidateName(string name, int line)
{
	if (name.size() < 3)
	{
		return true;
	}

	char _1st_3_chars[4] = { 0 };
	strncpy(_1st_3_chars, name.c_str(), 3);

	if (_stricmp(_1st_3_chars, "xml") == 0)
	{
		PrintError("[E,%d]: name \"%s\" contains \"[xX][mM][lL]\" pattern in the beggining of the string.\n", line, name.c_str());
		return false;
	}

	return false;
}

bool ValidateAtrributeList(list<Attribute> & attributes)
{
	bool errorOccured = false;

	for (auto& attribute : attributes)
	{
		if (!ValidateName(attribute.name(), attribute.nameLine()))
		{
			errorOccured = true;
		}
	}

	return errorOccured;
}

extern "C" bool ValidateProlog(C_str _name, C_Attributes * c_attributes)
{
	string& name = *reinterpret_cast<string*>(_name.p_string);
	int nameLine = _name.line;
	list<Attribute> & attributes = *reinterpret_cast<list<Attribute>*>(c_attributes->p_list);

	bool errorOccured = false;

	if (attributes.size() > 2)
	{
		PrintError("[E,%d]: Prolog can contain only \"version\" and \"encoding\".\n", nameLine);
		return false;
	}

	if (name != "xml")
	{
		PrintError("[E,%d]: Prolog must start with xml word.\n", nameLine);
		errorOccured = true;
	}

	auto it = attributes.begin();

	if ((*it).name() != "version")
	{
		PrintError("[E,%d]: Prolog 1st attribute should be \"version\".\n", (*it).nameLine());
		errorOccured = true;
	}
	else if ((*it).value() != "1.0")
	{
		PrintError("[E,%d]: Prolog \"version\" attribute can only contain \"1.0\".\n", (*it).valueLine());
		errorOccured = true;
	}

	it++;
	if (it == attributes.end())
	{
		return errorOccured;
	}

	if ((*it).name() != "encoding")
	{
		PrintError("[E,%d]: Prolog 2d attribute should be \"encoding\".\n", (*it).nameLine());
		errorOccured = true;
	}
	else if (_stricmp((*it).value().c_str(), "ASCII") != 0 &&
			_stricmp((*it).value().c_str(), "Windows-1252") != 0 &&
			_stricmp((*it).value().c_str(), "UTF-8") != 0 &&
			_stricmp((*it).value().c_str(), "ISO-8859-1") != 0)
	{
		PrintError("[E,%d]: Prolog \"encoding\" attribute can only contain \"ASCII\", \
\"Windows-1252\", \"UTF-8\", \"ISO-8859-1\" in any case.\n", (*it).valueLine());

		errorOccured = true;
	}

	return errorOccured;
}

