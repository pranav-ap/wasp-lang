#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include <optional>
#include <string>
#include <deque>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <variant>

// Type Objects

struct AnyType;
struct IntLiteralType;
struct FloatLiteralType;
struct StringLiteralType;
struct BooleanLiteralType;
struct IntType;
struct FloatType;
struct StringType;
struct BooleanType;
struct ListType;
struct TupleType;
struct SetType;
struct ClassType;
struct AliasType;
struct InterfaceType;
struct MapType;
struct EnumType;
struct VariantType;
struct NoneType;
struct FunctionType;
struct GeneratorType;
struct FunctionMemberType;
struct GeneratorMemberType;

// Object

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
struct RedoObject;
struct BuiltInsObject;
struct NoneObject;
struct FunctionObject;
struct GeneratorObject;
struct FunctionMethodObject;
struct GeneratorMethodObject;
struct EnumObject;
struct ClassObject;
struct InstanceObject;

using Object = OBJECTSYSTEM_API std::variant <
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
	ErrorObject, RedoObject,
	BreakObject, ContinueObject,
	BuiltInsObject,

	// Complex Objects

	EnumObject,
	FunctionObject, GeneratorObject,
	FunctionMethodObject, GeneratorMethodObject,
	ClassObject, InstanceObject,

	// Type Objects

	AnyType,

	IntLiteralType, FloatLiteralType, StringLiteralType, BooleanLiteralType,
	IntType, FloatType, StringType, BooleanType,

	ListType, TupleType, SetType, MapType,

	ClassType, AliasType, InterfaceType, 
	EnumType, VariantType, NoneType,

	FunctionType, GeneratorType,
	FunctionMemberType, GeneratorMemberType
> ;

using Object_ptr = OBJECTSYSTEM_API std::shared_ptr<Object>;
using ObjectVector = OBJECTSYSTEM_API std::vector<Object_ptr>;
using StringVector = OBJECTSYSTEM_API std::vector<std::wstring>;

struct OBJECTSYSTEM_API AbstractObject
{
};

struct OBJECTSYSTEM_API TypedObject
{
	Object_ptr type;

	TypedObject(Object_ptr type) : type(type) {};
};

struct OBJECTSYSTEM_API ScalarObject : public AbstractObject, public TypedObject
{
	ScalarObject(Object_ptr type) : TypedObject(type) {};
};

struct OBJECTSYSTEM_API CompositeObject : public AbstractObject, public TypedObject
{
	CompositeObject(Object_ptr type) : TypedObject(type) {};
};

struct OBJECTSYSTEM_API ActionObject : public AbstractObject
{
};

struct OBJECTSYSTEM_API NoneObject : public AbstractObject, public TypedObject
{
	NoneObject(Object_ptr type) : TypedObject(type) {};
};

struct OBJECTSYSTEM_API SubroutineObject : public AbstractObject, public TypedObject
{
	std::wstring name;
	std::vector<std::byte> instructions;
	int parameter_count;

	SubroutineObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count, Object_ptr type)
		: TypedObject(type), name(name), instructions(instructions), parameter_count(parameter_count) {};
};

struct OBJECTSYSTEM_API FunctionObject : public SubroutineObject
{
	FunctionObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count, Object_ptr type)
		: SubroutineObject(name, instructions, parameter_count, type) {};
};

struct OBJECTSYSTEM_API GeneratorObject : public SubroutineObject
{
	GeneratorObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count, Object_ptr type)
		: SubroutineObject(name, instructions, parameter_count, type) {};
};

struct OBJECTSYSTEM_API FunctionMethodObject : public SubroutineObject
{
	FunctionMethodObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count, Object_ptr type)
		: SubroutineObject(name, instructions, parameter_count, type) {};
};

struct OBJECTSYSTEM_API GeneratorMethodObject : public SubroutineObject
{
	GeneratorMethodObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count, Object_ptr type)
		: SubroutineObject(name, instructions, parameter_count, type) {};
};

struct OBJECTSYSTEM_API ClassObject : public AbstractObject, public TypedObject
{
	std::wstring name;
	std::map<std::wstring, int> members;

	ClassObject(std::wstring name, std::map<std::wstring, int> members, Object_ptr type)
		: TypedObject(type), name(name), members(members) {};
};

struct OBJECTSYSTEM_API InstanceObject : public AbstractObject, public TypedObject
{
	std::map<std::wstring, int> members;

	InstanceObject(std::map<std::wstring, int> members, Object_ptr type)
		: TypedObject(type), members(members) {};
};

struct OBJECTSYSTEM_API EnumObject : public AbstractObject, public TypedObject
{
	std::wstring name;
	std::map<std::wstring, int> members;

