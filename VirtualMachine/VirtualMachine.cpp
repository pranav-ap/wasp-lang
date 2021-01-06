#pragma once
#include "pch.h"
#include "VirtualMachine.h"
#include "Assertion.h"
#include <cstddef>

#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define IS_ERROR(x) std::holds_alternative<ErrorObject>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::get_if;
using std::holds_alternative;
using std::to_integer;
using std::make_shared;

VirtualMachine::VirtualMachine(ObjectStore_ptr constant_pool, CodeObject_ptr main_code_object)
{
	constant_pool = std::move(constant_pool);

	variable_store = std::make_shared<ObjectStore>();
	scope_stack.push(make_shared<LocalScope>());
	call_stack.push(make_shared<CallFrame>(main_code_object, 0));
}

// Perform

OpResult VirtualMachine::perform_nullary_operation(OpCode opcode)
{
	switch (opcode)
	{
	case OpCode::NO_OP:
	{
		return OpResult::OK;
	}
	case OpCode::START:
	{
		return OpResult::OK;
	}
	case OpCode::STOP:
	{
		return OpResult::DONE;
	}
	case OpCode::FUNCTION_START:
	{
		push_empty_scope_to_local_scope_stack();
		return OpResult::OK;
	}
	case OpCode::FUNCTION_STOP:
	{
		pop_from_local_scope_stack();
		return OpResult::DONE;
	}
	case OpCode::POP_FROM_STACK:
	{
		pop_from_value_stack();
		return OpResult::OK;
	}
	case OpCode::PUSH_CONSTANT_TRUE:
	{
		auto true_object = constant_pool->get_true_object();
		push_to_value_stack(true_object);
		return OpResult::OK;
	}
	case OpCode::PUSH_CONSTANT_FALSE:
	{
		auto false_object = constant_pool->get_false_object();
		push_to_value_stack(false_object);
		return OpResult::OK;
	}
	case OpCode::LOCAL_SCOPE_START:
	{
		push_empty_scope_to_local_scope_stack();
		return OpResult::OK;
	}
	case OpCode::LOCAL_SCOPE_STOP:
	{
		pop_from_local_scope_stack();
		return OpResult::OK;
	}
	case OpCode::RETURN_VOID:
	case OpCode::YIELD_VOID:
	{
		pop_from_local_scope_stack();
		return OpResult::FAILURE;
	}
	default:
	{
		return OpResult::FAILURE;
	}
	}
}

OpResult VirtualMachine::perform_unary_operation(OpCode opcode)
{
	Object_ptr TOS = pop_from_value_stack();
	Object_ptr result;

	switch (opcode)
	{
	case OpCode::UNARY_NEGATIVE:
	{
		result = perform_unary_negative(TOS);
		break;
	}
	case OpCode::UNARY_NOT:
	{
		result = perform_unary_not(TOS);
		break;
	}
	case OpCode::ASSERT:
	case OpCode::SWEAR:
	case OpCode::IMPLORE:
	{
		result = is_truthy(TOS) ? constant_pool->get_true_object() : constant_pool->get_false_object();
		break;
	}
	case OpCode::RETURN_VALUE:
	case OpCode::YIELD_VALUE:
	{
		pop_from_local_scope_stack();
		return OpResult::FAILURE;
	}
	case OpCode::MAKE_ITERABLE:
	{
		result = make_iterable(TOS);
		break;
	}
	default:
	{
		return OpResult::FAILURE;
	}
	}

	if (IS_ERROR(*result))
	{
		return OpResult::FAILURE;
	}

	push_to_value_stack(result);
	return OpResult::OK;
}

