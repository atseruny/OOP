#pragma once

#include "Compiler.hpp"
#include "CPU.hpp"
#include "Loader.hpp"


class Executor
{
public:
	Executor();
	~Executor() = default;
	void loadAndRun(const std::string& exePath, bool debug);

private:
	Memory memory;
	std::vector<int32_t> constPool;
	CPU cpu;

};