	EnumObject(std::wstring name, std::map<std::wstring, int> members, Object_ptr type)
		: TypedObject(type), name(name), members(members) {};
};

// Scalar Objects

struct OBJECTSYSTEM_API IntObject : public ScalarObject
{
	int value;
	IntObject(int value, Object_ptr type) : ScalarObject(type), value(value) {};
};

struct OBJECTSYSTEM_API FloatObject : public ScalarObject
{
	double value;
	FloatObject(double value, Object_ptr type) : ScalarObject(type), value(value) {};
};

struct OBJECTSYSTEM_API StringObject : public ScalarObject
{
	std::wstring value;
	StringObject(std::wstring value, Object_ptr type) : ScalarObject(type), value(value) {};
};

struct OBJECTSYSTEM_API BooleanObject : public ScalarObject
{
	bool value;
	BooleanObject(bool value, Object_ptr type) : ScalarObject(type), value(value) {};
};

// Composite Objects

struct OBJECTSYSTEM_API ListObject : public CompositeObject
{
	std::deque<Object_ptr> values;

	ListObject(Object_ptr type) : CompositeObject(type) {};

	Object_ptr append(Object_ptr value);
	Object_ptr prepend(Object_ptr value);

	Object_ptr pop_back();
	Object_ptr pop_front();

	Object_ptr get(Object_ptr index);
	Object_ptr set(Object_ptr index, Object_ptr value);

	void clear();
	bool is_empty();
	int get_length();
};

struct OBJECTSYSTEM_API TupleObject : public CompositeObject
{
	ObjectVector values;

	TupleObject(ObjectVector values, Object_ptr type) : CompositeObject(type), values(values) {};

	Object_ptr get(Object_ptr index);
	Object_ptr set(Object_ptr index, Object_ptr value);
	Object_ptr set(ObjectVector values);

	int get_length();
};

struct OBJECTSYSTEM_API SetObject : public CompositeObject
{
	ObjectVector values;

	SetObject(ObjectVector values, Object_ptr type) : CompositeObject(type), values(values) {};

	ObjectVector get();
	Object_ptr set(ObjectVector values);

	int get_length();
};

struct OBJECTSYSTEM_API MapObject : public CompositeObject
{
	std::map<Object_ptr, Object_ptr> pairs;

	MapObject(Object_ptr type) : CompositeObject(type) {};

	Object_ptr insert(Object_ptr key, Object_ptr value);
	Object_ptr get_pair(Object_ptr key);
	Object_ptr get(Object_ptr key);
	Object_ptr set(Object_ptr key, Object_ptr value);

	int get_size();
};

struct OBJECTSYSTEM_API EnumMemberObject : public CompositeObject
{
	int member_id;
	EnumMemberObject(int member_id, Object_ptr type) : CompositeObject(type), member_id(member_id) {};
};

struct OBJECTSYSTEM_API VariantObject : public CompositeObject
{
	Object_ptr value;

	VariantObject(Object_ptr value, Object_ptr type) : CompositeObject(type), value(std::move(value)) {};

	bool has_value();
};

// Action Objects

struct OBJECTSYSTEM_API BreakObject : public ActionObject
{
};

struct OBJECTSYSTEM_API ContinueObject : public ActionObject
{
};

struct OBJECTSYSTEM_API RedoObject : public ActionObject
{
};

struct OBJECTSYSTEM_API BuiltInsObject : public ActionObject, public TypedObject
{
	BuiltInsObject(Object_ptr type) : TypedObject(type) {};
};

struct OBJECTSYSTEM_API ReturnObject : public ActionObject
{
	std::optional<Object_ptr> value;

	ReturnObject() : value(std::nullopt) {};
	ReturnObject(Object_ptr value) : value(std::optional(std::move(value))) {};
};

struct OBJECTSYSTEM_API YieldObject : public ActionObject
{
	std::optional<Object_ptr> value;

	YieldObject() : value(std::nullopt) {};
	YieldObject(Object_ptr value) : value(std::optional(std::move(value))) {};
};

struct OBJECTSYSTEM_API ErrorObject : public ActionObject
{
	std::wstring message;

	ErrorObject() : message(L"") {};
	ErrorObject(std::wstring message) : message(message) {};
};

// Type

struct OBJECTSYSTEM_API AnyType : public AbstractObject
{
};

struct OBJECTSYSTEM_API ScalarType : public AnyType
{
};

struct OBJECTSYSTEM_API LiteralType : public AnyType
{
};

struct OBJECTSYSTEM_API CompositeType : public AnyType
{
};

struct OBJECTSYSTEM_API NoneType : public AnyType
{
};

struct OBJECTSYSTEM_API CallableType : public AnyType
{
	ObjectVector input_types;
	std::optional<Object_ptr> return_type;