OpResult VirtualMachine::perform_binary_operation(OpCode opcode)
{
	auto left = pop_from_value_stack();
	auto right = pop_from_value_stack();
	Object_ptr result;

	switch (opcode)
	{
	case OpCode::ADD:
	{
		result = perform_add(left, right);
		break;
	}
	case OpCode::SUBTRACT:
	{
		result = perform_subtract(left, right);
		break;
	}
	case OpCode::MULTIPLY:
	{
		result = perform_multiply(left, right);
		break;
	}
	case OpCode::DIVISION:
	{
		result = perform_division(left, right);
		break;
	}
	case OpCode::REMINDER:
	{
		result = perform_reminder(left, right);
		break;
	}
	case OpCode::POWER:
	{
		result = perform_power(left, right);
		break;
	}
	case OpCode::NOT_EQUAL:
	{
		result = perform_not_equal(left, right);
		break;
	}
	case OpCode::EQUAL:
	{
		result = perform_equal(left, right);
		break;
	}
	case OpCode::LESSER_THAN:
	{
		result = perform_lesser_than(left, right);
		break;
	}
	case OpCode::LESSER_THAN_EQUAL:
	{
		result = perform_lesser_than_equal(left, right);
		break;
	}
	case OpCode::GREATER_THAN:
	{
		result = perform_greater_than(left, right);
		break;
	}
	case OpCode::GREATER_THAN_EQUAL:
	{
		result = perform_greater_than_equal(left, right);
		break;
	}
	case OpCode::AND:
	{
		result = perform_and(left, right);
		break;
	}
	case OpCode::OR:
	{
		result = perform_or(left, right);
		break;
	}
	case OpCode::NULLISH_COALESE:
	{
		break;
		result = perform_nullish_coalese(left, right);
	}
	default:
	{
		return OpResult::FAILURE;
	}
	}

	if (IS_ERROR(*result))
	{
		return OpResult::FAILURE;
	}

	push_to_value_stack(result);
	return OpResult::OK;
}

// Execute

OpResult VirtualMachine::execute(OpCode opcode)
{
	switch (opcode)
	{
	case OpCode::NO_OP:
	case OpCode::START:
	case OpCode::STOP:
	case OpCode::FUNCTION_START:
	case OpCode::FUNCTION_STOP:
	case OpCode::POP_FROM_STACK:
	case OpCode::PUSH_CONSTANT_TRUE:
	case OpCode::PUSH_CONSTANT_FALSE:
	case OpCode::RETURN_VOID:
	case OpCode::YIELD_VOID:
	case OpCode::LOCAL_SCOPE_START:
	case OpCode::LOCAL_SCOPE_STOP:
	{
		return perform_nullary_operation(opcode);
	}

	case OpCode::UNARY_NEGATIVE:
	case OpCode::UNARY_NOT:
	case OpCode::ASSERT:
	case OpCode::SWEAR:
	case OpCode::IMPLORE:
	case OpCode::RETURN_VALUE:
	case OpCode::YIELD_VALUE:
	case OpCode::MAKE_ITERABLE:
	{
		return perform_unary_operation(opcode);
	}

	case OpCode::ADD:
	case OpCode::SUBTRACT:
	case OpCode::MULTIPLY:
	case OpCode::DIVISION:
	case OpCode::REMINDER:
	case OpCode::POWER:
	case OpCode::NOT_EQUAL:
	case OpCode::EQUAL:
	case OpCode::LESSER_THAN:
	case OpCode::LESSER_THAN_EQUAL:
	case OpCode::GREATER_THAN:
	case OpCode::GREATER_THAN_EQUAL:
	case OpCode::AND:
	case OpCode::OR:
	case OpCode::NULLISH_COALESE:
	{
		return perform_binary_operation(opcode);
	}
	default:
	{
		return OpResult::FAILURE;
	}
	}
}

