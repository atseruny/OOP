#include "../parser_v3/Token.hpp"
#include "Node.hpp"
#include "../parser_v3/State.hpp"
#include "SymbolTable.hpp"

State FSM[3][6] =
{
	// Start
	{ Wait_operator, Wait_operator, Error, Wait_operand, Error, Error },

	// Wait_operator
	{ Error, Error, End, Error, Wait_operator, Wait_operand },

	// Wait_operand
	{ Wait_operator, Wait_operator, Error, Wait_operand, Error, Error }
};

int priority(Node* node)
{
	if (node->name == "*" || node->name == "/")
		return 2;
	if (node->name == "+" || node->name == "-")
		return 1;
	return 0;
}


std::unique_ptr<Node> parser(std::vector<Token>& tokens, SymbolTable& ST)
{
	State state = Start;

	std::stack<std::unique_ptr<Node>> operators;
	std::stack<std::unique_ptr<Node>> operands;

	for (const Token& t : tokens)
	{
		state = FSM[state][t.type];

		if (state == Error)
			throw std::runtime_error("unexpected token");
		if (state == End)
			break;

		std::unique_ptr<Node> n = std::make_unique<Node>(t);

		if (t.type == NodeType::Num)
			operands.push(std::move(n));
		else if (t.type == NodeType::Var)
		{
			n->symAddr = ST.getAddress(n->name);
			operands.push(std::move(n));
		}
		else if (t.type == NodeType::Op)
		{
			while (!operators.empty() &&
				operators.top()->type != OpBr &&
				priority(operators.top().get()) >= priority(n.get()))
			{
				std::unique_ptr<Node> op = std::move(operators.top());
				operators.pop();

				if (operands.size() < 2)
					throw std::runtime_error("invalid expression");

				std::unique_ptr<Node> right = std::move(operands.top()); operands.pop();
				std::unique_ptr<Node> left  = std::move(operands.top()); operands.pop();

				op->left  = std::move(left);
				op->right = std::move(right);

				operands.push(std::move(op));
			}
			operators.push(std::move(n));
		}
		else if (t.type == OpBr)
			operators.push(std::move(n));
		else if (t.type == ClBr)
		{
			while (!operators.empty() && operators.top()->type != OpBr)
			{
				std::unique_ptr<Node> op = std::move(operators.top());
				operators.pop();

				if (operands.size() < 2)
					throw std::runtime_error("invalid expression");

				std::unique_ptr<Node> right = std::move(operands.top()); operands.pop();
				std::unique_ptr<Node> left  = std::move(operands.top()); operands.pop();

				op->left  = std::move(left);
				op->right = std::move(right);

				operands.push(std::move(op));
			}

			if (operators.empty())
				throw std::runtime_error("Mismatched parentheses");

			operators.pop();
		}
	}

	if (state != End)
		throw std::runtime_error("unexpected end");

	while (!operators.empty())
	{
		std::unique_ptr<Node> op = std::move(operators.top());
		operators.pop();

		if (operands.size() < 2)
			throw std::runtime_error("invalid expression");

		std::unique_ptr<Node> right = std::move(operands.top()); operands.pop();
		std::unique_ptr<Node> left  = std::move(operands.top()); operands.pop();

		op->left  = std::move(left);
		op->right = std::move(right);

		operands.push(std::move(op));
	}

	return std::move(operands.top());
}