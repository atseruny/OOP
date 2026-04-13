#include "Node.hpp"

int eval_tree(const Node *tree)
{
	if (!tree)
		throw std::runtime_error("Null tree");
	switch (tree->type)
	{
		case NodeType::Op:
		{
			switch (tree->op)
			{
				case Operator::Add:
					return (eval_tree(tree->left) + eval_tree(tree->right));
				case Operator::Sub:
					return eval_tree(tree->left) - eval_tree(tree->right);
				case Operator::Mult:
					return eval_tree(tree->left) * eval_tree(tree->right);
				case Operator::Div:
				{
					int right = eval_tree(tree->right);
					if (right == 0)
						throw std::runtime_error("Division by zero");
					return eval_tree(tree->left) / right;
				}
				default:
					return 0;
			}
		}
		case NodeType::Num:
			return tree->value;
		case NodeType::Var:
			return *(tree->ptr);
		default:
			return 0;
	}
	return 0;
}