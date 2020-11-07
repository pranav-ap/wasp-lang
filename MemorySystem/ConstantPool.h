#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include "Objects.h"
#include <memory>
#include <map>
#include <string> 

class MEMORYSYSTEM_API ConstantPool
{
	std::map<int, Object_ptr> pool;

public:
	int allocate();
	int allocate(int value);
	int allocate(double value);
	int allocate(std::wstring value);
	int allocate(Object_ptr value);

	Object_ptr get(int id);
	void set(int id, Object_ptr value);
};

using ConstantPool_ptr = MEMORYSYSTEM_API std::shared_ptr<ConstantPool>;
