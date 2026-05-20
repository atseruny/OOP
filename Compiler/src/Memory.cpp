#include "../includes/Memory.hpp"

Memory::Memory() : cells(MEM_SIZE + 1, 0) {}

int32_t Memory::read(uint16_t addr) const {
	return cells[addr];
}

void Memory::write(uint16_t addr, int32_t val) {
	if (addr == 0)
		return;
	cells[addr] = val;
}

void Memory::writeCode(uint16_t codeAddr, const Instruction& inst) {
	int32_t packed =
		(static_cast<uint8_t>(inst.op)) |
		(static_cast<uint32_t>(inst.dest) << 8) |
		(static_cast<uint32_t>(inst.left) << 16) |
		(static_cast<uint32_t>(inst.right) << 24);
	write(CODE_BASE + codeAddr, packed);
}

Instruction Memory::readCode(uint16_t codeAddr) const {
	uint32_t packed = static_cast<uint32_t>(read(CODE_BASE + codeAddr));

	Instruction inst;

	inst.op = (packed & 0xFF);
	inst.dest = (packed >> 8)  & 0xFF;
	inst.left = (packed >> 16) & 0xFF;
	inst.right = (packed >> 24) & 0xFF;
	return inst;
}

// Data section helpers
int32_t Memory::readData(uint8_t varAddr) const {
	return read(DATA_BASE + varAddr);
}
void Memory::writeData(uint8_t varAddr, int32_t val) {
	write(DATA_BASE + varAddr, val);
}

// Stack helpers
void Memory::stackPush(uint16_t& sp, int32_t val) {
	if (sp < STACK_LIMIT)
		throw std::runtime_error("Stack overflow");
	write(sp--, val);
}
int32_t Memory::stackPop(uint16_t& sp) {
	if (sp >= STACK_BASE)
		throw std::runtime_error("Stack underflow");
	return read(++sp);
}

void Memory::dump(uint16_t from, uint16_t to) const {
	std::cout << "\n[Memory dump 0x" << std::hex << from << " - 0x" << to << std::dec << "]\n";
	for (uint16_t a = from; a <= to; ++a)
	{
		if (cells[a] != 0)
			std::cout << "  [0x" << std::hex << std::setw(4) << std::setfill('0')
				<< a << "] = " << std::dec << cells[a] << "\n";
	}
}