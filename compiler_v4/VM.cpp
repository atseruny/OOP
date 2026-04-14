#include "VM.hpp"

VM::VM() : regs(100), next(0) { }

VM::~VM() { }

int VM::compileNum(const Node* node)
{
	int dest = next++;

	uint8_t cIdx = constants.size();
	constants.push_back(node->value);

	program.push_back({
		static_cast<uint8_t>(OpCode::LOAD_NUM),
		static_cast<uint8_t>(dest),
		static_cast<uint8_t>(cIdx),
		0
	});

	return dest;
}

int VM::compileVar(const Node* node)
{
	int dest = next++;

	program.push_back({
		static_cast<uint8_t>(OpCode::LOAD_VAR),
		static_cast<uint8_t>(dest),
		static_cast<uint8_t>(node->symAddr),
		0
	});

	return dest;
}

int VM::compileOp(const Node* node)
{
	int l = compile(node->left.get());
	int r = compile(node->right.get());

	int dest = next++;

	OpCode op;
	switch (node->op)
	{
		case Operator::Add:  op = OpCode::ADD; break;
		case Operator::Sub:  op = OpCode::SUB; break;
		case Operator::Mult: op = OpCode::MUL; break;
		case Operator::Div:  op = OpCode::DIV; break;
		default:
			throw std::runtime_error("Unknown operator");
	}

	program.push_back({
		static_cast<uint8_t>(op),
		static_cast<uint8_t>(dest),
		static_cast<uint8_t>(l),
		static_cast<uint8_t>(r)
	});

	return dest;
}

int VM::compileAssign(const Node* node)
{
	size_t addr = node->left->symAddr;

	int rhs = compile(node->right.get());

	//std::cout<<"rhs: "<<rhs<<"\n";
	program.push_back({
		static_cast<uint8_t>(OpCode::STORE_VAR),
		static_cast<uint8_t>(rhs),
		static_cast<uint8_t>(addr),
		0
	});

	return rhs;
}

int VM::compileBlock(const Node* node)
{
	//std::cout<<"From\n";

	for (auto& stmt : static_cast<const BlockNode*>(node)->statements)
	{
		//std::cout<<"Here\n";
		compile(stmt.get());
	}

	return -1;
}

int VM::compile(const Node* node)
{
	if (!node)
		return -1;

	switch (node->type)
	{
		case NodeType::Num:
			return compileNum(node);
		case NodeType::Var:
			return compileVar(node);
		case NodeType::Op:
			return compileOp(node);
		case NodeType::Assign:
			return compileAssign(node);
		case NodeType::Block:
			return compileBlock(node);
		default:
			throw std::runtime_error("Unknown node type in compile");
	}
}

void VM::visualize() const
{
	std::cout << "\n[VM Assembly]\n";
	std::cout << "----------------------\n";

	for (size_t i = 0; i < program.size(); i++)
	{
		const auto& inst = program[i];

		std::cout << std::setw(3) << i << ": ";

		switch (static_cast<OpCode>(inst.op))
		{
			case OpCode::LOAD_NUM:
				std::cout << "MOV r" << static_cast<int>(inst.dest)
						  << ", " << constants[inst.left];
				break;
			case OpCode::LOAD_VAR:
				std::cout << "MOV r" << static_cast<int>(inst.dest)
						  << ", [" << static_cast<int>(inst.left) << "]";
				break;
			case OpCode::ADD:
				std::cout << "ADD r" << static_cast<int>(inst.dest)
						  << ", r" << static_cast<int>(inst.left)
						  << ", r" << static_cast<int>(inst.right);
				break;
			case OpCode::SUB:
				std::cout << "SUB r" << static_cast<int>(inst.dest)
						  << ", r" << static_cast<int>(inst.left)
						  << ", r" << static_cast<int>(inst.right);
				break;
			case OpCode::MUL:
				std::cout << "MUL r" << static_cast<int>(inst.dest)
						  << ", r" << static_cast<int>(inst.left)
						  << ", r" << static_cast<int>(inst.right);
				break;
			case OpCode::DIV:
				std::cout << "DIV r" << static_cast<int>(inst.dest)
						  << ", r" << static_cast<int>(inst.left)
						  << ", r" << static_cast<int>(inst.right);
				break;
			case OpCode::STORE_VAR:
				std::cout << "MOV [" << static_cast<int>(inst.left)
						  << "], r"  << static_cast<int>(inst.dest);
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
			case OpCode::STORE_VAR:
				ST.setVariableByAddress(i.left, regs[i.dest]);
				break;
		}
	}
	return program.empty() ? 0 : regs[program.back().dest];
}