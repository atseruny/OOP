#pragma once

#include "Compiler.hpp"
#include <vector>
#include "Node.hpp"
#include <cstdint>
#include "SymbolTable.hpp"
#include <iomanip>

class VM
{
private:
	std::vector<int> regs;
	int next;
	std::vector<Instruction> program;
	std::vector<int> constants;
public:
	VM();
	~VM();
	void visualize() const;
	int compile(const Node* node);
	int execute(SymbolTable& ST);

};
