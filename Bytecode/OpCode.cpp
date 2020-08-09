#include "pch.h"
#include "OpCode.h"

OpCodeDefinition_ptr lookup_definition(OpCode opcode)
{
	auto definition = OpCodeDefinitionMap.at(opcode);
	return definition;
}