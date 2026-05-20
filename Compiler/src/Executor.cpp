#include "../includes/Executor.hpp"
#include "../includes/CPU.hpp"

Executor::Executor() : cpu(memory, constPool) {}

void Executor::loadAndRun(const std::string& exePath, bool debug = false) {
	std::cout << "[VM] Loading '" << exePath << "' ...\n";
	uint16_t nInstructions = Loader::load(exePath, memory, constPool);
	std::cout << "[VM] Loaded " << nInstructions << " instruction(s) into code section.\n";
	std::cout << "[VM] Running...\n\n";

	cpu.run();

	std::cout << "\n[VM] Execution finished.\n";

	if (debug) {
		cpu.dumpRegisters();
		memory.dump(DATA_BASE, DATA_BASE + 16);
	}
}