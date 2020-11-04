#pragma once

#include "TypeNode.h"
#include "Expression.h"
#include "SymbolScope.h"
#include <memory>
#include <map>

class TypeSystem
{
	int next_type_id;
	std::map<int, TypeNode_ptr> type_pool;

public:
	TypeSystem();

	bool equal(SymbolScope_ptr scope, const TypeNode_ptr type_1, const TypeNode_ptr type_2) const;
	bool equal(SymbolScope_ptr scope, const TypeNodeVector type_vector_1, const TypeNodeVector type_vector_2) const;

	bool assignable(SymbolScope_ptr scope, const TypeNode_ptr lhs_type, const TypeNode_ptr rhs_type) const;
	bool assignable(SymbolScope_ptr scope, const TypeNodeVector type_vector_1, const TypeNodeVector type_vector_2) const;

	TypeNode_ptr infer_type(SymbolScope_ptr scope, TypeNode_ptr left_type, WTokenType op, TypeNode_ptr right_type);
	TypeNode_ptr infer_type(SymbolScope_ptr scope, TypeNode_ptr left_type, WTokenType op);

	TypeNode_ptr allocate(SymbolScope_ptr scope, TypeNode_ptr type);
	TypeNode_ptr get(int id);

	// Is _ type?

	bool is_boolean_type(const TypeNode_ptr type) const;
	bool is_number_type(const TypeNode_ptr type) const;
	bool is_int_type(const TypeNode_ptr type) const;
	bool is_float_type(const TypeNode_ptr type) const;
	bool is_string_type(const TypeNode_ptr type) const;
	bool is_none_type(const TypeNode_ptr type) const;

	bool is_class_type(const TypeNode_ptr type) const;
	bool is_interface_type(const TypeNode_ptr type) const;
	bool is_enum_type(const TypeNode_ptr type) const;

	bool is_function_type(const TypeNode_ptr type) const;
	bool is_generator_type(const TypeNode_ptr type) const;
	bool is_function_method_type(const TypeNode_ptr type) const;
	bool is_generator_method_type(const TypeNode_ptr type) const;

	bool is_condition_type(SymbolScope_ptr scope, const TypeNode_ptr condition_type) const;
	bool is_spreadable_type(SymbolScope_ptr scope, const TypeNode_ptr condition_type) const;
	bool is_iterable_type(SymbolScope_ptr scope, const TypeNode_ptr type) const;
	bool is_key_type(SymbolScope_ptr scope, const TypeNode_ptr type) const;

	// assert type

	void expect_boolean_type(const TypeNode_ptr type) const;
	void expect_number_type(const TypeNode_ptr type) const;
	void expect_int_type(const TypeNode_ptr type) const;
	void expect_float_type(const TypeNode_ptr type) const;
	void expect_string_type(const TypeNode_ptr type) const;
	void expect_none_type(const TypeNode_ptr type) const;

	void expect_class_type(const TypeNode_ptr type) const;
	void expect_interface_type(const TypeNode_ptr type) const;
	void expect_enum_type(const TypeNode_ptr type) const;

	void expect_function_type(const TypeNode_ptr type) const;
	void expect_generator_type(const TypeNode_ptr type) const;
	void expect_function_method_type(const TypeNode_ptr type) const;
	void expect_generator_method_type(const TypeNode_ptr type) const;

	void expect_condition_type(SymbolScope_ptr scope, const TypeNode_ptr condition_type) const;
	void expect_spreadable_type(SymbolScope_ptr scope, const TypeNode_ptr condition_type) const;
	void expect_iterable_type(SymbolScope_ptr scope, const TypeNode_ptr type) const;
	void expect_key_type(SymbolScope_ptr scope, const TypeNode_ptr type) const;

	// Get primitive type

	TypeNode_ptr get_boolean_type() const;
	TypeNode_ptr get_int_type() const;
	TypeNode_ptr get_float_type() const;
	TypeNode_ptr get_string_type() const;
	TypeNode_ptr get_none_type() const;

	// Extract type from variant

	ClassTypeNode* extract_class_type(const TypeNode_ptr type) const;
	InterfaceTypeNode* extract_interface_type(const TypeNode_ptr type) const;
	EnumTypeNode* extract_enum_type(const TypeNode_ptr type) const;
	FunctionTypeNode* extract_function_type(const TypeNode_ptr type) const;
	GeneratorTypeNode* extract_generator_type(const TypeNode_ptr type) const;
	FunctionMemberTypeNode* extract_function_member_type(const TypeNode_ptr type) const;
	GeneratorMemberTypeNode* extract_generator_member_type(const TypeNode_ptr type) const;
};

using TypeSystem_ptr = std::shared_ptr<TypeSystem>;
