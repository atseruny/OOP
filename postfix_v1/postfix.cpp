#include "Postfix.hpp"

int priority(char ch) {
	if (ch == '+' || ch == '-')
		return 1;
	if (ch == '*' || ch == '/')
		return 2;
	return 0;
}

int eval(std::string& expr)
{
	std::stack<int> st;

	for (size_t i = 0; i < expr.size(); ++i) {
		char ch = expr[i];

		if (std::isdigit(ch)) {
			st.push(ch - '0');
		} else {
			int b = st.top(); st.pop();
			int a = st.top(); st.pop();

			int res = 0;

			if (ch == '+') res = a + b;
			else if (ch == '-') res = a - b;
			else if (ch == '*') res = a * b;
			else if (ch == '/') res = a / b;

			st.push(res);
		}
	}

	return st.top();
}

std::string toPostfix(const std::string& in)
{
	std::stack<char> st;
	std::string res;
	for (size_t i = 0; i < in.size(); ++i) {
		char ch = in[i];

		if (std::isdigit(ch)) {
			res += ch;
		}
		else if (ch == '(') {
			st.push(ch);
		}
		else if (ch == ')') {
			while (!st.empty() && st.top() != '(') {
				res += st.top();
				st.pop();
			}
			if (!st.empty()) st.pop(); // remove '('
		}
		else {
			while (!st.empty() && st.top() != '(' &&
				priority(st.top()) >= priority(ch)) {
				res += st.top();
				st.pop();
			}
			st.push(ch);
		}
	}

	while (!st.empty()) {
		res += st.top();
		st.pop();
	}
	return res;
}