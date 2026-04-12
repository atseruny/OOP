#include "Expr.hpp"

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
		case NodeType::EofEx:
			std::cout << "Type: End of Expression\n";
			break;
		case NodeType::If:
			std::cout << "Type: If\n";
			break;
		case NodeType::While:
			std::cout << "Type: While\n";
			break;
		case NodeType::Comp:
			std::cout << "Type: Comparison, Symbol: " << node.value << "\n";
			break;
		case NodeType::Assign:
			std::cout << "Type: Assignment, Symbol: " << node.value << "\n";
			break;
		case NodeType::Not:
			std::cout << "Type: Not, Symbol: " << node.value << "\n";
			break;
		case NodeType::Semi:
			std::cout << "Type: Semicolon, Symbol: " << node.value << "\n";
			break;
		case NodeType::OpBody:
			std::cout << "Type: Open Curly Bracket, Symbol: " << node.value << "\n";
			break;
		case NodeType::ClBody:
			std::cout << "Type: Close Curly Bracket, Symbol: " << node.value << "\n";
			break;
		case NodeType::Decl:
			std::cout << "Type: Declaration, Symbol: " << node.value << "\n";
			break;
		default:
			std::cout << "Type: Unknown\n";
			break;
	}
}

// void clear(Node* node)
// {
// 	if (!node)
// 		return;

// 	clear(node->left);
// 	clear(node->right);

// 	delete node;
// }

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

	// Node* tree;
	try
	{
		// expression = "a+6*7-b - 4 + 4 / 2";
		SymbolTable ST;
		// ST.setVariable("a", 40);
		// ST.setVariable("b", 3);

		// std::cout << "Enter variables: ";
		// int value;
		// while (std::cin >> value)
		// variables.push_back(value);

		std::stringstream line(expression);

		int pos = 0;
		std::vector<std::string> v = lexer(line);
		std::vector<Token> tokens = tokenizer(v);
		std::unique_ptr<Node> tree = parser(tokens, ST, pos);

		for(auto& c:tokens)
			printNode(c);
		// VM vm;
		// vm.compile(tree.get());
		// vm.visualize();
		// std::cout << "\nValue: " << vm.execute(ST) << "\n";
		// clear(tree);
	}
	catch(std::exception& e)
	{
		// clear(tree);
		std::cout << e.what() << '\n';
	}
}
