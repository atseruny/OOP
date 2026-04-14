#pragma once

#include "Expr.hpp"
#include "Compiler.hpp"
#include "SymbolTable.hpp"

class VM
{
private:
	std::vector<int> regs;
	int next;
	std::vector<Instruction> program;
	std::vector<int> constants;

	int compileNum(const Node* node);
	int compileVar(const Node* node);
	int compileOp(const Node* node);
	int compileAssign(const Node* node);
	int compileBlock(const Node* node);

public:
	VM();
	~VM();
	void visualize() const;
	int compile(const Node* node);
	int execute(SymbolTable& ST);

};
