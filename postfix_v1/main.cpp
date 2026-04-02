#include "Postfix.hpp"

int main() {
	std::string expr;
	std::string res;
	
	std::cout << "Enter expression: ";
	std::cin >> expr;
	
	res = toPostfix(expr);
	std::cout << "Result: " << eval(res) << "\n";
	
	return 0;
}