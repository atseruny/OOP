#include "Token.hpp"
#include "Node.hpp"
#include "State.hpp"

State FSM[3][6] =
{
	// Start
	{ State::Wait_operator, State::Wait_operator, State::Error, State::Wait_operand, State::Error, State::Error },

	// Wait_operator
	{ State::Error, State::Error, State::End, State::Error, State::Wait_operator, State::Wait_operand },

	// Wait_operand
	{ State::Wait_operator, State::Wait_operator, State::Error, State::Wait_operand, State::Error, State::Error }
};

int priority(Node* node)
{
	if (node->name == "*" || node->name == "/")
		return 2;
	if (node->name == "+" || node->name == "-")
		return 1;
	return 0;
}

Node* parser(std::vector<Token>& tokens, std::vector<int>& vars)
{
	State state = State::Start;
	Node* n = nullptr;
	std::stack<Node*> operators;
	std::stack<Node*> operands;
	std::vector<int>::iterator varIt = vars.begin();

	for (const Token &t : tokens)
	{
		state = FSM[static_cast<int>(state)][static_cast<int>(t.type)];

		if (state == State::Error)
			throw std::runtime_error("unexpected token");
		if (state == State::End)
			break;

		n = new Node(t);

		if (t.type == NodeType::Num)
			operands.push(n);
		else if (t.type == NodeType::Var)
		{
			if (varIt == vars.end())
				throw std::runtime_error("Not enough variables");

			n->ptr = &(*varIt);
			operands.push(n);
			++varIt;
		}
		else if (n->type == NodeType::Op)
		{
			while (!operators.empty() &&
					operators.top()->type != NodeType::OpBr &&
					priority(operators.top()) >= priority(n))
			{
				Node* op = operators.top();
				operators.pop();

				Node* right = operands.top(); operands.pop();
				Node* left = operands.top(); operands.pop();

				op->left = left;
				op->right = right;

				operands.push(op);
			}
			operators.push(n);
		}
		else if (t.type == NodeType::OpBr)
			operators.push(n);
		else if (t.type == NodeType::ClBr)
		{
			while (!operators.empty() && operators.top()->type != NodeType::OpBr)
			{
				Node* op = operators.top(); operators.pop();
				Node* right = operands.top(); operands.pop();
				Node* left  = operands.top(); operands.pop();
				op->left = left;
				op->right = right;
				operands.push(op);
			}
			if (operators.empty())
				throw std::runtime_error("Mismatched parentheses");

			Node* left_bracket = operators.top();
			operators.pop();
			delete left_bracket;
			delete n;
		}
	}
	if (state != State::End)
		throw std::runtime_error("unexpected end");
	while (!operators.empty())
	{
		Node* op = operators.top();
		operators.pop();

		Node* right = operands.top(); operands.pop();
		Node* left = operands.top(); operands.pop();

		op->left = left;
		op->right = right;

		operands.push(op);
	}
	return operands.top();
}