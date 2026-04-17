#include "VM.hpp"

VM::VM() : regs(100), next(0) { }

VM::~VM() { }

int VM::compileNum(Node* node)
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

int VM::compileVar(Node* node)
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

int VM::compileOp(Node* node)
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

int VM::compileAssign(Node* node)
{
	size_t addr = node->left->symAddr;

	int rhs = compile(node->right.get());

	program.push_back({
		static_cast<uint8_t>(OpCode::STORE_VAR),
		static_cast<uint8_t>(rhs),
		static_cast<uint8_t>(addr),
		0
	});

	return rhs;
}

int VM::compileBlock(Node* node)
{
	for (auto& stmt : static_cast<const BlockNode*>(node)->statements)
		compile(stmt.get());

	return -1;
}

int VM::compileIf(Node* node)
{
	IfNode* n = dynamic_cast<IfNode*>(node);

	int jmpIndex = compileComp(n->condition.get());

	compile(n->trueBranch.get());

	if (n->falseBranch)
	{
		int jmpEnd = program.size();
		program.push_back({
			static_cast<uint8_t>(OpCode::JMP),
			0,
			0,
			0
		});

		program[jmpIndex].dest = program.size();

		compile(n->falseBranch.get());

		program[jmpEnd].dest = program.size();
	}
	else
		program[jmpIndex].dest = program.size();

	return -1;
}

int VM::compileWhile(Node* node)
{
	WhileNode* n = dynamic_cast<WhileNode*>(node);

	int loopStart = program.size();

	int jmpExit = compileComp(n->condition.get());

	compile(n->body.get());

	program.push_back({
			static_cast<uint8_t>(OpCode::JMP),
			static_cast<uint8_t>(loopStart),
			0,
			0
		});
	
	program[jmpExit].dest = program.size();
	return - 1;
}

int VM::compileComp(Node* node)
{
	int l = compile(node->left.get());
	int r = compile(node->right.get());

	program.push_back({
		static_cast<uint8_t>(OpCode::CMP),
		static_cast<uint8_t>(l),
		static_cast<uint8_t>(r),
		0
	});

	OpCode jmp;

	if (node->name == "==")
		jmp = OpCode::JE;
	else if (node->name == "!=")
		jmp = OpCode::JNE;
	else if (node->name == ">=")
		jmp = OpCode::JGE;
	else if (node->name == "<=")
		jmp = OpCode::JLE;
	else if (node->name == ">")
		jmp = OpCode::JG;
	else if (node->name == "<")
		jmp = OpCode::JL;
	else
		throw std::runtime_error("Unknown comparison");

	int jmpIndex = program.size();

	program.push_back({
		static_cast<uint8_t>(jmp),
		0,
		0,
		0
	});

	return jmpIndex;
}

int VM::compile(Node* node)
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
		case NodeType::If:
			return compileIf(node);
		case NodeType::Comp:
			return compileComp(node);
		case NodeType::While:
			return compileWhile(node);
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
			case OpCode::CMP:
				std::cout << "CMP r" << (int)inst.dest
						<< ", r" << (int)inst.left;
				break;
			case OpCode::JE:
				std::cout << "JNE " << (int)inst.dest;
				break;
			case OpCode::JNE:
				std::cout << "JE " << (int)inst.dest;
				break;
			case OpCode::JL:
				std::cout << "JGE " << (int)inst.dest;
				break;
			case OpCode::JG:
				std::cout << "JLE " << (int)inst.dest;
				break;
			case OpCode::JLE:
				std::cout << "JG " << (int)inst.dest;
				break;
			case OpCode::JGE:
				std::cout << "JL " << (int)inst.dest;
				break;
			case OpCode::JMP:
				std::cout << "JMP " << static_cast<int>(inst.dest);
				break;
		}
		std::cout << "\n";
	}
}

// int VM::execute(SymbolTable& ST)
// {
// 	for (const auto& i : program)
// 	{
// 		switch (static_cast<OpCode>(i.op))
// 		{
// 			case OpCode::LOAD_NUM:
// 				regs[i.dest] = constants[i.left];
// 				break;
// 			case OpCode::LOAD_VAR:
// 				regs[i.dest] = ST.getValueByAddress(i.left);
// 				break;
// 			case OpCode::ADD:
// 				regs[i.dest] = regs[i.left] + regs[i.right];
// 				break;
// 			case OpCode::SUB:
// 				regs[i.dest] = regs[i.left] - regs[i.right];
// 				break;
// 			case OpCode::MUL:
// 				regs[i.dest] = regs[i.left] * regs[i.right];
// 				break;
// 			case OpCode::DIV:
// 				if (regs[i.right] == 0)
// 					throw std::runtime_error("Division by zero");
// 				regs[i.dest] = regs[i.left] / regs[i.right];
// 				break;
// 			case OpCode::STORE_VAR:
// 				ST.setVariableByAddress(i.left, regs[i.dest]);
// 				break;
// 		}
// 	}
// 	return program.empty() ? 0 : regs[program.back().dest];
// }