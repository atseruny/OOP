#include "Token.hpp"
#include "Node.hpp"
#include "State.hpp"

State FSM[5][6] =
{
/* Start */
	{ Wait_operator, Wait_operator, Error, Wait_operand, Error, Error },

	/* Wait_operator */
	{ Error, Error, End, Error, Wait_operator, Wait_operand },

	/* Wait_operand */
	{ Wait_operator, Wait_operator, Error, Wait_operand, Error, Error },

	/* Error */
	{ Error, Error, Error, Error, Error, Error },

	/* End */
	{ Error, Error, Error, Error, Error, Error }
};

Node* parser(std::vector<Token>& tokens)
{
	State state = Start;

	for (const Token &t : tokens)
	{
		state = FSM[state][t.type];

		if (state == Error)
			throw std::runtime_error("Invalid expression");
		
		if (state == End)
			break;
	}
	return nullptr;
}