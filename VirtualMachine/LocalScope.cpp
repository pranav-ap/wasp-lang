#pragma once
#include "pch.h"
#include "VirtualMachine.h"
#include "Assertion.h"

void LocalScope::add(int id)
{
	local_declarations.insert(id);
}

void LocalScope::remove(int id)
{
	local_declarations.erase(id);
}
