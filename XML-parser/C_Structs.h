#pragma once

struct str
{
	void * p_string;
	int line;
};

struct Name
{
	struct str name;
	struct str _namespace;
};

struct Attribute
{
	struct Name name;
	struct str value;
};
