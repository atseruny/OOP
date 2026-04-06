#pragma once

enum Operator
{
	Add,
	Sub,
	Div,
	Mult,
};

enum NodeType
{
	Num,
	Var,
	EofEx,
	OpBr,
	ClBr,
	Op,
	If,
	While,
	Comp,
	Assign,
	Not,
	Semi,
	OpBody,
	ClBody,
	Decl,
};