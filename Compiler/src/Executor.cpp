#include "../includes/Executor.hpp"
#include "../includes/CPU.hpp"

Executor::Executor() {}

std::ifstream Executor::validate(const std::string &exePath)
{
	std::ifstream f(exePath);
	if (!f.is_open())
		throw std::runtime_error("Cannot open '" + exePath + "'");

	std::string line;
	std::getline(f, line);
	if (line != "~AnushFile")
		throw std::runtime_error("Invalid file '" + exePath + "'");
	std::cout << "--Settng up Exection Environment";
	memory = new Memory();
	cpu = new CPU();
	return f;
}

Executor::~Executor()
{
	delete cpu;
	delete memory;
}

// void Executor::loadAndRun(const std::string &exePath)
// {
// 	std::cout << "[VM] Loading '" << exePath << "' ...\n";
// 	uint16_t nInstructions = Loader::load(exePath, memory, constPool);
// 	std::cout << "[VM] Loaded " << nInstructions << " instruction(s) into code section.\n";
// 	std::cout << "[VM] Running...\n\n";

// 	cpu.run();

// 	std::cout << "\n[VM] Execution finished.\n";

// 	cpu.dumpRegisters();
// 	memory.dump(DATA_BASE, DATA_BASE + 16);
// }