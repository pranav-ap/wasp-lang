#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include "CodeObject.h"
#include <optional>
#include <string>
#include <deque>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <variant>
#include <functional>

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
struct MapType;
struct VariantType;
struct NoneType;
struct EnumType;
struct FunctionType;

// Object

struct IntObject;
struct FloatObject;
struct StringObject;
struct BooleanObject;
struct ListObject;
struct TupleObject;
struct SetObject;
struct MapObject;
struct VariantObject;
struct NoneObject;
struct ReturnObject;
struct YieldObject;
struct ErrorObject;
struct RedoObject;
struct ContinueObject;
struct BreakObject;
struct IteratorObject;
struct EnumObject;
struct EnumMemberObject;
struct FunctionObject;
struct BuiltInFunctionObject;

using Object = OBJECTSYSTEM_API std::variant<
	std::monostate,

	IntObject, FloatObject, StringObject, BooleanObject, NoneObject,
	ListObject, TupleObject, SetObject, MapObject, VariantObject,
	ReturnObject, ErrorObject, YieldObject, RedoObject, BreakObject,
	ContinueObject, IteratorObject, EnumObject, EnumMemberObject,
	FunctionObject, BuiltInFunctionObject,

	AnyType, IntLiteralType, FloatLiteralType, StringLiteralType, BooleanLiteralType,
	IntType, FloatType, StringType, BooleanType, ListType, TupleType, SetType,
	MapType, VariantType, NoneType, EnumType, FunctionType
>;

using Object_ptr = OBJECTSYSTEM_API std::shared_ptr<Object>;
using ObjectVector = OBJECTSYSTEM_API std::vector<Object_ptr>;
using StringVector = OBJECTSYSTEM_API std::vector<std::wstring>;

struct OBJECTSYSTEM_API AbstractObject
{
};

struct OBJECTSYSTEM_API ScalarObject : public AbstractObject
{
};

struct OBJECTSYSTEM_API CompositeObject : public AbstractObject
{
};

struct OBJECTSYSTEM_API ActionObject : public AbstractObject
{
};

struct OBJECTSYSTEM_API NoneObject : public AbstractObject
{
};

struct IterableAbstractObject
{
	virtual Object_ptr get_iter() = 0;
};

// Scalar Objects

struct OBJECTSYSTEM_API IntObject : public ScalarObject
{
	int value;
	IntObject(int value) : value(value) {};
};

struct OBJECTSYSTEM_API FloatObject : public ScalarObject
{
	double value;
	FloatObject(double value) : value(value) {};
};

struct OBJECTSYSTEM_API StringObject : public ScalarObject, public IterableAbstractObject
{
	std::wstring value;
	StringObject(std::wstring value) : value(value) {};

	virtual Object_ptr get_iter();
};

struct OBJECTSYSTEM_API BooleanObject : public ScalarObject
{
	bool value;
	BooleanObject(bool value) : value(value) {};
};

// Composite Objects

struct OBJECTSYSTEM_API EnumMemberObject : public CompositeObject
{
	int enum_id;
	int member_id;

	EnumMemberObject(int enum_id, int member_id)
		: enum_id(enum_id), member_id(member_id) {};
};

struct OBJECTSYSTEM_API ListObject : public CompositeObject, public IterableAbstractObject
{
	std::deque<Object_ptr> values;

	Object_ptr append(Object_ptr value);
	Object_ptr prepend(Object_ptr value);

	Object_ptr pop_back();
	Object_ptr pop_front();

	Object_ptr get(Object_ptr index);
	Object_ptr set(Object_ptr index, Object_ptr value);

	void clear();
	bool is_empty();
	int get_length();

	virtual Object_ptr get_iter();

	ListObject(ObjectVector values);
};

struct OBJECTSYSTEM_API TupleObject : public CompositeObject
{
	ObjectVector values;

	TupleObject(ObjectVector values) : values(values) {};

	Object_ptr get(Object_ptr index);
	Object_ptr set(Object_ptr index, Object_ptr value);
	Object_ptr set(ObjectVector values);

	int get_length();
};

struct OBJECTSYSTEM_API SetObject : public CompositeObject, public IterableAbstractObject
{
	ObjectVector values;

	SetObject(ObjectVector values) : values(values) {};

	ObjectVector get();
	Object_ptr set(ObjectVector values);

	virtual Object_ptr get_iter();

	int get_length();
};

