#include "Token.hpp"
#include "Node.hpp"
#include "State.hpp"

Node* parser(std::vector<Token>& tokens, std::vector<int>& vars);
int eval_tree(const Node *tree);


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

void clear(Node* node)
{
	if (!node)
		return;

	clear(node->left);
	clear(node->right);

	delete node;
}

int main()
{
	Node* tree;
	try
	{
		std::vector<int> variables;
		std::string expression;
		std::cout << "Enter expression: ";
		std::getline(std::cin, expression);
		std::cout << "Enter variables: ";
		int value;
		while (std::cin >> value)
			variables.push_back(value);

		std::stringstream line(expression);

		std::vector<std::string> v = lexer(line);
		std::vector<Token> tokens = tokenizer(v);
		tree = parser(tokens, variables);
		// std::cout << "root: " << tree->name << "\n";
		
		// for(auto& c:tokens)
		// 	printNode(c);
		std::cout << "value: " << eval_tree(tree) << "\n";
	}
	catch(std::exception& e)
	{
		clear(tree);
		std::cout << e.what() << '\n';
	}
	clear(tree);
}
