
#include <string>
#include <utility>

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
	printf("[I]: Exiting parser. Press any key to continue...\n\n");
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

extern "C" void AddToTagList(str name, TagType type)
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
		tags.end();
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
				PrintError("[E,%d]: Not closed open tag \"%s\".\n", (*improperlyNestedOpenTagIt).Line(), (*improperlyNestedOpenTagIt).Name().c_str());
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
					PrintError("[E,%d]: Not closed close tag \"%s\".\n", (*pair.second).Line(), (*pair.second).Name().c_str());
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
				PrintError("[E,%d]: Not closed close tag \"%s\".\n", (*improperlyNestedCloseTagIt).Line(), (*improperlyNestedCloseTagIt).Name().c_str());
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
					PrintError("[E,%d]: Not closed open tag \"%s\".\n", (*pair.second).Line(), (*pair.second).Name().c_str());
					tags.erase(pair.second);
				}
			}
		}
	}
}