#pragma once

#include "IfNode.hpp"
#include "WhileNode.hpp"
#include "BlockNode.hpp"
#include <fstream>
#include "VM.hpp"
#include "SymbolTable.hpp"
#include "../parser_v3/Token.hpp"
#include "../parser_v3/State.hpp"


std::unique_ptr<Node> parser(std::vector<Token>& tokens, SymbolTable& ST, int pos);
std::vector<std::string> lexer(std::stringstream& line);