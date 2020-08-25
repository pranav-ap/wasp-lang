#pragma once

#include "Type.h"
#include "Expression.h"
#include <memory>
#include <map>

// TypePool

class TypePool
{
	std::map<int, Type_ptr> type_pool;
public:
};

using TypePool_ptr = std::shared_ptr<TypePool>;

// TypeSystem

class TypeSystem
{
	TypePool_ptr pool;

public:
	bool check(const Expression_ptr expression, const Type_ptr type) const;
	Type_ptr infer(const Expression_ptr expression) const;
};

using TypeSystem_ptr = std::shared_ptr<TypeSystem>;
