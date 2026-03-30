#include "VM.hpp"

VM::VM() : regs(100), next(0) { }

VM::~VM() { }

int VM::compile(Node* node)
{
	if (node->type == NodeType::Num)
	{
		int r = next++;
		regs[r] = node->value;
		uint8_t cIdx = constants.size();
		constants.push_back(node->value);
		program.push_back({(uint8_t)OpCode::LOAD_NUM, (uint8_t)r, (uint8_t)cIdx, (uint8_t)0});
		return r;
	}

	if (node->type == NodeType::Var)
	{
		int r = next++;
		program.push_back({(uint8_t)OpCode::LOAD_VAR, (uint8_t)r, (uint8_t)node->symAddr, (uint8_t)0});
		return r;
	}

	int l = compile(node->left);
	int r = compile(node->right);

	int dest = next++;

	OpCode op;
	switch (node->op)
	{
		case Operator::Add:  op = OpCode::ADD; break;
		case Operator::Sub:  op = OpCode::SUB; break;
		case Operator::Mult: op = OpCode::MUL; break;
		case Operator::Div:  op = OpCode::DIV; break;
		default: throw std::runtime_error("Unknown operator");
	}
	program.push_back({(uint8_t)op, (uint8_t)dest, (uint8_t)l, (uint8_t)r});
	return dest;
}


void VM::visualize() const
{
	std::cout << "\n[VM Bytecode Visualization]\n";
	std::cout << std::left
			<< std::setw(6)  << "Addr"
			<< std::setw(12) << "OpCode"
			<< std::setw(6)  << "L"
			<< std::setw(6)  << "R"
			<< std::setw(6)  << "Dst"
			<< "Value\n";
	std::cout << std::string(45, '-') << "\n";

	for (size_t i = 0; i < program.size(); i++)
	{
		const auto& inst = program[i];
		std::cout << "[" << std::setw(3) << i << "]  ";
		std::cout << std::left << std::setw(12);

		switch (static_cast<OpCode>(inst.op))
		{
		case OpCode::LOAD_NUM:
			std::cout << "LOAD_CONST"
					<< std::setw(6) << "-"
					<< std::setw(6) << "-"
					<< std::setw(6) << (int)inst.dest
					<< constants[inst.left];
			break;
		case OpCode::LOAD_VAR:
			std::cout << "LOAD_VAR"
					<< std::setw(6) << (int)inst.left
					<< std::setw(6) << "-"
					<< std::setw(6) << (int)inst.dest
					<< "*(" << (int)inst.dest << ")";
			break;
		case OpCode::ADD:
			std::cout << "ADD"
					<< std::setw(6) << (int)inst.left
					<< std::setw(6) << (int)inst.right
					<< std::setw(6) << (int)inst.dest
					<< "*(" << (int)inst.left << ") + *(" << (int)inst.right << ")";
			break;
		case OpCode::SUB:
			std::cout << "SUB"
					<< std::setw(6) << (int)inst.left
					<< std::setw(6) << (int)inst.right
					<< std::setw(6) << (int)inst.dest
					<< "*(" << (int)inst.left << ") - *(" << (int)inst.right << ")";
			break;
		case OpCode::MUL:
			std::cout << "MUL"
					<< std::setw(6) << (int)inst.left
					<< std::setw(6) << (int)inst.right
					<< std::setw(6) << (int)inst.dest
					<< "*(" << (int)inst.left << ") * *(" << (int)inst.right << ")";
			break;
		case OpCode::DIV:
			std::cout << "DIV"
					<< std::setw(6) << (int)inst.left
					<< std::setw(6) << (int)inst.right
					<< std::setw(6) << (int)inst.dest
					<< "*(" << (int)inst.left << ") / *(" << (int)inst.right << ")";
			break;
		}
		std::cout << "\n";
	}
}


int VM::execute(SymbolTable& ST)
{
	for (const auto& i : program)
	{
		switch (static_cast<OpCode>(i.op))
		{
			case OpCode::LOAD_NUM:
				regs[i.dest] = constants[i.left];
				break;
			case OpCode::LOAD_VAR:
				regs[i.dest] = ST.getValueByAddress(i.left);
				break;
			case OpCode::ADD:
				regs[i.dest] = regs[i.left] + regs[i.right];
				break;
			case OpCode::SUB:
				regs[i.dest] = regs[i.left] - regs[i.right];
				break;
			case OpCode::MUL:
				regs[i.dest] = regs[i.left] * regs[i.right];
				break;
			case OpCode::DIV:
				if (regs[i.right] == 0)
					throw std::runtime_error("Division by zero");
				regs[i.dest] = regs[i.left] / regs[i.right];
				break;
		}
	}
	return program.empty() ? 0 : regs[program.back().dest];
}