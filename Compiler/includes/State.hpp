#pragma once

enum class State
{
	Start,
	Wait_operator,
	Wait_operand,
	Error,
	End
};