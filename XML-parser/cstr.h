#pragma once

struct cstr
{
	void * p_string;
};

extern "C" void cstr_Init(struct cstr * p_cstr);
extern "C" void cstr_Destroy(struct cstr * p_cstr);
extern "C" void cstr_PushBack(struct cstr * p_cstr, char c);
extern "C" void cstr_Assign(struct cstr* p_cstr, char * value);