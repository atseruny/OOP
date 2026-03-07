#include "Token.hpp"
#include "Node.hpp"
#include "State.hpp"


State FSM[3][6] =
{
	/* Start */
	{ Wait_operator, Wait_operator, Error, Wait_operand, Error, Error },

	/* Wait_operator */
	{ Error, Error, End, Error, Wait_operator, Wait_operand },

	/* Wait_operand */
	{ Wait_operator, Wait_operator, Error, Wait_operand, Error, Error }
};

Node* parser(std::vector<Token>& tokens)
{
	State state = Start;
	int brackets = 0;
	Node* head = nullptr;
	std::stack<Node*> operators;

	for (const Token &t : tokens)
	{
		state = FSM[state][t.type];

		if (t.type == OpBr)
			brackets++;
		else if (t.type == ClBr)
			brackets--;

		if (state == Error)
			throw std::runtime_error("unexpected token");
		if (state == End)
			break;
	}

	if (state != End || brackets != 0)
		throw std::runtime_error("unexpected end");
	return nullptr;
}