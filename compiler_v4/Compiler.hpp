#pragma once

#include "../parser_v3/NodeType.hpp"
#include <cstdint>

enum class OpCode : uint8_t
{
	LOAD_NUM,
	LOAD_VAR,
	ADD,
	SUB,
	MUL,
	DIV
};

struct Instruction
{
	uint8_t op;
	uint8_t dest;
	uint8_t left;
	uint8_t right;
};
