
#include <string>

#include "C_Structs.h"

using namespace std;

extern "C" void str_Init(str * p_str, const char * initValue)
{
	p_str->p_string = new string(initValue);
}

extern "C" void str_Destroy(str * p_str)
{
	delete(p_str->p_string);
}

extern "C" void str_PushBack(str * p_str, char c)
{
	string* p_cpp_str = reinterpret_cast<string *>(p_str->p_string);

	p_cpp_str->push_back(c);
}

extern "C" void str_Assign(struct str* p_str, char * value)
{
	string* p_cpp_str = reinterpret_cast<string*>(p_str->p_string);

	p_cpp_str->assign(value);
}