
#include <list>

#include "C_Attribute.h"
#include "C_Attributes.h"

#include "Attribute.hpp"
#include "Utility.hpp"

extern "C" void Init_C_Atributes(C_Attributes * attributes)
{
	attributes->p_list = new list<Attribute>();
}

extern "C" void Add_C_Attribute(C_Attribute * c_attr, C_Attributes * attributes)
{
	list<Attribute>* p_attributeList = reinterpret_cast<list<Attribute>*>(attributes->p_list);

	string* p_name = reinterpret_cast<string*>(c_attr->name.p_string);
	string* p_value = reinterpret_cast<string*>(c_attr->value.p_string);

	p_attributeList->push_back(Attribute(*p_name, c_attr->name.line, *p_value, c_attr->value.line));
}

extern "C" bool Validate_C_Attributes(C_Attributes * c_attributes)
{
	return ValidateAtrributeList( * reinterpret_cast<list<Attribute> *>( c_attributes->p_list ));
}