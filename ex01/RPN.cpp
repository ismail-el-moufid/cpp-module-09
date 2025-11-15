#include "RPN.hpp"
#include <stack>
#include <cctype>
#include <sstream>
#include <stdexcept>

RPN::RPN() {}

RPN::RPN(const RPN &other) {(void)other;}

RPN &RPN::operator=(const RPN &other)
{
	(void)other;
	return *this;
}

RPN::~RPN() {}

bool RPN::isOperator(char c)
{
	return c == '+' || c == '-' || c == '*' || c == '/';
}

int RPN::performOperation(char op, int a, int b)
{
	switch (op)
	{
	case '+':
		return a + b;
	case '-':
		return a - b;
	case '*':
		return a * b;
	case '/':
		if (b == 0)
			throw std::runtime_error("Division by zero");
		return a / b;
	default:
		throw std::runtime_error("Invalid operator");
	}
}

int RPN::evaluate(const std::string &expression)
{
	std::stack<int> stack;
	std::istringstream iss(expression);
	std::string token;

	while (iss >> token)
	{
		if (token.size() == 1 && std::isdigit(static_cast<unsigned char>(token[0])))
		{
			stack.push(token[0] - '0');
			continue;
		}

		if (token.size() == 1 && isOperator(token[0]))
		{
			if (stack.size() < 2)
				throw std::runtime_error("Insufficient operands");

			int b = stack.top(); stack.pop();
			int a = stack.top(); stack.pop();

			int result = performOperation(token[0], a, b);
			stack.push(result);
		}
		else
			throw std::runtime_error("Invalid token in expression");
	}

	if (stack.size() != 1)
		throw std::runtime_error("Invalid RPN expression");

	return stack.top();
}