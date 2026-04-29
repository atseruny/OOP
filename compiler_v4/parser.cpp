#include "Compiler.hpp"

int insideFunction = 0;

int priority(Node* node)
{
	if (node->name == "*" || node->name == "/")
		return 2;
	if (node->name == "+" || node->name == "-")
		return 1;
	return 0;
}

std::unique_ptr<Node> parseExpression(std::vector<Token>& tokens, SymbolTable& ST)
{
	State state = State::Start;

	std::stack<std::unique_ptr<Node>> operators;
	std::stack<std::unique_ptr<Node>> operands;

	for (const Token& t : tokens)
	{
		state = FSM[static_cast<int>(state)][static_cast<int>(t.type)];

		if (state == State::Error)
			throw std::runtime_error("unexpected token " + t.value);
		if (state == State::End)
			break;

		std::unique_ptr<Node> n = std::make_unique<Node>(t);

		if (t.type == NodeType::Num)
			operands.push(std::move(n));
		else if (t.type == NodeType::Var)
		{
			n->symAddr = ST.getAddress(n->name);
			operands.push(std::move(n));
		}
		else if (t.type == NodeType::Op || t.type == NodeType::Comp)
		{
			while (!operators.empty() &&
				operators.top()->type != NodeType::OpBr &&
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
		else if (t.type == NodeType::OpBr)
			operators.push(std::move(n));
		else if (t.type == NodeType::ClBr)
		{
			while (!operators.empty() && operators.top()->type != NodeType::OpBr)
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

	if (state != State::End)
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
		throw std::runtime_error("Expected variable name " + tokens[pos].value);
	
	if (!ST.isDeclared(tokens[pos].value))
		throw std::runtime_error("Variable " + tokens[pos].value + " is not declared");
		
	auto node = std::make_unique<Node>(NodeType::Assign);
	node->left = std::make_unique<Node>(tokens[pos]);
	node->left->symAddr = ST.getAddress(tokens[pos].value);

	pos++;

	if (tokens[pos].type != NodeType::Assign)
		throw std::runtime_error("Expected = " + tokens[pos].value);

	pos++;

	std::vector<Token> expr;
	while (tokens[pos].type != NodeType::Semi)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Expected ; " + tokens[pos].value);
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
		throw std::runtime_error("Expected if " + tokens[pos].value);
	
	pos++;
	std::vector<Token> cond;
	
	if (tokens[pos].type != NodeType::OpBr)
		throw std::runtime_error("Expected ( " + tokens[pos].value);
	
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
	
	if (tokens[pos].type == NodeType::OpBody)
		node->trueBranch = parseBlock(tokens, ST, pos);
	else
		node->trueBranch = parseStatement(tokens, ST, pos);
	// if (tokens[pos].type != NodeType::OpBody)
	// 	throw std::runtime_error("Expected {");
	
	// node->trueBranch = parseBlock(tokens, ST, pos);
	
	if (tokens[pos].type == NodeType::Else)
	{
		pos++;
		if (tokens[pos].type == NodeType::If)
			node->falseBranch = parseIf(tokens, ST, pos);
		else if (tokens[pos].type == NodeType::OpBody)
			node->falseBranch = parseBlock(tokens, ST, pos);
		else
			node->falseBranch = parseStatement(tokens, ST, pos);
	}
	return node;
}

std::unique_ptr<Node> parseWhile(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	if (tokens[pos].type != NodeType::While)
		throw std::runtime_error("Expected while " + tokens[pos].value);
	
	pos++;
	std::vector<Token> cond;
	
	if (tokens[pos].type != NodeType::OpBr)
		throw std::runtime_error("Expected ( " + tokens[pos].value);
	
	pos++;
	
	while (tokens[pos].type != NodeType::ClBr)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Expected ) " + tokens[pos].value);
		cond.push_back(tokens[pos++]);
	}
	cond.push_back(Token("", NodeType::EofEx));
	
	pos++;
	
	auto node = std::make_unique<WhileNode>();
	node->condition = parseExpression(cond, ST);
	
	if (tokens[pos].type != NodeType::OpBody)
		throw std::runtime_error("Expected { " + tokens[pos].value);
	
	node->body = parseBlock(tokens, ST, pos);
	
	return node;
}


std::unique_ptr<Node> parseVarDecl(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	if (tokens[pos].type != NodeType::Type)
		throw std::runtime_error("Expected type" + tokens[pos].value);

	pos++;
	if (tokens[pos].type != NodeType::Var)
		throw std::runtime_error("Expected variable name" + tokens[pos].value);
	
	ST.declareVariable(tokens[pos].value);

	std::string varName = tokens[pos].value;
	
	if (tokens[pos + 1].type == NodeType::Assign)
		return parseAssign(tokens, ST, pos);
	
	pos++;

	if (tokens[pos].type != NodeType::Semi)
		throw std::runtime_error("Expected ';' after declaration" + tokens[pos].value);

	pos++;

	return nullptr;
}

std::unique_ptr<Node> parseReturn(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	if (tokens[pos].type != NodeType::Ret)
		throw std::runtime_error("Expected return");

	pos++;

	auto node = std::make_unique<ReturnNode>();

	// return;
	if (tokens[pos].type == NodeType::Semi)
	{
		pos++;
		return node;
	}

	std::vector<Token> expr;

	while (tokens[pos].type != NodeType::Semi)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Missing ';' after return");

		expr.push_back(tokens[pos++]);
	}

	pos++; // skip ';'
	expr.push_back(Token("", NodeType::EofEx));

	node->expr = parseExpression(expr, ST);

	return node;
}

std::unique_ptr<Node> parseStatement(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	if (static_cast<size_t>(pos) >= tokens.size())
		throw std::runtime_error("Unexpected end of input");

	switch (tokens[pos].type)
	{
		case NodeType::If:
			return parseIf(tokens, ST, pos);
		case NodeType::Type:
			return parseVarDecl(tokens, ST, pos);
		case NodeType::Var:
		{
			if (static_cast<size_t>(pos + 1) < tokens.size() && tokens[pos + 1].type == NodeType::Assign)
				return parseAssign(tokens, ST, pos);
			break;
		}
		case NodeType::OpBody:
			return parseBlock(tokens, ST, pos);
		case NodeType::While:
			return parseWhile(tokens, ST, pos);
		case NodeType::Ret:
		{
			if (!insideFunction)
				throw std::runtime_error("return outside of function");
			return parseReturn(tokens, ST, pos);
		}
		default:
			break;
	}
	std::vector<Token> expr;
	while (static_cast<size_t>(pos) < tokens.size() && tokens[pos].type != NodeType::Semi)
		expr.push_back(tokens[pos++]);

	if (static_cast<size_t>(pos) >= tokens.size())
		throw std::runtime_error("Missing ';'");
	expr.push_back(Token("", NodeType::EofEx));
	pos++;
	return parseExpression(expr, ST);
}

std::unique_ptr<Node> parseBlock(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	auto block = std::make_unique<BlockNode>();

	if (tokens[pos].type != NodeType::OpBody)
		throw std::runtime_error("Expected { " + tokens[pos].value);

	pos++;

	ST.enterScope();

	while (tokens[pos].type != NodeType::ClBody)
	{
		if (tokens[pos].type == NodeType::EofEx)
			throw std::runtime_error("Expected } " + tokens[pos].value);

		auto stmt = parseStatement(tokens, ST, pos);

		if (stmt)
			block->statements.push_back(std::move(stmt));
	}

	ST.exitScope();

	pos++;

	return block;
}


std::unique_ptr<Node> parseFunction(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	if (tokens[pos].type != NodeType::Func)
		throw std::runtime_error("Expected keyword \"Func\" " + tokens[pos].value);

	pos++;
	if (tokens[pos].type != NodeType::Type)
		throw std::runtime_error("Expected a type " + tokens[pos].value);
	
	auto func = std::make_unique<FuncNode>();
	if (tokens[pos].value == "int")
		func->retType = ReturnType::_int;
	else if (tokens[pos].value == "void")
		func->retType = ReturnType::_void;
	else
		throw std::runtime_error("Expected a type " + tokens[pos].value);

	pos++;
	
	if (tokens[pos].type != NodeType::Var)
		throw std::runtime_error("Expected function name " + tokens[pos].value);

	func->name = tokens[pos].value;

	pos++;

	if (tokens[pos].type != NodeType::OpBr)
		throw std::runtime_error("Expected ( "  + tokens[pos].value);

	pos++;

	// ---- PARAMETERS ----
	while (tokens[pos].type != NodeType::ClBr)
	{
		if (tokens[pos].type != NodeType::Type)
			throw std::runtime_error("Expected parameter type " + tokens[pos].value);

		std::string type = tokens[pos].value;
		pos++;

		if (tokens[pos].type != NodeType::Var)
			throw std::runtime_error("Expected parameter name " + tokens[pos].value);

		std::string name = tokens[pos].value;
		pos++;

		func->params.push_back({type, name});

		if (tokens[pos].type == NodeType::Semi)
			pos++; // skip ','
		else if (tokens[pos].type != NodeType::ClBr)
			throw std::runtime_error("Expected ',' or ')' " + tokens[pos].value);

	}

	pos++; // skip ')'

	// ---- FUNCTION SCOPE ----
	ST.enterScope();
	for (const auto& p : func->params)
	{
		if (ST.isDeclared(p.name))
			throw std::runtime_error("Duplicate parameter: " + p.name);

		ST.declareVariable(p.name);
	}

	// ---- BODY ----
	if (tokens[pos].type != NodeType::OpBody)
		throw std::runtime_error("Expected { for function body " + tokens[pos].value);
	
	insideFunction = 1;
    func->body = std::unique_ptr<BlockNode>(
        static_cast<BlockNode*>(parseBlock(tokens, ST, pos).release())
    );
	insideFunction = 0;
	ST.exitScope();
	for (auto& stmt : func->body->statements)
	{
		if (stmt->type == NodeType::Ret)
		{
			func->returnNode = static_cast<ReturnNode*>(stmt.get());
			break;
		}
	}
	return func;
}


std::vector<std::unique_ptr<Node>> parser(std::vector<Token>& tokens, SymbolTable& ST, int& pos)
{
	std::vector<std::unique_ptr<Node>> functions;
	// auto block = std::make_unique<BlockNode>();


	while (tokens[pos].type != NodeType::EofEx)
	{
		int oldPos = pos;

		if (tokens[pos].type != NodeType::Func)
			throw std::runtime_error("Only functions allowed at global scope");

		functions.push_back(parseFunction(tokens, ST, pos));

		if (pos == oldPos)
			throw std::runtime_error("Parser stuck (no progress)");
	}

	return functions;
}
