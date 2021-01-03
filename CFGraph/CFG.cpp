#pragma once
#include "pch.h"
#include "CFG.h"
#include "InstructionPrinter.h"
#include <memory>
#include <iostream>
#include <string>
#include <iomanip>

using std::wcout;
using std::cout;
using std::endl;
using std::setw;
using std::byte;
using std::wstring;
using std::to_wstring;
using std::to_string;
using std::make_shared;

void CFG::print(const std::map<int, std::wstring> name_map)
{
	InstructionPrinter_ptr printer = make_shared<InstructionPrinter>(object_store, name_map);

	for (auto const& [id, block] : basic_blocks)
	{
		wcout << L"\n == BLOCK " << id << endl;

		printer->print(block->code_object);

		if (block->type == BlockType::JumpOnFalse)
		{
			int true_successor_id = adjacency_list[id].first;
			int false_successor_id = adjacency_list[id].second;

			wcout << L"\n > If true, go to : Block " << true_successor_id;
			wcout << L"\n > If false, go to : Block " << false_successor_id << endl;
		}
		else if (block->type == BlockType::JustJump || block->type == BlockType::ConnectToFollowingBlock)
		{
			int unique_successor_id = adjacency_list[id].first;
			wcout << L"\n > Go to :  " << unique_successor_id << endl;
		}
	}
}
