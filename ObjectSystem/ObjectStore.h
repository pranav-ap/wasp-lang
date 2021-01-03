#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include "Objects.h"
#include <memory>
#include <map>
#include <string>

struct OBJECTSYSTEM_API ObjectStore
{
	int next_id;

	std::map<int, Object_ptr> objects;

	int allocate();
	int allocate(int value);
	int allocate(double value);
	int allocate(std::wstring value);
	
	int allocate_enum(int enum_id, int enum_member_id);

	int allocate(Object_ptr value);

	void set(int id, Object_ptr value);
	Object_ptr get(int id);

	Object_ptr get_any_type();
	Object_ptr get_int_type();
	Object_ptr get_float_type();
	Object_ptr get_string_type();
	Object_ptr get_boolean_type();
	Object_ptr get_none_type();

	Object_ptr get_true_object();
	Object_ptr get_false_object();

	Object_ptr make_object(bool value);
	Object_ptr make_object(int value);
	Object_ptr make_object(double value);
	Object_ptr make_object(std::wstring value);
	
	Object_ptr make_error_object(std::wstring text);
	
	ObjectStore();
};

using ObjectStore_ptr = OBJECTSYSTEM_API std::shared_ptr<ObjectStore>;
