#include "PmergeMe.hpp"
#include <iostream>

bool isNumber(const std::string &s)
{
	if (s.empty())
		return false;
	for (size_t i = 0; i < s.size(); ++i)
		if (!std::isdigit(s[i]))
			return false;
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
		return 0;

	std::vector<int> numbers;
	for (int i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);
		if (!isNumber(arg))
		{
			std::cerr << "Error" << std::endl;
			return 1;
		}
		numbers.push_back(std::atoi(argv[i]));
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

}
