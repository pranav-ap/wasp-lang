#pragma once

#include <memory>
#include <string>
#include <utility>

struct CSymbol
{
	std::wstring name;
	int label;

	CSymbol(std::wstring name, int label)
		: name(name), label(label) {};
};

using CSymbol_ptr = std::shared_ptr<CSymbol>;
