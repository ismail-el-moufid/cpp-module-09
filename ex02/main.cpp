#include "PmergeMe.hpp"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <limits>

bool isNumber(const std::string &s)
{
	if (s.empty())
		return false;
	size_t i = 0;
	if (s[0] == '+')
	{
		if (s.size() == 1)
			return false;
		i = 1;
	}
	for (; i < s.size(); ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(s[i])))
			return false;
	}
	return true;
}

template <typename T>
void printContainer(const T &cont)
{
	for (size_t i = 0; i < cont.size(); ++i)
	{
		std::cout << cont[i];
		if (i + 1 < cont.size())
			std::cout << " ";
	}
	std::cout << std::endl;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Error: no input provided" << std::endl;
		return 1;
	}

	std::vector<int> numbers;
	for (int i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);
		if (!isNumber(arg))
		{
			std::cerr << "Error: invalid input: " << arg << std::endl;
			return 1;
		}

		errno = 0;
		char *end = NULL;
		unsigned long val = std::strtoul(argv[i], &end, 10);
		if (errno == ERANGE || end == argv[i] || *end != '\0' ||
			val > static_cast<unsigned long>(std::numeric_limits<int>::max()))
		{
			std::cerr << "Error: number out of range: " << arg << std::endl;
			return 1;
		}
		numbers.push_back(static_cast<int>(val));
	}

	std::vector<int> numbersVec(numbers.begin(), numbers.end());
	std::deque<int> numbersDeque(numbers.begin(), numbers.end());

	std::cout << "Before: ";
	printContainer(numbersVec);

	std::clock_t startVec = std::clock();
	std::vector<int> sortedVec = PmergeMe::mergeInsertSort(numbersVec);
	std::clock_t endVec = std::clock();

	std::cout << "After: ";
	printContainer(sortedVec);

	double durationVec = 1000000.0 * (double)(endVec - startVec) / CLOCKS_PER_SEC;
	std::cout << "Time to process a range of " << numbers.size() << " elements with std::vector: " << durationVec << " us" << std::endl;

	std::clock_t startDeque = std::clock();
	std::deque<int> sortedDeque = PmergeMe::mergeInsertSort(numbersDeque);
	std::clock_t endDeque = std::clock();

	double durationDeque = 1000000.0 * (double)(endDeque - startDeque) / CLOCKS_PER_SEC;
	std::cout << "Time to process a range of " << numbers.size() << " elements with std::deque: " << durationDeque << " us" << std::endl;

	return 0;
}
