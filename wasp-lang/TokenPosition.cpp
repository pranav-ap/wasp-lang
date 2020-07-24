#pragma once
#include "TokenPosition.h"

void TokenPosition::increment_line_number()
{
	this->line_num++;
}

void TokenPosition::increment_column_number()
{
	this->column_num++;
}

void TokenPosition::decrement_column_number()
{
	this->column_num--;
}

void TokenPosition::reset_column_number()
{
	this->column_num = 1;
}

int TokenPosition::get_line_num() const
{
	return this->line_num;
}

int TokenPosition::get_column_num() const
{
	return this->column_num;
}