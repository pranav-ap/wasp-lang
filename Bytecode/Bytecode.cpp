#include "pch.h"
#include "Bytecode.h"
#include <iostream>

#define GET_OPCODE std::to_integer<int>(instructions.at(index))
#define INC_INDEX index++

void Bytecode::print()
{
	int length = instructions.size();

	for (int index = 0; index < length; index++)
	{
		int opcode = GET_OPCODE;

		switch ((OpCode)opcode)
		{
		case OpCode::PUSH:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "PUSH " << operand << std::endl;
			break;
		}
		case OpCode::POP:
		{
			std::cout << "POP" << std::endl;
			break;
		}
		case OpCode::UNARY_ADD:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "UNARY_ADD " << operand << std::endl;
			break;
		}
		case OpCode::UNARY_SUBTRACT:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "UNARY_SUBTRACT " << operand << std::endl;
			break;
		}
		case OpCode::UNARY_BANG:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "UNARY_BANG " << operand << std::endl;
			break;
		}

		case OpCode::ADD:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "ADD " << operand_1 << operand_2 << std::endl;
			break;
		}
		case OpCode::SUBTRACT:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "SUBTRACT " << operand_1 << operand_2 << std::endl;
			break;
		}
		case OpCode::MULTIPLY:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "MULTIPLY " << operand_1 << operand_2 << std::endl;
			break;
		}
		case OpCode::DIVISION:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "DIVISION " << operand_1 << operand_2 << std::endl;
			break;
		}
		case OpCode::REMINDER:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "REMINDER " << operand_1 << operand_2 << std::endl;
			break;
		}
		case OpCode::POWER:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "POWER " << operand_1 << operand_2 << std::endl;
			break;
		}
		case OpCode::NOT_EQUAL:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "NOT_EQUAL " << operand_1 << operand_2 << std::endl;
			break;
		}
		case OpCode::EQUAL:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "EQUAL " << operand_1 << operand_2 << std::endl;
			break;
		}

		case OpCode::LESSER_THAN:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "LESSER_THAN " << operand_1 << operand_2 << std::endl;
			break;
		}

		case OpCode::LESSER_THAN_EQUAL:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "LESSER_THAN_EQUAL " << operand_1 << operand_2 << std::endl;
			break;
		}

		case OpCode::GREATER_THAN:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "GREATER_THAN " << operand_1 << operand_2 << std::endl;
			break;
		}

		case OpCode::GREATER_THAN_EQUAL:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "GREATER_THAN_EQUAL " << operand_1 << operand_2 << std::endl;
			break;
		}
		case OpCode::AND:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "AND " << operand_1 << operand_2 << std::endl;
			break;
		}
		case OpCode::OR:
		{
			INC_INDEX;
			int operand_1 = GET_OPCODE;
			INC_INDEX;
			int operand_2 = GET_OPCODE;

			std::cout << "OR " << operand_1 << operand_2 << std::endl;
			break;
		}
		case OpCode::ASSERT:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "ASSERT " << operand << std::endl;
			break;
		}
		case OpCode::JUMP:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "JUMP " << operand << std::endl;
			break;
		}
		case OpCode::JUMP_IF_TRUE:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "JUMP_IF_TRUE " << operand << std::endl;
			break;
		}
		case OpCode::JUMP_IF_FALSE:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "JUMP_IF_FALSE " << operand << std::endl;
			break;
		}
		case OpCode::PASS:
		{
			std::cout << "PASS" << std::endl;
			break;
		}
		case OpCode::CALL_FUNCTION:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "CALL_FUNCTION " << operand << std::endl;
			break;
		}

		case OpCode::CALL_GENERATOR:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "CALL_GENERATOR " << operand << std::endl;
			break;
		}

		case OpCode::RETURN_VOID:
		{
			std::cout << "RETURN_VOID" << std::endl;
			break;
		}

		case OpCode::RETURN_VALUE:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "RETURN_VALUE " << operand << std::endl;
			break;
		}

		case OpCode::YIELD_VOID:
		{
			INC_INDEX;
			std::cout << "YIELD_VOID " << std::endl;
			break;
		}
		case OpCode::YIELD_VALUE:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "YIELD_VALUE " << operand << std::endl;
			break;
		}

		case OpCode::GET_VARIABLE:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "GET_VARIABLE " << operand << std::endl;
			break;
		}
		case OpCode::SET_VARIABLE:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "SET_VARIABLE " << operand << std::endl;
			break;
		}
		case OpCode::CONSTANT:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "CONSTANT " << operand << std::endl;
			break;
		}
		case OpCode::CONSTANT_TRUE:
		{
			std::cout << "CONSTANT_TRUE" << std::endl;
			break;
		}

		case OpCode::CONSTANT_FALSE:
		{
			std::cout << "CONSTANT_FALSE" << std::endl;
			break;
		}

		case OpCode::LIST:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "LIST " << operand << std::endl;
			break;
		}

		case OpCode::TUPLE:
		{
			int operand = GET_OPCODE;
			std::cout << "TUPLE " << operand << std::endl;
			break;
		}

		case OpCode::MAP:
		{
			INC_INDEX;
			int operand = GET_OPCODE;
			std::cout << "MAP " << operand << std::endl;
			break;
		}
		default:
		{
			break;
		}
		};
	}
}