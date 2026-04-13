#pragma once

#include <iostream>
#include <exception>
#include <map>

enum class NodeType
{
	ADD,
	SUB,
	DIV,
	MULTI,
	VAL,
	VAR
};

struct Node
{
	int value;
	NodeType type;
	Node* left;
	Node* right;
	std::string var;

	Node(int v, NodeType _type, Node* l, Node *r, const std::string& var_name) : value(v), type(_type), left(l), right(r), var(var_name) {};
	Node(int v, NodeType _type, Node* l, Node *r) : value(v), type(_type), left(l), right(r) {};
	Node() : value(0), type(NodeType::VAL), left(nullptr), right(nullptr) {};
	Node(int v) : value(v), type(NodeType::VAL), left(nullptr), right(nullptr) {};
	Node(const std::string _var) : value(0), type(NodeType::VAR), left(nullptr), right(nullptr), var(_var) {};
	~Node() { };
};

Node *parse_expr(char *s);
int eval_tree(const Node *tree, const std::map<std::string, int>& vars);
void destroy_tree(Node *n);
void parse_vars(char* var, std::map<std::string, int>& vars);
