#include "../includes/Debugger.hpp"
#include "../includes/CPU.hpp"
#include <sstream>
#include <unordered_set>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────────────
//  helpers
// ─────────────────────────────────────────────────────────────────────────────

static std::string trim(const std::string &s)
{
	size_t a = s.find_first_not_of(" \t\r\n");
	size_t b = s.find_last_not_of(" \t\r\n");
	return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
}

static std::pair<std::string, std::string> splitCmd(const std::string &line)
{
	std::istringstream ss(line);
	std::string cmd, rest;
	ss >> cmd;
	std::getline(ss, rest);
	return { cmd, trim(rest) };
}

// ─────────────────────────────────────────────────────────────────────────────
//  ctor / dtor
// ─────────────────────────────────────────────────────────────────────────────

Debugger::Debugger() {}

Debugger::~Debugger()
{
	delete memory;
	delete cpu;
}

// ─────────────────────────────────────────────────────────────────────────────
//  validate – mirrors Executor::validate
// ─────────────────────────────────────────────────────────────────────────────

std::ifstream Debugger::validate(const std::string &exePath)
{
	std::ifstream f(exePath);
	if (!f.is_open())
		throw std::runtime_error("Cannot open '" + exePath + "'");

	std::string line;
	std::getline(f, line);
	if (line != "~AnushFile")
		throw std::runtime_error("Invalid file '" + exePath + "'");

	std::cout << "[Debugger] Setting up debug environment\n";
	memory = new Memory();
	cpu    = new CPU();
	return f;
}


void Debugger::loadAndDebug(std::ifstream &exe)
{
	std::cout << "\n[Debugger] Loading program...\n";

	std::string content((std::istreambuf_iterator<char>(exe)),
	                     std::istreambuf_iterator<char>());

	{
		std::istringstream ss(content);
		std::string ln;
		bool inCode = false;
		while (std::getline(ss, ln))
		{
			if (ln == ".CODE") { inCode = true; continue; }
			if (inCode && !trim(ln).empty())
				srcLines.push_back(ln);
		}
	}

	{
		std::istringstream ss2(content);
		nInstr = Loader::loadFromStream(ss2, memory, constPool);
	}

	std::cout << "[Debugger] Loaded " << nInstr << " instruction(s)\n";
	cpu->setConstPool(constPool);

	std::cout << "\n[Debugger] Ready. Type 'help' for commands.\n\n";
	showCurrentLine();
	repl();
}


void Debugger::repl()
{
	running = true;
	std::string line;
	while (running)
	{
		std::cout << "(dbg) ";
		if (!std::getline(std::cin, line))
		{
			std::cout << "\n";
			break;
		}
		line = trim(line);
		if (line.empty()) continue;
		if (!dispatch(line)) break;
	}
}

bool Debugger::dispatch(const std::string &line)
{
	auto [cmd, arg] = splitCmd(line);

	if (cmd == "step"   || cmd == "s")  { cmdStep();              return running; }
	if (cmd == "next"   || cmd == "n")  { cmdNext();              return running; }
	if (cmd == "continue"||cmd == "c")  { cmdContinue();          return running; }
	if (cmd == "info"   || cmd == "i")  { cmdInfo(arg);           return true; }
	if (cmd == "print"  || cmd == "p")  { cmdPrint(arg);          return true; }
	if (cmd == "list"   || cmd == "l")  { cmdList(arg);           return true; }
	if (cmd == "regs"   || cmd == "r")  { cmdRegs();              return true; }
	if (cmd == "mem"    || cmd == "m")  { cmdMem(arg);            return true; }
	if (cmd == "help"   || cmd == "h")  { cmdHelp();              return true; }
	if (cmd == "quit"   || cmd == "q")  { cmdQuit();              return false; }

	std::cout << "Unknown command '" << cmd << "'. Type 'help'.\n";
	return true;
}

uint16_t Debugger::currentIP() const
{
	return cpu->getIP();
}

bool Debugger::stepOne()
{
	bool ok = cpu->stepOne(memory);
	return ok;
}

void Debugger::showCurrentLine() const
{
	uint16_t ip = cpu->getIP();
	std::cout << "=> [" << std::dec << ip << "] ";
	if (ip < srcLines.size())
		std::cout << srcLines[ip];
	else
		std::cout << "<end of program>";
	std::cout << "\n";
}

std::string Debugger::disasm(uint16_t idx) const
{
	if (idx < srcLines.size())
		return srcLines[idx];
	return "<out of range>";
}


void Debugger::cmdStep()
{
	bool alive = stepOne();
	if (!alive)
	{
		std::cout << "[Debugger] Program exited.\n";
		cpu->dumpRegisters();
		running = false;
		return;
	}
	showCurrentLine();
}

void Debugger::cmdNext()
{
	cmdStep();
}

