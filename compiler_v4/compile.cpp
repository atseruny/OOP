#include "../parser_v3/Token.hpp"
#include "../parser_v3/Node.hpp"
#include "../parser_v3/State.hpp"
#include "Compiler.hpp"

int compile(Node* node, std::vector<Instruction>& prog, VM& vm)
{
	if (node->type == NodeType::Num)
	{
		int r = vm.next++;
		vm.regs[r] = node->value;
		return r;
	}

	if (node->type == NodeType::Var)
	{
		int r = vm.next++;
		vm.regs[r] = *(node->ptr);
		return r;
	}

	int l = compile(node->left, prog, vm);
	int r = compile(node->right, prog, vm);

	int dest = vm.next++;

	Operator op;
	op = node->op;

	prog.push_back({op, dest, l, r});
	return dest;
}