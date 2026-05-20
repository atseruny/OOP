#pragma once

#include "Compiler.hpp"
#include "CPU.hpp"
#include "Loader.hpp"

class Executor
{
public:
	Executor();
	~Executor();
	void loadAndRun(const std::string &exePath);
	std::ifstream validate(const std::string &exePath);

private:
	Memory *memory;
	std::vector<int32_t> constPool;
	CPU *cpu;
};
