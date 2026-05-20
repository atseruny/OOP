#include "../includes/Compiler.hpp"
#include "../includes/VM.hpp"



int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Error: Expecting a file with code\n";
		return 1;
	}
	std::ifstream file(argv[1]);
	if (!file)
	{
		std::cerr << "Error: Cannot open file\n";
		return 1;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::string expression = buffer.str();

	SymbolTable ST;

	try
	{
		std::stringstream line(expression);

		int32_t pos = 0;
		std::vector<std::string> v = lexer(line);
		std::vector<Token> tokens = tokenizer(v);
		std::vector<std::unique_ptr<Node>> trees = parser(tokens, ST, pos);


		VM vm;
		for(auto& c:trees){
			// printAST(c.get());
			// std::cout <<"\nNEXT FUNC\n";
			vm.compile(c.get());
			vm.writeInExe();
		}
		// vm.visualize();

	}
	catch(std::exception& e)
	{
		std::cout << "[COMPILE ERROR] "<< e.what() << '\n';
	}
}
