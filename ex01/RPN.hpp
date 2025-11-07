#pragma once

#include <string>

class RPN
{

private:
	bool isOperator(char c) const;
	int performOperation(char op, int a, int b) const;

public:
	RPN();
	RPN(const RPN &other);
	RPN(std::string expression);
	RPN &operator=(const RPN &other);
	~RPN();

	int evaluate(const std::string &expression);

};