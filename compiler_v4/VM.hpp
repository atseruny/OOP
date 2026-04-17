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

	int compileNum(Node* node);
	int compileVar(Node* node);
	int compileOp(Node* node);
	int compileAssign(Node* node);
	int compileBlock(Node* node);
	int compileIf(Node* node);
	int compileComp(Node* node);
	int compileWhile(Node* node);

public:
	VM();
	~VM();
	void visualize() const;
	int compile(Node* node);
	int execute(SymbolTable& ST);

};
