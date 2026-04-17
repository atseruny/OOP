#include "Compiler.hpp"

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

void printAST(Node* node)
{
	if (!node)
		return;


	switch (node->type)
	{
		case NodeType::Num:
			std::cout << "Num(" << node->value << ")\n";
			return;
		case NodeType::Var:
			std::cout << "Var(addr=" << node->symAddr << ")\n";
			return;
		case NodeType::Assign:
			std::cout << "Assign\n";
			break;
		case NodeType::Op:
		{
			switch (node->op)
			{
			case Operator::Add:
				std::cout << "Add\n";
				break;
			case Operator::Sub:
				std::cout << "Sub\n";
				break;
			case Operator::Mult:
				std::cout << "Mult\n";
				break;
			case Operator::Div:
				std::cout << "Div\n";
				break;
			default:
				break;
			}
			break;
		}
		case NodeType::Block:
		{
			std::cout << "Block\n";
			BlockNode* n = dynamic_cast<BlockNode*>(node);
			for (size_t i = 0; i < n->statements.size(); i++)
			{
				printAST(n->statements[i].get());
			}
			return;
		}
		default:
			std::cout << "NodeType(" << (int)node->type << ")\n";
			break;
	}

	if (node->left)
	{
		std::cout << "L:";
		printAST(node->left.get());
	}

	if (node->right)
	{
		std::cout << "R:";
		printAST(node->right.get());
	}
}



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

		int pos = 0;
		std::vector<std::string> v = lexer(line);
		std::vector<Token> tokens = tokenizer(v);
		std::unique_ptr<Node> tree = parser(tokens, ST, pos);

		// printAST(tree.get());
		// for(auto& c:tokens)
		// 	printNode(c);
		VM vm;
		vm.compile(tree.get());
		vm.visualize();
		vm.writeInExe();

		// std::cout << "\nValue: " << vm.execute(ST) << "\n";
	}
	catch(std::exception& e)
	{

		std::cout << e.what() << '\n';
	}
}
