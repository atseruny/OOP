#include "Expr.hpp"

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

// std::unique_ptr<Node> parseCondition(std::vector<Token>& tokens, SymbolTable& ST)
// {
	
// }

// std::unique_ptr<Node> parseIf(std::vector<Token>& tokens, SymbolTable& ST)
// {
//     std::cout << "Is if\n";

//     int i = 1; // skip IF

//     if (tokens[i].type != NodeType::OpBr)
//         throw std::runtime_error("Expected (");

//     i++; // now at condition start

//     // collect condition until ')'
//     std::vector<Token> cond;

//     while (i < tokens.size() && tokens[i].type != NodeType::ClBr && tokens[i].type != NodeType::EofEx)
//     {
//         cond.push_back(tokens[i]);
//         i++;
//     }

//     if (i == tokens.size())
//         throw std::runtime_error("Missing )");

//     i++; // skip ')'

//     // parse condition
//     auto condition = parseCondition(cond, ST);

//     // rest = body (everything after ')')
//     std::vector<Token> body(tokens.begin() + i, tokens.end());

//     auto bodyNode = parser(body, ST);

//     auto node = std::make_unique<Node>(tokens[0]);
//     node->type = NodeType::If;
//     node->left = std::move(condition);
//     node->right = std::move(bodyNode);

//     return node;
// }

// std::unique_ptr<Node> parseWhile(std::vector<Token>& tokens, SymbolTable& ST)
// {
// 	std::cout << "Is while\n";
// }

std::unique_ptr<Node> parseExpression(std::vector<Token>& tokens, SymbolTable& ST)
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

std::unique_ptr<Node> parseAssign(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	if (tokens[pos].type != NodeType::Var)
		throw std::runtime_error("Expected variable name");
	
	if (!ST.isDeclared(tokens[pos].value))
		throw std::runtime_error("Variable " + tokens[pos].value + " is not declared");
		
	auto node = std::make_unique<Node>(NodeType::Assign);
	node->left = std::make_unique<Node>(tokens[pos]);
	node->left->symAddr = ST.getAddress(tokens[pos].value);

	pos++;

	if (tokens[pos].type != NodeType::Assign)
		throw std::runtime_error("Expected =");

	pos++;

	std::vector<Token> expr;
	while (tokens[pos].type != NodeType::Semi)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Missing ;");
		expr.push_back(tokens[pos++]);
	}
	expr.push_back(Token("", NodeType::EofEx));
	node->right = parseExpression(expr, ST);
	pos++;
	return node;
}

std::unique_ptr<Node> parseIf(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	if (tokens[pos].type != NodeType::If)
		throw std::runtime_error("Expected if");
	
	pos++;
	std::vector<Token> cond;
	
	if (tokens[pos].type != NodeType::OpBr)
		throw std::runtime_error("Expected (");
	
	pos++;
	
	while (tokens[pos].type != NodeType::ClBr)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Missing )");
		cond.push_back(tokens[pos++]);
	}
	cond.push_back(Token("", NodeType::EofEx));
	
	pos++;
	
	auto node = std::make_unique<IfNode>();
	node->condition = parseExpression(cond, ST);
	
	if (tokens[pos].type != NodeType::OpBody)
		throw std::runtime_error("Expected {");
	
	node->trueBranch = parseBlock(tokens, ST, pos);
	
	if (tokens[pos].type == NodeType::Else)
	{
		pos++;
		if (tokens[pos].type == NodeType::If)
			node->falseBranch = parseIf(tokens, ST, pos);
		else if (tokens[pos].type == NodeType::OpBody)
			node->falseBranch = parseBlock(tokens, ST, pos);
		else
			throw std::runtime_error("Expected if or { after else");
	}
	return node;
}


std::unique_ptr<Node> parseVarDecl(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	if (tokens[pos].type != NodeType::Decl)
		throw std::runtime_error("Expected type");

	pos++;

	if (tokens[pos].type != NodeType::Var)
		throw std::runtime_error("Expected variable name");

	std::string varName = tokens[pos].value;

	size_t addr = ST.getAddress(varName);

	pos++;

	if (tokens[pos].type != NodeType::Semi)
		throw std::runtime_error("Missing ; after declaration");

	pos++;

	auto node = std::make_unique<Node>(NodeType::Decl);
	node->name = varName;
	node->symAddr = addr;

	return node;
}


std::unique_ptr<Node> parseStatement(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	if (tokens[pos].type == NodeType::If)
		return parseIf(tokens, ST, pos);

	// if (tokens[pos].type == NodeType::While)
	//     return parseWhile(tokens, ST, pos);

	if (tokens[pos].type == NodeType::Decl)
		return parseVarDecl(tokens, ST, pos);

	if (tokens[pos].type == NodeType::Var &&
		tokens[pos + 1].type == NodeType::Assign)
		return parseAssign(tokens, ST, pos);

	// expression
	// auto node = parseExpression(tokens, ST); // ⚠️ you must adapt this later
	// pos++; // temporary fix

	// return node;
}

std::unique_ptr<Node> parseBlock(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	auto block = std::make_unique<BlockNode>();

	if (tokens[pos].type != NodeType::OpBody)
		throw std::runtime_error("Expected {");

	pos++;

	while (tokens[pos].type != NodeType::ClBody)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Missing }");

		block->statements.push_back(parseStatement(tokens, ST, pos));
	}

	pos++;

	return block;
}


std::unique_ptr<Node> parser(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	if (tokens[pos].type == NodeType::OpBody)
		return parseBlock(tokens, ST, pos);
	// if (tokens[0].type == NodeType::If)
	// 	return parseIf(tokens, ST);

	// if (tokens[0].type == NodeType::While)
	// 	return parseWhile(tokens, ST);

	// // fallback → expression
	// return parseExpression(tokens, ST);
}

