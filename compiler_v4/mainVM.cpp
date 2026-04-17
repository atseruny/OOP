#include "Compiler.hpp"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Error: Expecting a file for execution\n";
		return 1;
	}
	try
	{
		VM vm;
		vm.loadFile(argv[1]);
		// vm.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}