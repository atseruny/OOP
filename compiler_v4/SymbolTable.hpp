#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

class SymbolTable {
	std::unordered_map<std::string, size_t> nameToIndex;
	std::vector<double> memory;
public:
	SymbolTable();
	~SymbolTable();
	size_t getAddress(const std::string&);
	void setVariable(const std::string&, double);
	double getValueByAddress(size_t) const;
};