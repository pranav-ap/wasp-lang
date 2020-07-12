#pragma once

#include "spdlog.h"
#include "WaspCLI.h"

int main(int argc, char** argv)
{
	spdlog::set_pattern("%^[%=8l]%$ %@ %v");

	WaspCLI cli;
	CLI11_PARSE(cli.get_app(), argc, argv);
}