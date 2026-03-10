#pragma once

enum State
{
	Start,
	Wait_operator,
	Wait_operand,
	Error,
	End
};