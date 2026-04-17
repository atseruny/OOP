#pragma once

#include "../parser_v3/NodeType.hpp"
#include <cstdint>
#include "IfNode.hpp"
#include "WhileNode.hpp"
#include "BlockNode.hpp"
#include <fstream>
#include "VM.hpp"
#include "SymbolTable.hpp"
#include "../parser_v3/Token.hpp"
#include "../parser_v3/State.hpp"
#include <iomanip>

std::vector<std::string> lexer(std::stringstream& line);
std::unique_ptr<Node> parser(std::vector<Token>& tokens, SymbolTable& ST, int& pos);
std::unique_ptr<Node> parseBlock(std::vector<Token>& tokens, SymbolTable& ST, int& pos);
std::unique_ptr<Node> parseStatement(std::vector<Token>& tokens, SymbolTable& ST, int& pos);


inline State FSM[3][6] =
{
	// Start
	{ State::Wait_operator, State::Wait_operator, State::Error, State::Wait_operand, State::Error, State::Error },

	// Wait_operator
	{ State::Error, State::Error, State::End, State::Error, State::Wait_operator, State::Wait_operand },

	// Wait_operand
	{ State::Wait_operator, State::Wait_operator, State::Error, State::Wait_operand, State::Error, State::Error }
};

enum class OpCode : uint8_t
{
	LOAD_NUM,
	LOAD_VAR,
	STORE_VAR,

	ADD,
	SUB,
	MUL,
	DIV,

	JMP,
	CMP,
	JE,
	JNE,
	JG,
	JL,
	JGE,
	JLE
};

struct Instruction
{
	uint8_t op;
	uint8_t dest;
	uint8_t left;
	uint8_t right;
};
