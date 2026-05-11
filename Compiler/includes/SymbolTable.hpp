#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

class SymbolTable {
	std::vector<std::unordered_map<std::string, size_t>> scopes;
	std::vector<int> memory;

public:
	SymbolTable();
	~SymbolTable();
	size_t getAddress(const std::string&);
	void setVariable(const std::string&, int);
	int getValueByAddress(size_t) const;
	bool isDeclared(const std::string&) const;
	void declareVariable(const std::string&);
	void setVariableByAddress(size_t address, int value);

	void enterScope();
	void exitScope();
};