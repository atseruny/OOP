#include "../includes/VM.hpp"

VM::VM() : regs(100), next(0), cmpFlag(0) { }

VM::~VM() { }

int VM::compileNum(Node* node)
{
	int dest = next++;

	uint8_t cIdx = static_cast<uint8_t>(constants.size());
	constants.push_back(node->value);

	program.push_back({
		static_cast<uint8_t>(OpCode::LOAD_NUM),
		static_cast<uint8_t>(dest),
		cIdx,
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
	const BlockNode* bnode = static_cast<const BlockNode*>(node);
	for (auto& stmt : bnode->statements)
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
			0, 0, 0
		});

		program[jmpIndex].dest = static_cast<uint8_t>(program.size());

		compile(n->falseBranch.get());

		program[jmpEnd].dest = static_cast<uint8_t>(program.size());
	}
	else
		program[jmpIndex].dest = static_cast<uint8_t>(program.size());

	return -1;
}

int VM::compileWhile(Node* node)
{
	WhileNode* n = dynamic_cast<WhileNode*>(node);

	int loopStart = static_cast<int>(program.size());

	int jmpExit = compileComp(n->condition.get());

	compile(n->body.get());

	program.push_back({
		static_cast<uint8_t>(OpCode::JMP),
		static_cast<uint8_t>(loopStart),
		0, 0
	});

	program[jmpExit].dest = static_cast<uint8_t>(program.size());

	return -1;
}

int VM::compileComp(Node* node)
{
	int l = compile(node->left.get());
	int r = compile(node->right.get());

	program.push_back({
		static_cast<uint8_t>(OpCode::CMP),
		0,
		static_cast<uint8_t>(l),
		static_cast<uint8_t>(r)
	});

	OpCode jmp;
	if      (node->name == "==") jmp = OpCode::JNE;
	else if (node->name == "!=") jmp = OpCode::JE;
	else if (node->name == ">=") jmp = OpCode::JL;
	else if (node->name == "<=") jmp = OpCode::JG;
	else if (node->name == ">")  jmp = OpCode::JLE;
	else if (node->name == "<")  jmp = OpCode::JGE;
	else
		throw std::runtime_error("Unknown comparison operator: " + node->name);

	int jmpIndex = static_cast<int>(program.size());

	program.push_back({
		static_cast<uint8_t>(jmp),
		0, 0, 0
	});

	return jmpIndex;
}

void VM::compileFunction(Node* node)
{
	FuncNode* fn = static_cast<FuncNode*>(node);
	functions[fn->name] = static_cast<int>(program.size());

	int skipJmp = static_cast<int>(program.size());
	program.push_back({
		static_cast<uint8_t>(OpCode::JMP),
		0, 0, 0
	});

	int savedNext = next;
	next = 0;

	compile(fn->body.get());

	if (program.empty() ||
		static_cast<OpCode>(program.back().op) != OpCode::RET)
	{
		program.push_back({
			static_cast<uint8_t>(OpCode::RET),
			0, 0, 0
		});
	}

	next = savedNext;

	program[skipJmp].dest = static_cast<uint8_t>(program.size());
}

int VM::compileCall(Node* node)
{
	CallNode* cnode = static_cast<CallNode*>(node);

	for (auto& arg : cnode->args)
		compile(arg.get());

	auto it = functions.find(cnode->name);
	if (it == functions.end())
		throw std::runtime_error("Undefined function: " + cnode->name);

	int dest = next++;

	program.push_back({
		static_cast<uint8_t>(OpCode::CALL),
		static_cast<uint8_t>(dest),
		static_cast<uint8_t>(it->second),
		0
	});

	return dest;
}

int VM::compileReturn(Node* node)
{
	ReturnNode* ret = static_cast<ReturnNode*>(node);

	if (ret->expr)
	{
		int reg = compile(ret->expr.get());

		program.push_back({
			static_cast<uint8_t>(OpCode::RET),
			static_cast<uint8_t>(reg),
			0, 0
		});

		return reg;
	}

	program.push_back({
		static_cast<uint8_t>(OpCode::RET),
		0, 0, 0
	});

	return -1;
}

