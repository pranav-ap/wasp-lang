#pragma once

#ifdef VM_EXPORTS
#define VM_API __declspec(dllexport)
#else
#define VM_API __declspec(dllimport)
#endif

#include "MemorySystem.h"
#include "ObjectSystem.h"

#include <stack>

// Frame

struct Frame
{
};

using Frame_ptr = std::shared_ptr<Frame>;

// VM

class VM_API VM
{
	ConstantPool_ptr constant_pool;
	CodeSection_ptr code_section;

	std::stack<int> eval_stack;
	std::stack<Frame_ptr> frame_stack;

private:
	void eval_stack_push(int value);
	int eval_stack_pop();

	void frame_stack_push(Frame_ptr frame);
	Frame_ptr frame_stack_pop();

	void execute_unary_operation();
	void execute_binary_integer_operation();
	void execute_binary_string_operation();
	void execute_comparison_operation();

public:
	VM() :
		constant_pool(std::make_shared<ConstantPool>()),
		code_section(std::make_shared<CodeSection>()) {};

	VM(MemorySystem_ptr memory)
	{
		constant_pool = memory->get_constant_pool();
		code_section = memory->get_code_section();
	}

	void execute();
};

using VM_ptr = std::shared_ptr<VM>;
