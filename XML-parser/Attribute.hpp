#pragma once

#include <string>

using namespace std;

class Attribute
{
public:
	Attribute(string name, int nameLine, string value, int valueLine)
		: _name(name), _nameLine(nameLine), _value(value), _valueLine(valueLine) {}

	string	name()		{ return this->_name; }
	string	value()		{ return this->_value; }
	int		nameLine()	{ return this->_nameLine; }
	int		valueLine() { return this->_valueLine; }

private:
	string _name;
	string _value;
	int _nameLine;
	int _valueLine;
};