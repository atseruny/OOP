#include "Node.hpp"

int eval_tree(const Node *tree)
{
	if (!tree)
		throw std::runtime_error("Null tree");
	switch (tree->type)
	{
	case Op:
		switch (tree->op)
		{
		case Add:
			return (eval_tree(tree->left) + eval_tree(tree->right));
		case Sub:
			return eval_tree(tree->left) - eval_tree(tree->right);
		case Mult:
			return eval_tree(tree->left) * eval_tree(tree->right);
		case Div:
		{
			int right = eval_tree(tree->right);
			if (right == 0)
				throw std::runtime_error("Division by zero");
			return eval_tree(tree->left) / right;
		}
		}
	case Num:
		return tree->value;
	case Var:
		return *(tree->ptr);
	}
	return 0;
}