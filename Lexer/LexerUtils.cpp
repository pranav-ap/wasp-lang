#pragma once
#include "pch.h"
#include "Lexer.h"

bool Lexer::expect_current_char(char ch)
{
	if (ch == this->get_current_char())
	{
		this->pointer.advance();
		return true;
	}

	return false;
}

char Lexer::get_char_at(int index) const
{
	if ((size_t)index >= this->raw_source.size())
		return NULL;

	return this->raw_source[index];
}

char Lexer::get_current_char() const
{
	int index = this->pointer.get_index();
	return this->get_char_at(index);
}

char Lexer::get_right_char() const
{
	int index = this->pointer.get_index();
	return this->get_char_at(index + 1);
}