void Debugger::cmdContinue()
{
	while (true)
	{
		bool alive = stepOne();
		if (!alive)
		{
			std::cout << "[Debugger] Program exited.\n";
			cpu->dumpRegisters();
			running = false;
			return;
		}
	}
}

void Debugger::cmdInfo(const std::string &arg)
{
	if (arg == "regs" || arg == "registers" || arg == "r")
	{
		cmdRegs();
		return;
	}
	std::cout << "IP      = " << std::dec << currentIP() << "\n";
	std::cout << "SP      = 0x" << std::hex << cpu->getSP() << std::dec << "\n";
	std::cout << "cmpFlag = " << cpu->getCmpFlag() << "\n";
}

void Debugger::cmdPrint(const std::string &arg)
{
	if (arg.empty())
	{
		std::cout << "Usage: print <rN | #N>\n"
		             "       print r4       → value of register 4\n"
		             "       print #12      → value of const-pool[12]\n"
		             "       print mem N    → raw memory cell N\n";
		return;
	}

	// print mem <addr>
	if (arg.substr(0, 3) == "mem")
	{
		cmdMem(trim(arg.substr(3)));
		return;
	}

	// print rN
	if (arg[0] == 'r' || arg[0] == 'R')
	{
		try
		{
			uint8_t r = static_cast<uint8_t>(std::stoul(arg.substr(1)));
			if (r >= NUM_REGS)
			{
				std::cout << "Register index out of range (0–" << NUM_REGS - 1 << ")\n";
				return;
			}
			std::cout << "r" << (int)r << " = " << cpu->getReg(r) << "\n";
		}
		catch (...) { std::cout << "Invalid register '" << arg << "'\n"; }
		return;
	}

	// print #N (const pool)
	if (arg[0] == '#')
	{
		try
		{
			size_t idx = std::stoul(arg.substr(1));
			if (idx >= constPool.size())
			{
				std::cout << "Const-pool index out of range (size=" << constPool.size() << ")\n";
				return;
			}
			std::cout << "constPool[" << idx << "] = " << constPool[idx] << "\n";
		}
		catch (...) { std::cout << "Invalid const-pool index '" << arg << "'\n"; }
		return;
	}

	std::cout << "Unknown print target '" << arg << "'. Use rN, #N, or mem N.\n";
}

void Debugger::cmdList(const std::string &arg)
{
	uint16_t centre = currentIP();
	int      window = 5;

	if (!arg.empty())
	{
		try { centre = static_cast<uint16_t>(std::stoul(arg)); }
		catch (...) { std::cout << "Usage: list [instruction-index]\n"; return; }
	}

	int16_t from = static_cast<int16_t>(centre) - window;
	if (from < 0) from = 0;
	uint16_t to = static_cast<uint16_t>(centre) + window;
	if (to >= static_cast<uint16_t>(srcLines.size()))
		to = static_cast<uint16_t>(srcLines.size()) - 1;

	for (uint16_t i = static_cast<uint16_t>(from); i <= to; ++i)
	{
		bool isCur = (i == currentIP());

		std::cout << (isCur ? ">" : " ")
		          << " [" << std::setw(3) << std::dec << i << "]  "
		          << srcLines[i] << "\n";
	}
}

void Debugger::cmdRegs()
{
	cpu->dumpRegisters();
}

void Debugger::cmdMem(const std::string &arg)
{
	if (arg.empty())
	{
		std::cout << "Usage: mem <address>\n";
		return;
	}
	try
	{
		uint32_t addr = std::stoul(arg, nullptr, 0);   // supports 0x prefix
		if (addr >= MEM_SIZE)
		{
			std::cout << "Address 0x" << std::hex << addr << " out of range.\n";
			return;
		}
		int32_t val = memory->read(static_cast<uint16_t>(addr));
		std::cout << "mem[0x" << std::hex << std::setw(4) << std::setfill('0') << addr
		          << "] = " << std::dec << val
		          << "  (0x" << std::hex << val << ")\n" << std::dec;
	}
	catch (...) { std::cout << "Invalid address '" << arg << "'\n"; }
}

void Debugger::cmdHelp()
{
	std::cout <<
	"  step   | s              Execute one instruction\n"
	"  next   | n              Same as step\n"
	"  continue | c            Run until EXIT\n"
	"  list   | l [N]          Disassemble ±5 instructions around N (default: current)\n"
	"  regs   | r              Dump all registers\n"
	"  print  | p <rN|#N>      Print register rN or const-pool entry #N\n"
	"  print    p mem <addr>   Print raw memory cell at address\n"
	"  mem    | m <addr>       Same as print mem\n"
	"  info   | i [break|regs] Show IP/SP/cmpFlag or registers\n"
	"  help   | h              Show this help\n"
	"  quit   | q              Exit the debugger\n";
}

void Debugger::cmdQuit()
{
	std::cout << "[Debugger] Quit.\n";
	running = false;
}
