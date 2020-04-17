#pragma once
#include "WaspCLI.h"
#include "spdlog.h"

int main(int argc, char** argv)
{
	WaspCLI cli;
	spdlog::set_pattern("[%^---%L---%$] %v");
	CLI11_PARSE(cli.get_app(), argc, argv);
}