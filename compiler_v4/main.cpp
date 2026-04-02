#include "../parser_v3/Token.hpp"
#include "../parser_v3/State.hpp"
#include "SymbolTable.hpp"
#include "VM.hpp"

std::unique_ptr<Node> parser(std::vector<Token>& tokens, SymbolTable& ST);
std::vector<std::string> lexer(std::stringstream& line);

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

int main()
{
	// Node* tree;
	try
	{
		std::string expression = "a+6*7-b - 4 + 4 / 2";
		SymbolTable ST;
		ST.setVariable("a", 40);
		ST.setVariable("b", 3);

		// std::cout << "Enter expression: ";
		// std::getline(std::cin, expression);
		// std::cout << "Enter variables: ";
		// int value;
		// while (std::cin >> value)
		// variables.push_back(value);

		std::stringstream line(expression);

		std::vector<std::string> v = lexer(line);
		std::vector<Token> tokens = tokenizer(v);
		std::unique_ptr<Node> tree = parser(tokens, ST);

		// for(auto& c:tokens)
		// 	printNode(c);
		VM vm;
		vm.compile(tree.get());
		vm.visualize();
		std::cout << "\nValue: " << vm.execute(ST) << "\n";
		// clear(tree);
	}
	catch(std::exception& e)
	{
		// clear(tree);
		std::cout << e.what() << '\n';
	}
}
