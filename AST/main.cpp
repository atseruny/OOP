#include "Token.hpp"

// int main(int argc, char **argv)
// {
// 	int i = 2;
// 	if (argc == 1)
// 	{
// 		std::cout << "Use: ./expr \"arithmetic expression\" \"variables(optional)\"\n";
// 		return (1);
// 	}
// 	Node *tree = parse_expr(argv[1]);
// 	if (!tree)
// 		return (1);
// 	std::map<std::string, int> vars;
// 	if (argc > 2)
// 	{
// 		while (i < argc)
// 			parse_vars(argv[i++], vars);
// 	}
// 	std::cout << "Value: " << eval_tree(tree, vars) << std::endl;
// 	// for(auto& v : vars)
// 	// 	std::cout << v.first << ": " << v.second << "\n";
// 	destroy_tree(tree);
// 	return 0;
// }

void printNode(const Token& node)
{
	switch (node.type)
	{
		case NodeType::Num:
			std::cout << "Type: Number, Value: " << node.value << "\n";
			break;

		case NodeType::Var:
			std::cout << "Type: Variable, Name: " << node.value << "\n";
			break;

		case NodeType::Op:
			std::cout << "Type: Operator: " << node.value << "\n";
			break;

		case NodeType::OpBr:
			std::cout << "Type: Open Bracket, Symbol: " << node.value << "\n";
			break;

		case NodeType::ClBr:
			std::cout << "Type: Close Bracket, Symbol: " << node.value << "\n";
			break;

		default:
			std::cout << "Type: Unknown\n";
			break;
	}
}

int main()
{
	try
	{
		std::string expression;
		std::cout << "Enter expression: ";
		std::getline(std::cin, expression);

		std::stringstream line(expression);

		std::vector<std::string> v = lexer(line);
		std::vector<Token> tokens = tokenizer(v);
		for(auto& c:v)
			std::cout << c << "  ";
		std::cout <<'\n';
		for(auto& c:tokens)
			printNode(c);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
}