OpResult VirtualMachine::execute(OpCode opcode, int operand)
{
	switch (opcode)
	{
	case OpCode::PUSH_CONSTANT:
	{
		auto obj = constant_pool->get(operand);
		push_to_value_stack(obj);
		return OpResult::OK;
	}
	case OpCode::LOAD_LOCAL:
	{
		auto obj = variable_store->get(operand);
		push_to_value_stack(obj);
		return OpResult::OK;
	}
	case OpCode::STORE_LOCAL:
	{
		auto obj = pop_from_value_stack();
		variable_store->set(operand, obj);
		return OpResult::OK;
	}
	case OpCode::CREATE_LOCAL:
	{
		scope_stack.top()->add(operand);
		auto obj = pop_from_value_stack();
		variable_store->set(operand, obj);
		return OpResult::OK;
	}
	case OpCode::MAKE_LIST:
	{
		auto elements = pop_n_from_value_stack(operand);
		std::reverse(elements.begin(), elements.end());

		auto list_object = MAKE_OBJECT_VARIANT(ListObject(elements));
		push_to_value_stack(list_object);
		return OpResult::OK;
	}
	case OpCode::MAKE_TUPLE:
	{
		auto elements = pop_n_from_value_stack(operand);
		std::reverse(elements.begin(), elements.end());

		auto tuple_object = MAKE_OBJECT_VARIANT(TupleObject(elements));
		push_to_value_stack(tuple_object);
		return OpResult::OK;
	}
	case OpCode::MAKE_MAP:
	{
		auto map_object = MapObject();
		int count = 0;

		while (count < operand)
		{
			auto key_value = pop_n_from_value_stack(2);
			map_object.insert(key_value[1], key_value[0]); // {key, value}
			count++;
		}

		push_to_value_stack(MAKE_OBJECT_VARIANT(map_object));
		return OpResult::OK;
	}
	case OpCode::JUMP:
	{
		set_ip(operand);
		return OpResult::OK;
	}
	case OpCode::POP_JUMP:
	{
		pop_from_value_stack();
		set_ip(operand);
		return OpResult::OK;
	}
	case OpCode::JUMP_IF_FALSE:
	{
		auto TOS = top_of_value_stack();

		if (!is_truthy(TOS))
		{
			set_ip(operand);
		}

		return OpResult::OK;
	}
	case OpCode::POP_JUMP_IF_FALSE:
	{
		auto TOS = top_of_value_stack();

		if (!is_truthy(TOS))
		{
			pop_from_value_stack();
			set_ip(operand);
		}

		return OpResult::OK;
	}
	case OpCode::GET_NEXT_OR_JUMP:
	{
		auto TOS = top_of_value_stack();

		if (!holds_alternative<IteratorObject>(*TOS))
		{
			return OpResult::FAILURE;
		}

		auto iterator_obj = get_if<IteratorObject>(&*TOS);
		std::optional<Object_ptr> result = iterator_obj->get_next();

		if (result.has_value())
		{
			push_to_value_stack(result.value());
			return OpResult::OK;
		}

		set_ip(operand);
		return OpResult::OK;
	}
	case OpCode::LABEL:
	{
		// Do nothing
		return OpResult::OK;
	}
	default:
	{
		return OpResult::FAILURE;
	}
	}
}

OpResult VirtualMachine::execute(OpCode opcode, int operand_1, int operand_2)
{
	switch (opcode)
	{
	case OpCode::CALL_FUNCTION:
	case OpCode::CALL_GENERATOR:
	{
		// TODO
		return OpResult::FAILURE;
	}
	default:
	{
		return OpResult::FAILURE;
	}
	}
}

// Run

OpResult VirtualMachine::run()
{
	OpResult result = OpResult::FAILURE;

	while (true)
	{
		int ip = get_ip();
		const OpCode opcode = (OpCode)get_current_code_object()->instructions.at(ip);
		const int arity = get_opcode_arity(opcode);

		switch (arity)
		{
		case 0:
		{
			ip += 1;

			result = execute(opcode);
			break;
		}
		case 1:
		{
			ByteVector operands = get_current_code_object()->operands_of_opcode_at(ip);
			ASSERT(operands.size() == 1, "Expected 1 operands");
			ip += 2;

			result = execute(opcode, to_integer<int>(operands[0]));
			break;
		}
		case 2:
		{
			ByteVector operands = get_current_code_object()->operands_of_opcode_at(ip);
			ASSERT(operands.size() == 2, "Expected 2 operands");
			ip += 3;
			
			result = execute(opcode, to_integer<int>(operands[0]), to_integer<int>(operands[1]));
			break;
		}
		default:
		{
			FATAL("No Wasp bytecode has more than 2 operands");
			return OpResult::FAILURE;
		}
		}

		switch (result)
		{
		case OpResult::FAILURE:
		case OpResult::DONE:
		{
			return result;
		}
		}
	}
}

