#pragma once

#include "Compiler.hpp"
#include "SymbolTable.hpp"

struct Instruction;

enum class Operand
{
	MEM,
	REG,
	CONST,
};

class VM
{
private:
	std::vector<int> regs;
	int next;
	std::vector<Instruction> program;
	std::vector<int> constants;
	int cmpFlag;

	int compileNum(Node* node);
	int compileVar(Node* node);
	int compileOp(Node* node);
	int compileAssign(Node* node);
	int compileBlock(Node* node);
	int compileIf(Node* node);
	int compileComp(Node* node);
	int compileWhile(Node* node);

	void parseLine(const std::string& line);
	int regIndex(const std::string& r);
	Operand defType(const std::string& var) const;

public:
	VM();
	~VM();
	void visualize() const;
	int compile(Node* node);
	void writeInExe();
	void loadFile(const std::string& name);
	int execute(SymbolTable& ST);

};