	CallableType(ObjectVector input_types, std::optional<Object_ptr> return_type)
		: input_types(input_types), return_type(return_type) {};
};

// Scalar Types

struct OBJECTSYSTEM_API IntType : public ScalarType
{
};

struct OBJECTSYSTEM_API FloatType : public ScalarType
{
};

struct OBJECTSYSTEM_API StringType : public ScalarType
{
};

struct OBJECTSYSTEM_API BooleanType : public ScalarType
{
};

// Literal Types

struct OBJECTSYSTEM_API IntLiteralType : public LiteralType
{
	int value;
	IntLiteralType(int value) : value(value) {};
};

struct OBJECTSYSTEM_API FloatLiteralType : public LiteralType
{
	double value;
	FloatLiteralType(double value) : value(value) {};
};

struct OBJECTSYSTEM_API StringLiteralType : public LiteralType
{
	std::wstring value;
	StringLiteralType(std::wstring value) : value(value) {};
};

struct OBJECTSYSTEM_API BooleanLiteralType : public LiteralType
{
	bool value;
	BooleanLiteralType(bool value) : value(value) {};
};

// Composite Types

struct OBJECTSYSTEM_API ListType : public CompositeType
{
	Object_ptr element_type;
	ListType(Object_ptr element_type) : element_type(std::move(element_type)) {};
};

struct OBJECTSYSTEM_API TupleType : public CompositeType
{
	ObjectVector element_types;
	TupleType(ObjectVector element_types) : element_types(element_types) {};
};

struct OBJECTSYSTEM_API SetType : public CompositeType
{
	ObjectVector element_types;
	SetType(ObjectVector element_types) : element_types(element_types) {};
};

struct OBJECTSYSTEM_API MapType : public CompositeType
{
	Object_ptr key_type;
	Object_ptr value_type;

	MapType(Object_ptr key_type, Object_ptr value_type)
		: key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct OBJECTSYSTEM_API AliasType : public CompositeType
{
	std::wstring name;
	Object_ptr type;

	AliasType(std::wstring name, Object_ptr type) : name(name), type(type) {};
};

struct OBJECTSYSTEM_API UserDefinedType : public CompositeType
{
	std::wstring name;

	StringVector interfaces;
	StringVector base_types;

	std::map<std::wstring, Object_ptr> members;
	std::map<std::wstring, bool> is_public_member;

	UserDefinedType(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, Object_ptr> members, std::map<std::wstring, bool> is_public_member)
		: name(name), interfaces(interfaces), base_types(base_types), members(members), is_public_member(is_public_member) {};
};

struct OBJECTSYSTEM_API ClassType : public UserDefinedType
{
	ClassType(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, Object_ptr> members, std::map<std::wstring, bool> is_public_member)
		: UserDefinedType(name, interfaces, base_types, members, is_public_member) {};
};

struct OBJECTSYSTEM_API InterfaceType : public UserDefinedType
{
	InterfaceType(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, Object_ptr> members, std::map<std::wstring, bool> is_public_member)
		: UserDefinedType(name, interfaces, base_types, members, is_public_member) {};
};

struct OBJECTSYSTEM_API EnumType : public CompositeType
{
	std::wstring enum_name;
	std::map<std::wstring, int> members;

	EnumType(std::wstring enum_name, std::map<std::wstring, int> members)
		: enum_name(enum_name), members(members) {};
};

struct OBJECTSYSTEM_API VariantType : public CompositeType
{
	ObjectVector types;
	VariantType(ObjectVector types) : types(types) {};
};

// Callable Type

struct OBJECTSYSTEM_API FunctionType : public CallableType
{
	FunctionType(ObjectVector input_types, std::optional<Object_ptr> return_type)
		: CallableType(input_types, return_type) {};
};

struct OBJECTSYSTEM_API GeneratorType : public CallableType
{
	GeneratorType(ObjectVector input_types, std::optional<Object_ptr> return_type)
		: CallableType(input_types, return_type) {};
};

struct OBJECTSYSTEM_API FunctionMemberType : public CallableType
{
	std::wstring type_name;

	FunctionMemberType(std::wstring type_name, ObjectVector input_types, std::optional<Object_ptr> return_type)
		: CallableType(input_types, return_type), type_name(type_name) {};
};

struct OBJECTSYSTEM_API GeneratorMemberType : public CallableType
{
	std::wstring type_name;

	GeneratorMemberType(std::wstring type_name, ObjectVector input_types, std::optional<Object_ptr> return_type)
		: CallableType(input_types, return_type), type_name(type_name) {};
};

// Utils

OBJECTSYSTEM_API std::wstring stringify_object(Object_ptr value);