int VM::compile(Node* node)
{
	if (!node)
		return -1;

	switch (node->type)
	{
		case NodeType::Num:    return compileNum(node);
		case NodeType::Var:    return compileVar(node);
		case NodeType::Op:     return compileOp(node);
		case NodeType::Assign: return compileAssign(node);
		case NodeType::Block:  return compileBlock(node);
		case NodeType::If:     return compileIf(node);
		case NodeType::Comp:   return compileComp(node);
		case NodeType::While:  return compileWhile(node);
		case NodeType::Ret:    return compileReturn(node);
		case NodeType::Func:
			compileFunction(static_cast<FuncNode*>(node));
			return -1;
		case NodeType::Call:
			return compileCall(static_cast<CallNode*>(node));
		default:
			throw std::runtime_error("Unknown node type in compile: " +
			                         std::to_string(static_cast<int>(node->type)));
	}
}

//  visualize()  –  prints the instruction list to stdout

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
				          << ", #" << constants[inst.left];
				break;
			case OpCode::LOAD_VAR:
				std::cout << "MOV r" << static_cast<int>(inst.dest)
				          << ", [" << static_cast<int>(inst.left) << "]";
				break;
			case OpCode::STORE_VAR:
				std::cout << "MOV [" << static_cast<int>(inst.left)
				          << "], r" << static_cast<int>(inst.dest);
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
			case OpCode::CMP:
				std::cout << "CMP r" << static_cast<int>(inst.left)
				          << ", r" << static_cast<int>(inst.right);
				break;
			case OpCode::JMP:
				std::cout << "JMP " << static_cast<int>(inst.dest);
				break;
			case OpCode::JE:
				std::cout << "JE "  << static_cast<int>(inst.dest);
				break;
			case OpCode::JNE:
				std::cout << "JNE " << static_cast<int>(inst.dest);
				break;
			case OpCode::JG:
				std::cout << "JG "  << static_cast<int>(inst.dest);
				break;
			case OpCode::JL:
				std::cout << "JL "  << static_cast<int>(inst.dest);
				break;
			case OpCode::JGE:
				std::cout << "JGE " << static_cast<int>(inst.dest);
				break;
			case OpCode::JLE:
				std::cout << "JLE " << static_cast<int>(inst.dest);
				break;
			case OpCode::CALL:
				std::cout << "CALL " << static_cast<int>(inst.left);
				break;
			case OpCode::RET:
				std::cout << "RET";
				if (inst.dest != 0)
					std::cout << " r" << static_cast<int>(inst.dest);
				break;
		}

		std::cout << "\n";
	}
}


//  writeInExe()  –  writes the instruction list as text to ./exe


