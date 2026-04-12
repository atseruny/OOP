#include "SymbolTable.hpp"

SymbolTable::SymbolTable() { }

SymbolTable::~SymbolTable() { }

size_t SymbolTable::getAddress(const std::string& name) {
	auto it = nameToIndex.find(name);
		if (it != nameToIndex.end()) {
			return it->second;
		}
		size_t newAddress = memory.size();
		nameToIndex[name] = newAddress;
		memory.push_back(0.0);
		return newAddress;
}

double SymbolTable::getValueByAddress(size_t address) const {
	if(address < memory.size())
		return memory[address];
	throw std::runtime_error("Invalid address");
}

void SymbolTable::setVariable(const std::string& name, double value) {
	size_t addr = getAddress(name);
	memory[addr] = value;
}

bool SymbolTable::isDeclared(const std::string& name) const
{
	return nameToIndex.find(name) != nameToIndex.end();
}