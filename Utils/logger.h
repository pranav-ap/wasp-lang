#pragma once
#include "rang.hpp"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <sstream>

#define TITLE(text) \
    std::cout << rang::style::bold << rang::fg::yellow << " " << text << "\n" \
    << rang::style::reset << rang::fg::reset << std::endl

#define INFO(text) \
    std::cout << " " << rang::bgB::cyan << rang::fg::black  << std::setw(9) << std::left \
    << "  INFO" << rang::bg::reset << rang::fg::reset << " " << text << std::endl

#define TRACE(text) \
    std::cout << " " << rang::bgB::gray << rang::fg::black << std::setw(9) << std::left \
    << "  TRACE" << rang::bg::reset << rang::fg::reset << " " << text << std::endl

#define ERROR(text) \
    std::cout << " " << rang::bgB::red << rang::fg::black << std::setw(9) << std::left \
    << "  ERROR" << rang::bg::reset << rang::fg::reset << " " << text << std::endl

#define FATAL(text) \
    std::cout << " " << rang::bgB::magenta << rang::fg::black << std::setw(9) << std::left \
    << "  FATAL" << rang::bg::reset << rang::fg::reset << " " << text << std::endl;        \
    exit(1)

#define FATAL_IF_NULLPTR(x, text) if (!x) { FATAL(text); }
#define FATAL_IF_TRUE(x, text) if (x) { FATAL(text); }
#define FATAL_IF_FALSE(x, text) if (!x) { FATAL(text); }
