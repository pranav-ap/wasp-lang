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
struct OperatorType;
struct TypeIdentifier;

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
	ClassObject, InstanceObject,

	// Type Objects

	AnyType,

	IntLiteralType, FloatLiteralType, StringLiteralType, BooleanLiteralType,
	IntType, FloatType, StringType, BooleanType,

	ListType, TupleType, SetType,

	ClassType, AliasType, InterfaceType, MapType,
	EnumType, TypeIdentifier,

	VariantType, NoneType,

	FunctionType, GeneratorType,
	FunctionMemberType, GeneratorMemberType,
	OperatorType
> ;

using Object_ptr = MEMORYSYSTEM_API std::shared_ptr<Object>;
using ObjectVector = MEMORYSYSTEM_API std::vector<Object_ptr>;
using StringVector = MEMORYSYSTEM_API std::vector<std::wstring>;

struct MEMORYSYSTEM_API AbstractObject
{
	virtual std::wstring stringify() const = 0;
};

struct MEMORYSYSTEM_API TypedObject
{
	Object_ptr type;

	TypedObject(Object_ptr type) : type(type) {};
};

struct MEMORYSYSTEM_API ScalarObject : public AbstractObject, public TypedObject
{
	ScalarObject(Object_ptr type) : TypedObject(type) {};
	virtual std::wstring stringify() const = 0;
};

struct MEMORYSYSTEM_API CompositeObject : public AbstractObject, public TypedObject
{
	CompositeObject(Object_ptr type) : TypedObject(type) {};
	virtual std::wstring stringify() const = 0;
};

struct MEMORYSYSTEM_API ActionObject : public AbstractObject
{
	virtual std::wstring stringify() const = 0;
};

struct MEMORYSYSTEM_API NoneObject : public AbstractObject, public TypedObject
{
	NoneObject(Object_ptr type) : TypedObject(type) {};
	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API SubroutineObject : public AbstractObject, public TypedObject
{
	std::wstring name;
	std::vector<std::byte> instructions;
	int parameter_count;

	SubroutineObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count, Object_ptr type)
		: TypedObject(type), name(name), instructions(instructions), parameter_count(parameter_count) {};

	virtual std::wstring stringify() const = 0;
};

