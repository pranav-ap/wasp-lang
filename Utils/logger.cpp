#pragma once
#include "pch.h"
#include "rang.hpp"
#include "logger.h"

#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <string>

void Logger::log(rang::bgB bg, std::string type, std::string text)
{
	std::cout << " " << bg << rang::fg::black << std::setw(10) << std::left << type
		<< rang::bg::reset << rang::fg::reset << " " << text << std::endl;
}

void Logger::info(std::string text)
{
	log(rang::bgB::cyan, "   INFO", text);
}

void Logger::fatal(std::string text)
{
	log(rang::bgB::magenta, "   FATAL", text);
	exit(1);
}

void Logger::trace(std::string text)
{
	log(rang::bgB::gray, "   TRACE", text);
}

void Logger::error(std::string text)
{
	log(rang::bgB::red, "   ERROR", text);
}