struct OBJECTSYSTEM_API MapObject : public CompositeObject, public IterableAbstractObject
{
	std::map<Object_ptr, Object_ptr> pairs;

	Object_ptr insert(Object_ptr key, Object_ptr value);
	Object_ptr get_pair(Object_ptr key);
	Object_ptr get(Object_ptr key);
	Object_ptr set(Object_ptr key, Object_ptr value);

	virtual Object_ptr get_iter();

	int get_size();
};

struct OBJECTSYSTEM_API IteratorObject : public CompositeObject
{
	ObjectVector vec;
	ObjectVector::iterator it;

	IteratorObject(ObjectVector& vec) : vec(vec), it(std::begin(vec)) {};
	std::optional<Object_ptr> get_next();
	void reset_iter();
};

struct OBJECTSYSTEM_API VariantObject : public CompositeObject
{
	Object_ptr value;

	VariantObject(Object_ptr value) : value(std::move(value)) {};

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

// Enum

struct OBJECTSYSTEM_API EnumObject : public AbstractObject
{
	std::wstring name;
	std::map<std::wstring, int> members;

	EnumObject(std::wstring name, std::map<std::wstring, int> members)
		: name(name), members(members) {};
};

// Callable object

struct OBJECTSYSTEM_API SubroutineObject : public AbstractObject
{
	std::wstring name;
	CodeObject_ptr code;

	SubroutineObject(std::wstring name, std::vector<std::byte> instructions)
		: name(name), code(std::make_shared<CodeObject>(instructions)) {};
};

struct OBJECTSYSTEM_API FunctionObject : public SubroutineObject
{
	FunctionObject(std::wstring name, std::vector<std::byte> instructions)
		: SubroutineObject(name, instructions) {};
};

// Builtins

struct OBJECTSYSTEM_API BuiltInObject : public AbstractObject
{
	std::wstring module_name;
	std::wstring name;

	Object_ptr type;

	BuiltInObject(std::wstring module_name, std::wstring name)
		: module_name(module_name), name(name), type(nullptr) {};

	BuiltInObject(std::wstring module_name, std::wstring name, Object_ptr type)
		: module_name(module_name), name(name), type(type) {};
};

struct OBJECTSYSTEM_API BuiltInFunctionObject : public BuiltInObject
{
	std::function<Object_ptr(std::vector<Object_ptr>)> func;

	BuiltInFunctionObject(std::wstring module_name, std::wstring name, std::function<Object_ptr(std::vector<Object_ptr>)> func)
		: BuiltInObject(module_name, name), func(func) {};

	BuiltInFunctionObject(std::wstring module_name, std::wstring name, Object_ptr type, std::function<Object_ptr(std::vector<Object_ptr>)> func)
		: BuiltInObject(module_name, name, type), func(func) {};
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

	CallableType(ObjectVector input_types)
		: input_types(input_types), return_type(std::nullopt) {};

	CallableType(ObjectVector input_types, Object_ptr return_type)
		: input_types(input_types), return_type(std::make_optional(return_type)) {};
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
	Object_ptr element_type;
	SetType(Object_ptr element_type) : element_type(element_type) {};
};

struct OBJECTSYSTEM_API MapType : public CompositeType
{
	Object_ptr key_type;
	Object_ptr value_type;

	MapType(Object_ptr key_type, Object_ptr value_type)
		: key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct OBJECTSYSTEM_API VariantType : public CompositeType
{
	ObjectVector types;
	VariantType(ObjectVector types) : types(types) {};
};

struct OBJECTSYSTEM_API EnumType : public CompositeType
{
	std::wstring enum_name;
	std::map<std::wstring, int> members;

	EnumType(std::wstring enum_name, std::map<std::wstring, int> members)
		: enum_name(enum_name), members(members) {};
};

// Callable Type

struct OBJECTSYSTEM_API FunctionType : public CallableType
{
	FunctionType(ObjectVector input_types)
		: CallableType(input_types) {};

	FunctionType(ObjectVector input_types, Object_ptr return_type)
		: CallableType(input_types, return_type) {};
};

// Utils

OBJECTSYSTEM_API std::wstring stringify_object(Object_ptr value);
OBJECTSYSTEM_API ObjectVector to_vector(std::deque<Object_ptr> values);
OBJECTSYSTEM_API ObjectVector to_vector(std::wstring text);
OBJECTSYSTEM_API bool are_equal(Object_ptr left, Object_ptr right);
OBJECTSYSTEM_API bool are_equal(ObjectVector left_vector, ObjectVector right_vector);
