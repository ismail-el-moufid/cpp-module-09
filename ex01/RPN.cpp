#include "RPN.hpp"
#include <stack>

RPN::RPN() {}

RPN::RPN(const RPN &other) {(void)other;}

RPN &RPN::operator=(const RPN &other)
{
	(void)other;
	return *this;
}

RPN::~RPN() {}

bool RPN::isOperator(char c) const
{
	return c == '+' || c == '-' || c == '*' || c == '/';
}

int RPN::performOperation(char op, int a, int b) const
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

	for (size_t i = 0; i < expression.length(); ++i)
	{
		char token = expression[i];

		if (std::isspace(token))
			continue;

		if (std::isdigit(token))
		{
			stack.push(token - '0');
		}
		else if (isOperator(token))
		{
			if (stack.size() < 2)
				throw std::runtime_error("Insufficient operands");

			int b = stack.top();
			stack.pop();
			int a = stack.top();
			stack.pop();

			int result = performOperation(token, a, b);
			stack.push(result);
		}
		else
		{
			throw std::runtime_error("Invalid character in expression");
		}
	}

	if (stack.size() != 1)
		throw std::runtime_error("Invalid RPN expression");

	return stack.top();
}
