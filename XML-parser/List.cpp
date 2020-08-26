
#include <list>

#include "List.h"
#include "Element.h"

using namespace std;

extern "C" void InitializeList(List * p_List)
{
	p_List->list = new list<Element>();
}