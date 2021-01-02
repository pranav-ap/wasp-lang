#pragma once
#include "OpCode.h"
#include "CFG.h"
#include <memory>

#ifdef CFGRAPH_EXPORTS
#define CFGRAPH_API __declspec(dllexport)
#else
#define CFGRAPH_API __declspec(dllimport)
#endif

class CFGRAPH_API CFGAssembler
{	
public:
	CFGAssembler() {};
	CodeObject_ptr assemble(CFG_ptr cfg);
};

using CFGAssembler_ptr = CFGRAPH_API std::unique_ptr<CFGAssembler>;
