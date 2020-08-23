#pragma once

#include <memory>
#include <string>

struct CSymbol
{
	std::wstring name;
	int id;

	CSymbol(std::wstring name, int id)
		: name(name), id(id) {};
};

using CSymbol_ptr = std::shared_ptr<CSymbol>;
