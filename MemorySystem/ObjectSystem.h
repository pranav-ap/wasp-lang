#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include <optional>
#include <string>
#include <deque>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <variant>

struct IntObject;
struct FloatObject;
struct StringObject;
struct BooleanObject;
struct ListObject;
struct TupleObject;
struct SetObject;
struct MapObject;
struct EnumMemberObject;
struct VariantObject;
struct ReturnObject;
struct YieldObject;
struct ErrorObject;
struct BreakObject;
struct ContinueObject;
struct BuiltInsObject;
struct NoneObject;
struct FunctionObject;
struct GeneratorObject;
struct FunctionMethodObject;
struct GeneratorMethodObject;
struct EnumObject;
struct ClassObject;
struct InstanceObject;

using Object = MEMORYSYSTEM_API std::variant <
	std::monostate,

	// Scalar Objects
	IntObject, FloatObject, StringObject, BooleanObject,
	NoneObject,

	// Composite Objects
	ListObject, TupleObject, SetObject,
	EnumMemberObject,
	MapObject,
	VariantObject,

	// Action Objects
	ReturnObject, YieldObject,
	ErrorObject,
	BreakObject, ContinueObject,
	BuiltInsObject,

	// Complex Objects
	EnumObject,
	FunctionObject, GeneratorObject,
	FunctionMethodObject, GeneratorMethodObject,
	ClassObject, InstanceObject
> ;

using Object_ptr = MEMORYSYSTEM_API std::shared_ptr<Object>;

// Defining Objects

struct MEMORYSYSTEM_API BaseObject
{
	virtual std::wstring stringify() const = 0;
};

struct MEMORYSYSTEM_API ScalarObject : public BaseObject
{
	virtual std::wstring stringify() const = 0;
};

struct MEMORYSYSTEM_API CompositeObject : public BaseObject
{
	virtual std::wstring stringify() const = 0;
};

struct MEMORYSYSTEM_API ActionObject : public BaseObject
{
	virtual std::wstring stringify() const = 0;
};

struct MEMORYSYSTEM_API NoneObject : public BaseObject
{
	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API DefinitionObject : public BaseObject
{
	std::wstring name;
	std::vector<std::byte> instructions;
	int parameter_count;

	DefinitionObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count)
		: name(name), instructions(instructions), parameter_count(parameter_count) {};

	virtual std::wstring stringify() const = 0;
};

struct MEMORYSYSTEM_API FunctionObject : public DefinitionObject
{
	FunctionObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count)
		: DefinitionObject(name, instructions, parameter_count) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API GeneratorObject : public DefinitionObject
{
	GeneratorObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count)
		: DefinitionObject(name, instructions, parameter_count) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API FunctionMethodObject : public DefinitionObject
{
	FunctionMethodObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count)
		: DefinitionObject(name, instructions, parameter_count) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API GeneratorMethodObject : public DefinitionObject
{
	GeneratorMethodObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count)
		: DefinitionObject(name, instructions, parameter_count) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API ClassObject : public BaseObject
{
	std::wstring name;
	std::map<std::wstring, int> members;

	ClassObject() {};
	ClassObject(std::wstring name, std::map<std::wstring, int> members)
		: name(name), members(members) {};

	int lookup_member_id(std::wstring name);

	std::wstring stringify() const override;
};

using ClassObject_ptr = MEMORYSYSTEM_API std::shared_ptr<ClassObject>;

struct MEMORYSYSTEM_API InstanceObject : public BaseObject
{
	ClassObject_ptr type;
	std::map<std::wstring, int> members;

	InstanceObject(ClassObject_ptr type, std::map<std::wstring, int> members)
		: type(type), members(members) {};

	int lookup_member_id(std::wstring name);

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API EnumObject : public BaseObject
{
	std::wstring name;
	std::map<std::wstring, int> members;

	EnumObject() {};
	EnumObject(std::wstring name, std::map<std::wstring, int> members)
		: name(name), members(members) {};

	std::wstring stringify() const override;
};

// Scalar Objects

struct MEMORYSYSTEM_API IntObject : public ScalarObject
{
	int value;
	IntObject(int value) : value(value) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API FloatObject : public ScalarObject
{
	double value;
	FloatObject(double value) : value(value) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API StringObject : public ScalarObject
{
	std::wstring value;
	StringObject(std::wstring value) : value(value) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API BooleanObject : public ScalarObject
{
	bool value;
	BooleanObject(bool value) : value(value) {};

	std::wstring stringify() const override;
};

// Composite Objects

struct MEMORYSYSTEM_API ListObject : public CompositeObject
{
	std::deque<Object_ptr> values;

	ListObject() {};

	Object_ptr append(Object_ptr value);
	Object_ptr prepend(Object_ptr value);

	Object_ptr pop_back();
	Object_ptr pop_front();

	Object_ptr get(Object_ptr index);
	Object_ptr set(Object_ptr index, Object_ptr value);

	void clear();
	bool is_empty();
	int get_length();

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API TupleObject : public CompositeObject
{
	std::vector<Object_ptr> values;

	TupleObject(std::vector<Object_ptr> values) : values(values) {};

	Object_ptr get(Object_ptr index);
	Object_ptr set(Object_ptr index, Object_ptr value);
	Object_ptr set(std::vector<Object_ptr> values);

	int get_length();

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API SetObject : public CompositeObject
{
	std::vector<Object_ptr> values;

	SetObject(std::vector<Object_ptr> values) : values(values) {};

	std::vector<Object_ptr> get();
	Object_ptr set(std::vector<Object_ptr> values);

	int get_length();

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API MapObject : public CompositeObject
{
	std::map<Object_ptr, Object_ptr> pairs;

	MapObject() {};

	Object_ptr insert(Object_ptr key, Object_ptr value);
	Object_ptr get_pair(Object_ptr key);
	Object_ptr get(Object_ptr key);
	Object_ptr set(Object_ptr key, Object_ptr value);

	int get_size();

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API EnumMemberObject : public CompositeObject
{
	std::wstring enum_name;
	std::vector<std::wstring> member_names;
	int enum_member_index;

	EnumMemberObject(std::wstring enum_name, std::vector<std::wstring> member_names, int enum_member_index)
		: enum_name(enum_name), member_names(member_names), enum_member_index(enum_member_index) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API VariantObject : public CompositeObject
{
	Object_ptr value;

	VariantObject(Object_ptr value)
		: value(std::move(value)) {};

	bool has_value();
	std::wstring stringify() const override;
};

// Action Objects

struct MEMORYSYSTEM_API BreakObject : public ActionObject
{
	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API ContinueObject : public ActionObject
{
	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API BuiltInsObject : public ActionObject
{
	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API ReturnObject : public ActionObject
{
	std::optional<Object_ptr> value;

	ReturnObject()
		: value(std::nullopt) {};
	ReturnObject(Object_ptr value)
		: value(std::optional(std::move(value))) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API YieldObject : public ActionObject
{
	std::optional<Object_ptr> value;

	YieldObject()
		: value(std::nullopt) {};
	YieldObject(Object_ptr value)
		: value(std::optional(std::move(value))) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API ErrorObject : public ActionObject
{
	std::wstring message;

	ErrorObject()
		: message(L"") {};
	ErrorObject(std::wstring message)
		: message(message) {};

	std::wstring stringify() const override;
};

// Utils

MEMORYSYSTEM_API std::wstring stringify_object(Object_ptr value);
