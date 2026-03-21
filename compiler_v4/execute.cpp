#include "../parser_v3/Token.hpp"
#include "../parser_v3/Node.hpp"
#include "../parser_v3/State.hpp"
#include "Compiler.hpp"

int execute(const std::vector<Instruction>& prog, VM& vm)
{
	for (auto& i : prog)
	{
		int a = vm.regs[i.left];
		int b = vm.regs[i.right];

		switch (i.op)
		{
			case Operator::Add:
				vm.regs[i.dest] = a + b;
				break;
			case Operator::Sub:
				vm.regs[i.dest] = a - b;
				break;
			case Operator::Mult:
				vm.regs[i.dest] = a * b;
				break;
			case Operator::Div:
				if (b == 0)
					throw std::runtime_error("Division by zero");
				vm.regs[i.dest] = a / b;
				break;
		}
	}

	return prog.empty() ? 0 : vm.regs[prog.back().dest];
}