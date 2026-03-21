#pragma once

#include <iostream>
#include <exception>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "NodeType.hpp"
#include <stack>
#include "Token.hpp"

struct Node
{
	int value;
	int* ptr;
	Operator op;
	NodeType type;
	Node *left;
	Node *right;
	std::string name;


	Node(const Token& token)
	{
		left = right = nullptr;
		ptr = nullptr;
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

Node *parse_expr(char *s);
int eval_tree(const Node *tree, const std::map<std::string, int> &vars);
void destroy_tree(Node *n);
void parse_vars(char *var, std::map<std::string, int> &vars);
std::vector<std::string> lexer(std::stringstream &line);
