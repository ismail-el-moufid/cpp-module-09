#include "PmergeMe.hpp"
#include <vector>
#include <deque>
#include <algorithm>
#include <cstdlib>
#include <limits>

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& other) { (void)other; }

PmergeMe& PmergeMe::operator=(const PmergeMe& other) 
{
	(void)other;
	return *this;
}

PmergeMe::~PmergeMe() {}

static std::vector<size_t> jacobsthalOrder(size_t m)
{
	std::vector<size_t> order;
	if (m == 0)
		return order;

	std::vector<size_t> js;
	size_t J0 = 0;
	size_t J1 = 1;
	
	while (J1 < m)
	{
		js.push_back(J1);
		if (J1 > (std::numeric_limits<size_t>::max() - J0) / 2)
			break;
		size_t Jn = J1 + 2 * J0;
		if (Jn >= m)
			break;
		J0 = J1;
		J1 = Jn;
	}

	std::vector<char> used(m, 0);
	if (m > 0)
	{
		order.push_back(0);
		used[0] = 1;
	}

	for (size_t i = 0; i < js.size(); ++i)
	{
		size_t jval = js[i];
		size_t prev = (i > 0) ? js[i - 1] : 0;
		
		for (size_t idx = jval; idx > prev; --idx)
		{
			if (idx < m && !used[idx])
			{
				order.push_back(idx);
				used[idx] = 1;
			}
		}
	}

	for (size_t i = 0; i < m; ++i)
		if (!used[i])
			order.push_back(i);

	return order;
}

void binaryInsert(std::vector<int> &mainChain, int value)
{
	std::vector<int>::iterator it = std::lower_bound(mainChain.begin(), mainChain.end(), value);
	mainChain.insert(it, value);
}

std::vector<int> PmergeMe::mergeInsertSort(const std::vector<int> &data)
{
	size_t n = data.size();
	if (n <= 1)
		return data;

	std::vector<int> larger;
	std::vector<int> smaller;
	larger.reserve(n / 2 + 1);
	smaller.reserve(n / 2 + 1);

	size_t i = 0;
	for (; i + 1 < n; i += 2)
	{
		int a = data[i];
		int b = data[i + 1];
		if (a > b)
		{
			larger.push_back(a);
			smaller.push_back(b);
		}
		else
		{
			larger.push_back(b);
			smaller.push_back(a);
		}
	}

	bool hasLeftover = (i < n);
	int leftover = hasLeftover ? data[i] : 0;

	std::vector<int> sortedLarger = mergeInsertSort(larger);
	
	std::vector<int> result;
	result.reserve(n);
	
	if (!smaller.empty())
		result.push_back(smaller[0]);
	
	result.insert(result.end(), sortedLarger.begin(), sortedLarger.end());

	std::vector<size_t> order = jacobsthalOrder(smaller.size());
	for (size_t j = 1; j < order.size(); ++j)
	{
		size_t idx = order[j];
		if (idx < smaller.size())
			binaryInsert(result, smaller[idx]);
	}

	if (hasLeftover)
		binaryInsert(result, leftover);

	return result;
}

void binaryInsert(std::deque<int> &mainChain, int value)
{
	std::deque<int>::iterator it = std::lower_bound(mainChain.begin(), mainChain.end(), value);
	mainChain.insert(it, value);
}

std::deque<int> PmergeMe::mergeInsertSort(const std::deque<int> &data)
{
	size_t n = data.size();
	if (n <= 1)
		return data;

	std::deque<int> larger;
	std::deque<int> smaller;

	size_t i = 0;
	for (; i + 1 < n; i += 2)
	{
		int a = data[i];
		int b = data[i + 1];
		if (a > b)
		{
			larger.push_back(a);
			smaller.push_back(b);
		}
		else
		{
			larger.push_back(b);
			smaller.push_back(a);
		}
	}
	
	bool hasLeftover = (i < n);
	int leftover = hasLeftover ? data[i] : 0;

	std::deque<int> sortedLarger = mergeInsertSort(larger);
	
	std::deque<int> result;
	
	if (!smaller.empty())
		result.push_back(smaller[0]);
	
	result.insert(result.end(), sortedLarger.begin(), sortedLarger.end());

	std::vector<size_t> order = jacobsthalOrder(smaller.size());
	for (size_t j = 1; j < order.size(); ++j)
	{
		size_t idx = order[j];
		if (idx < smaller.size())
			binaryInsert(result, smaller[idx]);
	}

	if (hasLeftover)
		binaryInsert(result, leftover);

	return result;
}