void VM::writeInExe()
{
	std::fstream exe;
	exe.open("./exe", std::ios::out | std::ios::trunc);
	if (!exe.is_open())
		throw std::runtime_error("Could not open ./exe for writing");

	// ── Constant pool section ────────────────────────────────────────────────
	// Written first so the VM loader can rebuild the pool before parsing code.
	// Format:  .CONST <count>
	//          <value0>
	//          <value1>  ...
	exe << ".CONST " << constants.size() << "\n";
	for (int c : constants)
		exe << c << "\n";

	// ── Function address table ───────────────────────────────────────────────
	// Format:  .FUNC <name> <address>
	for (const auto& [name, addr] : functions)
		exe << ".FUNC " << name << " " << addr << "\n";

	// ── Code section ────────────────────────────────────────────────────────
	exe << ".CODE\n";

	for (size_t i = 0; i < program.size(); i++)
	{
		const auto& inst = program[i];

		switch (static_cast<OpCode>(inst.op))
		{
			case OpCode::LOAD_NUM:
				exe << "MOV r" << static_cast<int>(inst.dest)
				    << ", #" << constants[inst.left];
				break;

			case OpCode::LOAD_VAR:
				exe << "MOV r" << static_cast<int>(inst.dest)
				    << ", [" << static_cast<int>(inst.left) << "]";
				break;

			case OpCode::STORE_VAR:
				exe << "MOV [" << static_cast<int>(inst.left)
				    << "], r" << static_cast<int>(inst.dest);
				break;

			case OpCode::ADD:
				exe << "ADD r" << static_cast<int>(inst.dest)
				    << ", r" << static_cast<int>(inst.left)
				    << ", r" << static_cast<int>(inst.right);
				break;

			case OpCode::SUB:
				exe << "SUB r" << static_cast<int>(inst.dest)
				    << ", r" << static_cast<int>(inst.left)
				    << ", r" << static_cast<int>(inst.right);
				break;

			case OpCode::MUL:
				exe << "MUL r" << static_cast<int>(inst.dest)
				    << ", r" << static_cast<int>(inst.left)
				    << ", r" << static_cast<int>(inst.right);
				break;

			case OpCode::DIV:
				exe << "DIV r" << static_cast<int>(inst.dest)
				    << ", r" << static_cast<int>(inst.left)
				    << ", r" << static_cast<int>(inst.right);
				break;

			case OpCode::CMP:
				exe << "CMP r" << static_cast<int>(inst.left)
				    << ", r" << static_cast<int>(inst.right);
				break;

			case OpCode::JMP:
				exe << "JMP " << static_cast<int>(inst.dest);
				break;

			case OpCode::JE:  exe << "JE "  << static_cast<int>(inst.dest); break;
			case OpCode::JNE: exe << "JNE " << static_cast<int>(inst.dest); break;
			case OpCode::JG:  exe << "JG "  << static_cast<int>(inst.dest); break;
			case OpCode::JL:  exe << "JL "  << static_cast<int>(inst.dest); break;
			case OpCode::JGE: exe << "JGE " << static_cast<int>(inst.dest); break;
			case OpCode::JLE: exe << "JLE " << static_cast<int>(inst.dest); break;

			case OpCode::CALL:
				exe << "CALL " << static_cast<int>(inst.left);
				break;

			case OpCode::RET:
				exe << "RET";
				if (inst.dest != 0)
					exe << " r" << static_cast<int>(inst.dest);
				break;
		}

		exe << "\n";
	}

	exe.close();
}

// ─────────────────────────────────────────────────────────────────────────────
//  loadFile() / parseLine()  –  read back what writeInExe() produced
//  (kept for completeness; the new RISC-V VM has its own Loader)
// ─────────────────────────────────────────────────────────────────────────────

// void VM::loadFile(const std::string& filename)
// {
// 	std::ifstream file(filename);
// 	if (!file.is_open())
// 		throw std::runtime_error("Cannot open file: " + filename);

// 	std::string line;
// 	bool inCode = false;

// 	while (std::getline(file, line))
// 	{
// 		if (line.empty()) continue;

// 		// Handle section headers written by writeInExe()
// 		if (line.rfind(".CONST", 0) == 0) {
// 			int count = std::stoi(line.substr(7));
// 			for (int i = 0; i < count; i++) {
// 				std::getline(file, line);
// 				constants.push_back(std::stoi(line));
// 			}
// 			continue;
// 		}
// 		if (line.rfind(".FUNC", 0) == 0) {
// 			std::istringstream ss(line.substr(6));
// 			std::string name; int addr;
// 			ss >> name >> addr;
// 			functions[name] = addr;
// 			continue;
// 		}
// 		if (line == ".CODE") { inCode = true; continue; }

// 		if (inCode)
// 			parseLine(line);
// 	}

// 	file.close();
// }

// int VM::regIndex(const std::string& r)
// {
// 	// r is like "r3" – strip the leading 'r'
// 	return std::stoi(r.substr(1));
// }

// Operand VM::defType(const std::string& var) const
// {
// 	if (!var.empty() && var[0] == 'r')
// 		return Operand::REG;
// 	if (!var.empty() && var[0] == '[')
// 		return Operand::MEM;
// 	return Operand::CONST;
// }

// void VM::parseLine(const std::string& line)
// {
// 	std::istringstream ss(line);
// 	std::string op;
// 	ss >> op;

// 	auto stripComma = [](std::string s) -> std::string {
// 		if (!s.empty() && s.back() == ',') s.pop_back();
// 		return s;
// 	};

// 	if (op == "MOV")
// 	{
// 		std::string dst, src;
// 		ss >> dst >> src;
// 		dst = stripComma(dst);

