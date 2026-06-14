#pragma once

#include "Compiler.hpp"
#include "CPU.hpp"
#include "Loader.hpp"

struct DebugInstruction
{
	Instruction inst;
	std::string srcText; // assembly
};

class Debugger
{
public:
	Debugger();
	~Debugger();

	std::ifstream validate(const std::string &exePath);
	void loadAndDebug(std::ifstream &exe);

private:
	Memory *memory = nullptr;
	CPU *cpu = nullptr;
	std::vector<int32_t> constPool;
	uint16_t nInstr = 0;

	std::vector<std::string> srcLines;

	bool running = false;

	void repl();
	bool dispatch(const std::string &line);

	void cmdStep();
	void cmdNext();
	void cmdContinue();
	void cmdInfo(const std::string &arg);
	void cmdPrint(const std::string &arg);
	void cmdList(const std::string &arg);
	void cmdRegs();
	void cmdMem(const std::string &arg);
	void cmdHelp();
	void cmdQuit();

	bool stepOne(); // execute one instruction; returns false on EXIT
	void showCurrentLine() const;

	uint16_t currentIP() const;
};
