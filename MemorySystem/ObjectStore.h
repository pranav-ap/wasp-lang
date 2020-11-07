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

class MEMORYSYSTEM_API ObjectStore
{
	std::map<int, Object_ptr> objects;

public:
	std::map<int, std::wstring> name_map;

	void set(int id, Object_ptr value);
	Object_ptr get(int id);
};

using ObjectStore_ptr = MEMORYSYSTEM_API std::shared_ptr<ObjectStore>;
