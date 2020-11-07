#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include "Objects.h"
#include "ConstantPool.h"
#include "ObjectStore.h"
#include "SymbolScope.h"
#include "WTokenType.h"

#include <vector>
#include <utility>
#include <memory>

struct ObjectSystem
{
	ConstantPool_ptr constant_pool;
	ObjectStore_ptr object_store;

	ObjectSystem();

	// Type System

	Object_ptr get_any_type();
	Object_ptr get_int_type();
	Object_ptr get_float_type();
	Object_ptr get_string_type();
	Object_ptr get_bool_type();
	Object_ptr get_none_type();

	bool equal(SymbolScope_ptr scope, const Object_ptr type_1, const Object_ptr type_2) const;
	bool equal(SymbolScope_ptr scope, const ObjectVector type_vector_1, const ObjectVector type_vector_2) const;

	bool assignable(SymbolScope_ptr scope, const Object_ptr lhs_type, const Object_ptr rhs_type) const;
	bool assignable(SymbolScope_ptr scope, const ObjectVector type_vector_1, const ObjectVector type_vector_2) const;

	Object_ptr infer(SymbolScope_ptr scope, Object_ptr left_type, WTokenType op, Object_ptr right_type);
	Object_ptr infer(SymbolScope_ptr scope, Object_ptr left_type, WTokenType op);

	// Is _ type?

	bool is_boolean_type(const Object_ptr type) const;
	bool is_number_type(const Object_ptr type) const;
	bool is_int_type(const Object_ptr type) const;
	bool is_float_type(const Object_ptr type) const;
	bool is_string_type(const Object_ptr type) const;
	bool is_none_type(const Object_ptr type) const;
	bool is_class_type(const Object_ptr type) const;
	bool is_interface_type(const Object_ptr type) const;
	bool is_enum_type(const Object_ptr type) const;
	bool is_function_type(const Object_ptr type) const;
	bool is_generator_type(const Object_ptr type) const;
	bool is_function_method_type(const Object_ptr type) const;
	bool is_generator_method_type(const Object_ptr type) const;
	bool is_condition_type(SymbolScope_ptr scope, const Object_ptr condition_type) const;
	bool is_spreadable_type(SymbolScope_ptr scope, const Object_ptr condition_type) const;
	bool is_iterable_type(SymbolScope_ptr scope, const Object_ptr type) const;
	bool is_key_type(SymbolScope_ptr scope, const Object_ptr type) const;

	// assert type

	void expect_boolean_type(const Object_ptr type) const;
	void expect_number_type(const Object_ptr type) const;
	void expect_int_type(const Object_ptr type) const;
	void expect_float_type(const Object_ptr type) const;
	void expect_string_type(const Object_ptr type) const;
	void expect_none_type(const Object_ptr type) const;
	void expect_class_type(const Object_ptr type) const;
	void expect_interface_type(const Object_ptr type) const;
	void expect_enum_type(const Object_ptr type) const;
	void expect_function_type(const Object_ptr type) const;
	void expect_generator_type(const Object_ptr type) const;
	void expect_function_method_type(const Object_ptr type) const;
	void expect_generator_method_type(const Object_ptr type) const;
	void expect_condition_type(SymbolScope_ptr scope, const Object_ptr condition_type) const;
	void expect_spreadable_type(SymbolScope_ptr scope, const Object_ptr condition_type) const;
	void expect_iterable_type(SymbolScope_ptr scope, const Object_ptr type) const;
	void expect_key_type(SymbolScope_ptr scope, const Object_ptr type) const;

	// Extract type from variant

	ClassType* extract_class_type(const Object_ptr type) const;
	InterfaceType* extract_interface_type(const Object_ptr type) const;
	EnumType* extract_enum_type(const Object_ptr type) const;
	FunctionType* extract_function_type(const Object_ptr type) const;
	GeneratorType* extract_generator_type(const Object_ptr type) const;
	FunctionMemberType* extract_function_member_type(const Object_ptr type) const;
	GeneratorMemberType* extract_generator_member_type(const Object_ptr type) const;
};

using ObjectSystem_ptr = std::shared_ptr<ObjectSystem>;
