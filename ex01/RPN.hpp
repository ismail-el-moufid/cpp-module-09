#pragma once

#include <string>

class RPN
{

private:
	RPN();
	RPN(const RPN &other);
	RPN &operator=(const RPN &other);
	~RPN();
	

public:
	static bool isOperator(char c);
	static int performOperation(char op, int a, int b);
	static int evaluate(const std::string &expression);

};