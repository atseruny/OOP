#include "Token.hpp"

bool isnum(const std::string &word)
{
	if (word.empty())
		return false;
	for (size_t i = 0; i < word.length(); i++)
	{
		if (!std::isdigit(word[i]))
			return false;
	}
	return true;
}

bool isvar(const std::string &word)
{
	if (word.empty())
		return false;
	for (size_t i = 0; i < word.length(); i++)
	{
		if (!std::isalnum(word[i]))
			return false;
	}
	if (!std::isalpha(word[0]))
		return false;
	return true;
}

std::vector<Token> tokenizer(const std::vector<std::string> &words)
{
	std::vector<Token> tokens;
	for (size_t i = 0; i < words.size(); i++)
	{
		if (isnum(words[i]))
			tokens.push_back(Token(words[i], NodeType::Num));
		else if (isvar(words[i]))
			tokens.push_back(Token(words[i], NodeType::Var));
		else if (words[i] == "(")
			tokens.push_back(Token(words[i], NodeType::OpBr));
		else if (words[i] == ")")
			tokens.push_back(Token(words[i], NodeType::ClBr));
		else if (words[i] == "+" || words[i] == "-" || words[i] == "*" || words[i] == "/")
			tokens.push_back(Token(words[i], NodeType::Op));
		else
			throw std::runtime_error("unexpected token");
	}
	return tokens;
}