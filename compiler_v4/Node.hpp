#pragma once

#include "../parser_v3/NodeType.hpp"
#include <stack>
#include "../parser_v3/Token.hpp"
#include <sstream>

struct Node
{
	int value;
	size_t symAddr;
	Operator op;
	NodeType type;
	Node *left;
	Node *right;
	std::string name;


	Node(const Token& token)
	{
		left = right = nullptr;
		symAddr = 0;
		value = 0;
		name = token.value;
		type = token.type;
		switch (token.type)
		{
		case NodeType::Num:
			value = std::atoi(token.value.c_str());
			break;
		case NodeType::Var:
			name = token.value;
			break;
		case NodeType::Op:
			if (token.value == "+")
				op = Operator::Add;
			else if (token.value == "-")
				op = Operator::Sub;
			else if (token.value == "*")
				op = Operator::Mult;
			else if (token.value == "/")
				op = Operator::Div;
			break;
		default:
			break;
		}
	}
	~Node() {};
};
