#include "Token.hpp"
#include "Node.hpp"
#include "State.hpp"

Node* parser(std::vector<Token>& tokens);


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
		Node* tree = parser(tokens);
		
		for(auto& c:tokens)
			printNode(c);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
}