struct MEMORYSYSTEM_API FunctionObject : public SubroutineObject
{
	FunctionObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count, Object_ptr type)
		: SubroutineObject(name, instructions, parameter_count, type) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API GeneratorObject : public SubroutineObject
{
	GeneratorObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count, Object_ptr type)
		: SubroutineObject(name, instructions, parameter_count, type) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API FunctionMethodObject : public SubroutineObject
{
	FunctionMethodObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count, Object_ptr type)
		: SubroutineObject(name, instructions, parameter_count, type) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API GeneratorMethodObject : public SubroutineObject
{
	GeneratorMethodObject(std::wstring name, std::vector<std::byte> instructions, int parameter_count, Object_ptr type)
		: SubroutineObject(name, instructions, parameter_count, type) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API ClassObject : public AbstractObject, public TypedObject
{
	std::wstring name;
	std::map<std::wstring, int> members;

	ClassObject(std::wstring name, std::map<std::wstring, int> members, Object_ptr type)
		: TypedObject(type), name(name), members(members) {};

	std::wstring stringify() const override;
};

using ClassObject_ptr = MEMORYSYSTEM_API std::shared_ptr<ClassObject>;

struct MEMORYSYSTEM_API InstanceObject : public AbstractObject, public TypedObject
{
	std::map<std::wstring, int> members;

	InstanceObject(std::map<std::wstring, int> members, Object_ptr type)
		: TypedObject(type), members(members) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API EnumObject : public AbstractObject, public TypedObject
{
	std::wstring name;
	std::map<std::wstring, int> members;

	EnumObject(std::wstring name, std::map<std::wstring, int> members, Object_ptr type)
		: TypedObject(type), name(name), members(members) {};

	std::wstring stringify() const override;
};

// Scalar Objects

struct MEMORYSYSTEM_API IntObject : public ScalarObject
{
	int value;
	IntObject(int value, Object_ptr type) : ScalarObject(type), value(value) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API FloatObject : public ScalarObject
{
	double value;
	FloatObject(double value, Object_ptr type) : ScalarObject(type), value(value) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API StringObject : public ScalarObject
{
	std::wstring value;
	StringObject(std::wstring value, Object_ptr type) : ScalarObject(type), value(value) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API BooleanObject : public ScalarObject
{
	bool value;
	BooleanObject(bool value, Object_ptr type) : ScalarObject(type), value(value) {};

	std::wstring stringify() const override;
};

// Composite Objects

struct MEMORYSYSTEM_API ListObject : public CompositeObject
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

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API TupleObject : public CompositeObject
{
	ObjectVector values;

	TupleObject(ObjectVector values, Object_ptr type) : CompositeObject(type), values(values) {};

	Object_ptr get(Object_ptr index);
	Object_ptr set(Object_ptr index, Object_ptr value);
	Object_ptr set(ObjectVector values);

	int get_length();

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API SetObject : public CompositeObject
{
	ObjectVector values;

	SetObject(ObjectVector values, Object_ptr type) : CompositeObject(type), values(values) {};

	ObjectVector get();
	Object_ptr set(ObjectVector values);

	int get_length();

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API MapObject : public CompositeObject
{
	std::map<Object_ptr, Object_ptr> pairs;

	MapObject(Object_ptr type) : CompositeObject(type) {};

	Object_ptr insert(Object_ptr key, Object_ptr value);
	Object_ptr get_pair(Object_ptr key);
	Object_ptr get(Object_ptr key);
	Object_ptr set(Object_ptr key, Object_ptr value);

	int get_size();

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API EnumMemberObject : public CompositeObject
{
	int member_id;
	EnumMemberObject(int member_id, Object_ptr type) : CompositeObject(type), member_id(member_id) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API VariantObject : public CompositeObject
{
	Object_ptr value;

	VariantObject(Object_ptr value, Object_ptr type) : CompositeObject(type), value(std::move(value)) {};

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

struct MEMORYSYSTEM_API BuiltInsObject : public ActionObject, public TypedObject
{
	BuiltInsObject(Object_ptr type) : TypedObject(type) {};
	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API ReturnObject : public ActionObject
{
	std::optional<Object_ptr> value;

	ReturnObject() : value(std::nullopt) {};
	ReturnObject(Object_ptr value) : value(std::optional(std::move(value))) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API YieldObject : public ActionObject
{
	std::optional<Object_ptr> value;

	YieldObject() : value(std::nullopt) {};
	YieldObject(Object_ptr value) : value(std::optional(std::move(value))) {};

	std::wstring stringify() const override;
};

struct MEMORYSYSTEM_API ErrorObject : public ActionObject
{
	std::wstring message;

	ErrorObject() : message(L"") {};
	ErrorObject(std::wstring message) : message(message) {};

	std::wstring stringify() const override;
};

// Type

struct MEMORYSYSTEM_API AnyType
{
};

struct MEMORYSYSTEM_API ScalarType : public AnyType
{
};

struct MEMORYSYSTEM_API LiteralType : public AnyType
{
};

struct MEMORYSYSTEM_API CompositeType : public AnyType
{
};

struct MEMORYSYSTEM_API NoneType : public AnyType
{
};

struct MEMORYSYSTEM_API CallableType : public AnyType
{
	ObjectVector input_types;
	std::optional<Object_ptr> return_type;

	CallableType(ObjectVector input_types, std::optional<Object_ptr> return_type)
		: input_types(input_types), return_type(return_type) {};
};

struct MEMORYSYSTEM_API TypeIdentifier : public AnyType
{
	std::wstring name;

	TypeIdentifier(std::wstring name) : name(name) {};
};

// Scalar Types

struct MEMORYSYSTEM_API IntType : public ScalarType
{
};

struct MEMORYSYSTEM_API FloatType : public ScalarType
{
};

struct MEMORYSYSTEM_API StringType : public ScalarType
{
};

struct MEMORYSYSTEM_API BooleanType : public ScalarType
{
};

// Literal Types

struct MEMORYSYSTEM_API IntLiteralType : public LiteralType
{
	int value;
	IntLiteralType(int value) : value(value) {};
};

struct MEMORYSYSTEM_API FloatLiteralType : public LiteralType
{
	double value;
	FloatLiteralType(double value) : value(value) {};
};

struct MEMORYSYSTEM_API StringLiteralType : public LiteralType
{
	std::wstring value;
	StringLiteralType(std::wstring value) : value(value) {};
};

struct MEMORYSYSTEM_API BooleanLiteralType : public LiteralType
{
	bool value;
	BooleanLiteralType(bool value) : value(value) {};
};

// Composite Types

struct MEMORYSYSTEM_API ListType : public CompositeType
{
	Object_ptr element_type;
	ListType(Object_ptr element_type) : element_type(std::move(element_type)) {};
};

struct MEMORYSYSTEM_API TupleType : public CompositeType
{
	ObjectVector element_types;
	TupleType(ObjectVector element_types) : element_types(element_types) {};
};

struct MEMORYSYSTEM_API SetType : public CompositeType
{
	ObjectVector element_types;
	SetType(ObjectVector element_types) : element_types(element_types) {};
};

struct MEMORYSYSTEM_API MapType : public CompositeType
{
	Object_ptr key_type;
	Object_ptr value_type;

	MapType(Object_ptr key_type, Object_ptr value_type)
		: key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct MEMORYSYSTEM_API AliasType : public CompositeType
{
	std::wstring name;
	Object_ptr type;

	AliasType(std::wstring name, Object_ptr type) : name(name), type(type) {};
};

struct MEMORYSYSTEM_API UserDefinedType : public CompositeType
{
	std::wstring name;

	StringVector interfaces;
	StringVector base_types;

	std::map<std::wstring, Object_ptr> members;
	std::map<std::wstring, bool> is_public_member;

	UserDefinedType(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, Object_ptr> members, std::map<std::wstring, bool> is_public_member)
		: name(name), interfaces(interfaces), base_types(base_types), members(members), is_public_member(is_public_member) {};
};

struct MEMORYSYSTEM_API ClassType : public UserDefinedType
{
	ClassType(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, Object_ptr> members, std::map<std::wstring, bool> is_public_member)
		: UserDefinedType(name, interfaces, base_types, members, is_public_member) {};
};

struct MEMORYSYSTEM_API InterfaceType : public UserDefinedType
{
	InterfaceType(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, Object_ptr> members, std::map<std::wstring, bool> is_public_member)
		: UserDefinedType(name, interfaces, base_types, members, is_public_member) {};
};

struct MEMORYSYSTEM_API EnumType : public CompositeType
{
	std::wstring enum_name;
	std::map<std::wstring, int> members;

	EnumType(std::wstring enum_name, std::map<std::wstring, int> members)
		: enum_name(enum_name), members(members) {};
};

struct MEMORYSYSTEM_API VariantType : public CompositeType
{
	ObjectVector types;
	VariantType(ObjectVector types) : types(types) {};
};

// Callable Type

struct MEMORYSYSTEM_API FunctionType : public CallableType
{
	FunctionType(ObjectVector input_types, std::optional<Object_ptr> return_type)
		: CallableType(input_types, return_type) {};
};

struct MEMORYSYSTEM_API GeneratorType : public CallableType
{
	GeneratorType(ObjectVector input_types, std::optional<Object_ptr> return_type)
		: CallableType(input_types, return_type) {};
};

struct MEMORYSYSTEM_API FunctionMemberType : public CallableType
{
	std::wstring type_name;

	FunctionMemberType(std::wstring type_name, ObjectVector input_types, std::optional<Object_ptr> return_type)
		: CallableType(input_types, return_type), type_name(type_name) {};
};

struct MEMORYSYSTEM_API GeneratorMemberType : public CallableType
{
	std::wstring type_name;

	GeneratorMemberType(std::wstring type_name, ObjectVector input_types, std::optional<Object_ptr> return_type)
		: CallableType(input_types, return_type), type_name(type_name) {};
};

enum class MEMORYSYSTEM_API OperatorPosition
{
	Infix,
	Prefix,
	Postfix
};

struct MEMORYSYSTEM_API OperatorType : public CallableType
{
	OperatorPosition position;

	OperatorType(OperatorPosition position, ObjectVector input_types, std::optional<Object_ptr> return_type)
		: CallableType(input_types, return_type), position(position) {};
};

// Utils

MEMORYSYSTEM_API std::wstring stringify_object(Object_ptr value);
