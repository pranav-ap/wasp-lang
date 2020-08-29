#pragma once
#include "pch.h"
#include "CFG.h"
#include <memory>
#include <iostream>
#include <string>
#include <iomanip>

#define GET_OPCODE (OpCode) std::to_integer<int>(block->instructions.at(index))
#define INC_INDEX index++

using std::wcout;
using std::cout;
using std::endl;
using std::setw;
using std::byte;
using std::wstring;
using std::to_wstring;
using std::to_string;
using std::make_shared;

void CFG::print()
{
	InstructionPrinter_ptr printer = make_shared<InstructionPrinter>(constant_pool);

	for (auto const& [id, block] : node_id_to_basic_blocks)
	{
		wcout << L"\n == BLOCK " << id << endl;

		printer->print(block->instructions);

		if (block->type == BlockType::ConditionalJump)
		{
			int to_label = to_integer<int>(block->instructions.back());

			int true_successor_id = adjacency_list[id].first;
			int false_successor_id = adjacency_list[id].second;

			wcout << L"\n > If true, go to : " << true_successor_id;
			wcout << L"\n > If false, go to : " << false_successor_id << endl;
		}
		else if (block->type == BlockType::Unconditional)
		{
			int unique_successor_id = adjacency_list[id].first;
			wcout << L"\n > Go to :  " << unique_successor_id << endl;;
		}
		else if (block->type == BlockType::UnconditionalJump)
		{
			int to_label = to_integer<int>(block->instructions.back());

			int unique_successor_id = adjacency_list[id].first;
			wcout << L"\n > Go to :  " << unique_successor_id << endl;;
		}
	}
}