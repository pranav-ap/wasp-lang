#pragma once
#include "Position.h"

void Position::increment_line_number()
{
	this->line_num++;
}

void Position::increment_column_number()
{
	this->column_num++;
}

void Position::reset_column_number()
{
	this->column_num = 1;
}