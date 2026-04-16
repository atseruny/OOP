#pragma once

#include "../parser_v3/NodeType.hpp"
#include <cstdint>

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
