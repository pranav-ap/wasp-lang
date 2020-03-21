#pragma once
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

bool Lexer::followed_by(char ch)
{
	int index = this->pointer.get_index();

	while (char c = this->get_char_at(index))
	{
		if (c != ' ')
			return c == ch;

		index++;
	}
}

char Lexer::get_char_at(int index)
{
	if ((size_t)index >= this->raw_source.size())
		return NULL;

	return this->raw_source[index];
}

char Lexer::get_current_char()
{
	int index = this->pointer.get_index();
	return this->get_char_at(index);
}

char Lexer::get_right_char()
{
	int index = this->pointer.get_index();
	return this->get_char_at(index + 1);
}