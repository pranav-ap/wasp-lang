#pragma once

#include "pch.h"
#include "Info.h"

void FunctionInfo::add_overload(std::string signature, Arguments arguments, ReturnType return_type, Block block)
{
	table.insert_or_assign(signature, std::make_tuple(arguments, return_type, block));
}