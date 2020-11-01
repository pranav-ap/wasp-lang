#pragma once

#include "Type.h"
#include "Expression.h"
#include "SymbolScope.h"
#include <memory>
#include <map>

class TypeSystem
{
	int next_type_id;
	std::map<int, Type_ptr> type_pool;

public:
	TypeSystem();

	bool equal(SymbolScope_ptr scope, const Type_ptr type_1, const Type_ptr type_2) const;
	bool equal(SymbolScope_ptr scope, const TypeVector type_vector_1, const TypeVector type_vector_2) const;

	bool assignable(SymbolScope_ptr scope, const Type_ptr lhs_type, const Type_ptr rhs_type) const;
	bool assignable(SymbolScope_ptr scope, const TypeVector type_vector_1, const TypeVector type_vector_2) const;

	Type_ptr infer_type(SymbolScope_ptr scope, Type_ptr left_type, WTokenType op, Type_ptr right_type);
	Type_ptr infer_type(SymbolScope_ptr scope, Type_ptr left_type, WTokenType op);

	Type_ptr allocate(SymbolScope_ptr scope, Type_ptr type);
	Type_ptr get(int id);

	// Is _ type?

	bool is_boolean_type(const Type_ptr type) const;
	bool is_number_type(const Type_ptr type) const;
	bool is_int_type(const Type_ptr type) const;
	bool is_float_type(const Type_ptr type) const;
	bool is_string_type(const Type_ptr type) const;
	bool is_none_type(const Type_ptr type) const;

	bool is_class_type(const Type_ptr type) const;
	bool is_interface_type(const Type_ptr type) const;
	bool is_enum_type(const Type_ptr type) const;

	bool is_function_type(const Type_ptr type) const;
	bool is_generator_type(const Type_ptr type) const;
	bool is_function_method_type(const Type_ptr type) const;
	bool is_generator_method_type(const Type_ptr type) const;

	bool is_condition_type(SymbolScope_ptr scope, const Type_ptr condition_type) const;
	bool is_spreadable_type(SymbolScope_ptr scope, const Type_ptr condition_type) const;
	bool is_iterable_type(SymbolScope_ptr scope, const Type_ptr type) const;
	bool is_key_type(SymbolScope_ptr scope, const Type_ptr type) const;

	// assert type

	void expect_boolean_type(const Type_ptr type) const;
	void expect_number_type(const Type_ptr type) const;
	void expect_int_type(const Type_ptr type) const;
	void expect_float_type(const Type_ptr type) const;
	void expect_string_type(const Type_ptr type) const;
	void expect_none_type(const Type_ptr type) const;

	void expect_class_type(const Type_ptr type) const;
	void expect_interface_type(const Type_ptr type) const;
	void expect_enum_type(const Type_ptr type) const;

	void expect_function_type(const Type_ptr type) const;
	void expect_generator_type(const Type_ptr type) const;
	void expect_function_method_type(const Type_ptr type) const;
	void expect_generator_method_type(const Type_ptr type) const;

	void expect_condition_type(SymbolScope_ptr scope, const Type_ptr condition_type) const;
	void expect_spreadable_type(SymbolScope_ptr scope, const Type_ptr condition_type) const;
	void expect_iterable_type(SymbolScope_ptr scope, const Type_ptr type) const;
	void expect_key_type(SymbolScope_ptr scope, const Type_ptr type) const;

	// Get primitive type

	Type_ptr get_boolean_type() const;
	Type_ptr get_int_type() const;
	Type_ptr get_float_type() const;
	Type_ptr get_string_type() const;
	Type_ptr get_none_type() const;

	// Extract type from variant

	ClassType* extract_class_type(const Type_ptr type) const;
	InterfaceType* extract_interface_type(const Type_ptr type) const;
	EnumType* extract_enum_type(const Type_ptr type) const;
	FunctionType* extract_function_type(const Type_ptr type) const;
	GeneratorType* extract_generator_type(const Type_ptr type) const;
	FunctionMemberType* extract_function_member_type(const Type_ptr type) const;
	GeneratorMemberType* extract_generator_member_type(const Type_ptr type) const;
};

using TypeSystem_ptr = std::shared_ptr<TypeSystem>;
