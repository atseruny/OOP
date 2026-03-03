#include "Node.hpp"

void readNum(std::stringstream& line, std::vector<std::string>& words)
{
	std::string s;
	while (line.peek() != EOF && std::isdigit(line.peek()))
	{
		s += line.get();
	}
	words.push_back(s);
}

void readVar(std::stringstream& line, std::vector<std::string>& words)
{
	std::string s;
	while (line.peek() != EOF && std::isalnum(line.peek()))
	{
		s += line.get();
	}
	words.push_back(s);
}

bool isoperator(const char c)
{
	return c == '+' || c == '-' || c == '*' || c == '/';
}

std::vector<std::string> lexer(std::stringstream& line)
{
	char ch;
	std::vector<std::string> words;
	std::string s;
	while (line.get(ch))
	{
		if (std::isspace(ch))
			continue;
		else if (std::isdigit(ch))
		{
			line.putback(ch);
			readNum(line, words);
		}
		else if (std::isalpha(ch))
		{
			line.putback(ch);
			readVar(line, words);
		}
		else if (isoperator(ch) || ch == '(' || ch == ')')
			words.push_back(std::string(1, ch));
		else
			throw std::runtime_error("unexpected input");
	}
	return words;
}