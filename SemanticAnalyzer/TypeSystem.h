#pragma once

#include "Type.h"
#include "Expression.h"
#include "SymbolScope.h"
#include <memory>
#include <map>

class TypeSystem
{
	std::map<int, Type_ptr> type_pool;

public:
	TypeSystem();

	bool is_boolean_type(const Type_ptr type) const;
	bool is_number_type(const Type_ptr type) const;
	bool is_string_type(const Type_ptr type) const;
	bool is_none_type(const Type_ptr type) const;

	bool is_iterable_type(const Type_ptr type) const;
	bool is_key_type(const Type_ptr key_type) const;

	bool equal(SymbolScope_ptr scope, const Type_ptr type_1, const Type_ptr type_2) const;
	bool equal(SymbolScope_ptr scope, const TypeVector type_vector_1, const TypeVector type_vector_2) const;

	bool assignable(SymbolScope_ptr scope, const Type_ptr lhs_type, const Type_ptr rhs_type) const;
	bool assignable(SymbolScope_ptr scope, VariantType const& lhs_variant_type, Type_ptr const& rhs_type) const;

	Type_ptr get_boolean_type() const;
	Type_ptr get_number_type() const;
	Type_ptr get_string_type() const;
	Type_ptr get_none_type() const;
};

using TypeSystem_ptr = std::shared_ptr<TypeSystem>;

;