#include "../includes/Compiler.hpp"
#include "../includes/Debugger.hpp"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <exe-file>\n";
		return 1;
	}

	std::string exePath = argv[1];

	Debugger dbg;
	try
	{
		std::ifstream file = dbg.validate(exePath);
		dbg.loadAndDebug(file);
	}
	catch (const std::exception &e)
	{
		std::cerr << "[DEBUGGER ERROR] " << e.what() << "\n";
		return 1;
	}

	return 0;
}
