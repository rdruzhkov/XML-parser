#pragma once

enum TagType
{
	Open,
	Close
};

struct str
{
	void * p_string;
	int line;
};

struct Attribute
{
	struct str name;
	struct str value;
};

