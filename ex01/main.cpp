#include "RPN.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << (argc > 0 ? argv[0] : "./RPN") << " \"<RPN expression>\"" << std::endl;
		return 1;
	}

	std::string expression = argv[1];
	RPN rpn;

	try
	{
		int result = rpn.evaluate(expression);
		std::cout << result << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error" << std::endl;
		return 1;
	}

	return 0;
}