// 		if (defType(dst) == Operand::REG)
// 		{
// 			uint8_t D = static_cast<uint8_t>(regIndex(dst));
// 			if (src[0] == '#')
// 			{
// 				int val = std::stoi(src.substr(1));
// 				uint8_t cIdx = static_cast<uint8_t>(constants.size());
// 				constants.push_back(val);
// 				program.push_back({ static_cast<uint8_t>(OpCode::LOAD_NUM), D, cIdx, 0 });
// 			}
// 			else if (defType(src) == Operand::MEM)
// 			{
// 				std::string addr = src.substr(1, src.size() - 2);
// 				uint8_t A = static_cast<uint8_t>(std::stoi(addr));
// 				program.push_back({ static_cast<uint8_t>(OpCode::LOAD_VAR), D, A, 0 });
// 			}
// 		}
// 		else if (defType(dst) == Operand::MEM)
// 		{
// 			std::string addrStr = dst.substr(1, dst.size() - 2);
// 			uint8_t A = static_cast<uint8_t>(std::stoi(addrStr));
// 			uint8_t S = static_cast<uint8_t>(regIndex(src));
// 			program.push_back({ static_cast<uint8_t>(OpCode::STORE_VAR), S, A, 0 });
// 		}
// 		return;
// 	}

// 	auto parseArith = [&](OpCode aop) {
// 		std::string rD, rL, rR;
// 		ss >> rD >> rL >> rR;
// 		uint8_t D = static_cast<uint8_t>(regIndex(stripComma(rD)));
// 		uint8_t L = static_cast<uint8_t>(regIndex(stripComma(rL)));
// 		uint8_t R = static_cast<uint8_t>(regIndex(stripComma(rR)));
// 		program.push_back({ static_cast<uint8_t>(aop), D, L, R });
// 	};

// 	if      (op == "ADD") { parseArith(OpCode::ADD); return; }
// 	else if (op == "SUB") { parseArith(OpCode::SUB); return; }
// 	else if (op == "MUL") { parseArith(OpCode::MUL); return; }
// 	else if (op == "DIV") { parseArith(OpCode::DIV); return; }

// 	if (op == "CMP")
// 	{
// 		std::string rL, rR;
// 		ss >> rL >> rR;
// 		uint8_t L = static_cast<uint8_t>(regIndex(stripComma(rL)));
// 		uint8_t R = static_cast<uint8_t>(regIndex(rR));
// 		program.push_back({ static_cast<uint8_t>(OpCode::CMP), 0, L, R });
// 		return;
// 	}

// 	auto parseJump = [&](OpCode jop) {
// 		std::string addr;
// 		ss >> addr;
// 		uint8_t target = static_cast<uint8_t>(std::stoi(addr));
// 		program.push_back({ static_cast<uint8_t>(jop), target, 0, 0 });
// 	};

// 	if      (op == "JMP") { parseJump(OpCode::JMP); return; }
// 	else if (op == "JE")  { parseJump(OpCode::JE);  return; }
// 	else if (op == "JNE") { parseJump(OpCode::JNE); return; }
// 	else if (op == "JG")  { parseJump(OpCode::JG);  return; }
// 	else if (op == "JL")  { parseJump(OpCode::JL);  return; }
// 	else if (op == "JGE") { parseJump(OpCode::JGE); return; }
// 	else if (op == "JLE") { parseJump(OpCode::JLE); return; }

// 	if (op == "CALL")
// 	{
// 		std::string addr;
// 		ss >> addr;
// 		uint8_t target = static_cast<uint8_t>(std::stoi(addr));
// 		program.push_back({ static_cast<uint8_t>(OpCode::CALL), 0, target, 0 });
// 		return;
// 	}

// 	if (op == "RET")
// 	{
// 		std::string maybeReg;
// 		uint8_t retReg = 0;
// 		if (ss >> maybeReg)
// 			retReg = static_cast<uint8_t>(regIndex(maybeReg));
// 		program.push_back({ static_cast<uint8_t>(OpCode::RET), retReg, 0, 0 });
// 		return;
// 	}
// }







