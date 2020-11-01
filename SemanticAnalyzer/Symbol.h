#pragma once

#include "Type.h"
#include <memory>
#include <string>

struct Symbol
{
	std::wstring name;

	bool is_public;
	bool is_mutable;

	Type_ptr type;

	Symbol(std::wstring name, Type_ptr type, bool is_public, bool is_mutable)
		: name(name), type(type), is_public(is_public), is_mutable(is_mutable) {};
};

using Symbol_ptr = std::shared_ptr<Symbol>;
