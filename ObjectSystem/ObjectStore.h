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

class OBJECTSYSTEM_API ObjectStore
{
protected:
	std::map<int, Object_ptr> objects;

public:
	// Get

	Object_ptr get(int id);

	Object_ptr get_any_type();
	Object_ptr get_int_type();
	Object_ptr get_float_type();
	Object_ptr get_string_type();
	Object_ptr get_boolean_type();
	Object_ptr get_none_type();

	Object_ptr get_true_literal_type();
	Object_ptr get_false_literal_type();

	Object_ptr get_true_object();
	Object_ptr get_false_object();

	// Make

	Object_ptr make_object(bool value);
	Object_ptr make_object(int value);
	Object_ptr make_object(double value);
	Object_ptr make_object(std::wstring value);
	Object_ptr make_error_object(std::wstring text);

	ObjectStore();
};

class OBJECTSYSTEM_API ConstantPool : public ObjectStore
{
public:
	int next_id;

	int allocate();
	int allocate(int value);
	int allocate(double value);
	int allocate(std::wstring value);
	int allocate(Object_ptr value);
	int allocate_type(Object_ptr value);
	int allocate_enum_member(int enum_id, int enum_member_id);

	ConstantPool() : ObjectStore(), next_id(10) {};
};

class OBJECTSYSTEM_API DefinitionStore : public ObjectStore
{
public:
	void create(int id, Object_ptr value);
	void set(int id, Object_ptr value);
	void discard(int id);

	DefinitionStore() : ObjectStore() {};
};

using ObjectStore_ptr = OBJECTSYSTEM_API std::shared_ptr<ObjectStore>;
using ConstantPool_ptr = OBJECTSYSTEM_API std::shared_ptr<ConstantPool>;
using DefinitionStore_ptr = OBJECTSYSTEM_API std::shared_ptr<DefinitionStore>;
