#include "../includes/CPU.hpp"

CPU::CPU(): IP(0), SP(STACK_BASE), cmpFlag(0)
{
	for (int i = 0; i < NUM_REGS; ++i)
		regs[i] = 0;
	regs[REG_SP] = STACK_BASE;
}

// int32_t CPU::reg(uint8_t r) const
// {
// 	return r == 0 ? 0 : regs[r];
// }

// void CPU::setReg(uint8_t r, int32_t v)
// {
// 	if (r != 0)
// 		regs[r] = v;
// }

// void CPU::push(int32_t val)
// {
// 	mem.stackPush(SP, val);
// }

// int32_t CPU::pop()
// {
// 	return mem.stackPop(SP);
// }

// Instruction CPU::fetch()
// {
// 	return mem.readCode(IP++);
// }

// void CPU::execute(const Instruction& inst)
// {
// 	switch (static_cast<OpCode>(inst.op)) {

// 	case OpCode::LOAD_NUM:
// 		setReg(inst.dest, constPool.at(inst.left));
// 		break;

// 	case OpCode::LOAD_VAR:
// 		setReg(inst.dest, mem.readData(inst.left));
// 		break;

// 	case OpCode::STORE_VAR:
// 		mem.writeData(inst.left, reg(inst.dest));
// 		break;

// 	case OpCode::ADD:
// 		setReg(inst.dest, reg(inst.left) + reg(inst.right));
// 		break;

// 	case OpCode::SUB:
// 		setReg(inst.dest, reg(inst.left) - reg(inst.right));
// 		break;

// 	case OpCode::MUL:
// 		setReg(inst.dest, reg(inst.left) * reg(inst.right));
// 		break;

// 	case OpCode::DIV:
// 		if (reg(inst.right) == 0)
// 			throw std::runtime_error("CPU: division by zero");
// 		setReg(inst.dest, reg(inst.left) / reg(inst.right));
// 		break;

// 	case OpCode::CMP:
// 		cmpFlag = reg(inst.left) - reg(inst.right);
// 		break;

// 	case OpCode::JMP:
// 		IP = inst.dest;
// 		break;

// 	case OpCode::JE:
// 		if (cmpFlag == 0) IP = inst.dest;
// 		break;

// 	case OpCode::JNE:
// 		if (cmpFlag != 0) IP = inst.dest;
// 		break;

// 	case OpCode::JG:
// 		if (cmpFlag > 0) IP = inst.dest;
// 		break;

// 	case OpCode::JL:
// 		if (cmpFlag < 0) IP = inst.dest;
// 		break;

// 	case OpCode::JGE:
// 		if (cmpFlag >= 0) IP = inst.dest;
// 		break;

// 	case OpCode::JLE:
// 		if (cmpFlag <= 0) IP = inst.dest;
// 		break;

// 	case OpCode::CALL:
// 		push(IP);                  // save return address (IP already past CALL)
// 		push(reg(REG_FP));         // save frame pointer
// 		setReg(REG_FP, SP);        // new frame pointer = current SP
// 		setReg(REG_RA, IP);        // also keep in ra (RISC-V convention)
// 		IP = inst.dest;            // jump to function
// 		break;

// 	case OpCode::RET:
// 		SP = static_cast<uint16_t>(reg(REG_FP));  // restore SP to frame base
// 		setReg(REG_FP, pop());     // restore caller's frame pointer
// 		IP  = static_cast<uint16_t>(pop());        // restore return address
// 		break;

// 	}
// }

// void CPU::run()
// {
// 	while (!halted) {
// 		Instruction inst = fetch();
// 		execute(inst);
// 	}
// }

// void CPU::dumpRegisters() const
// {
// 	std::cout << "\n[Register file]\n";
// 	const char* names[] = {
// 		"zero","ra","sp","fp",
// 		"t0","t1","t2","t3","t4","t5","t6","t7",
// 		"s0","s1","s2","s3","s4","s5","s6","s7",
// 		"a0","a1","a2","a3","a4","a5","a6","a7",
// 		"x28","x29","x30","x31"
// 	};
// 	for (int i = 0; i < NUM_REGS; ++i) {
// 		int32_t v = (i == 0) ? 0 : regs[i];
// 		if (v != 0)
// 			std::cout << "  x" << std::setw(2) << std::left << i
// 						<< " (" << std::setw(4) << names[i] << ") = "
// 						<< v << "\n";
// 	}
// 	std::cout << "  IP = " << IP
// 				<< "   SP = 0x" << std::hex << SP << std::dec
// 				<< "   cmpFlag = " << cmpFlag << "\n";
// }
