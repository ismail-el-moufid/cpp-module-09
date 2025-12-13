#include "PmergeMe.hpp"
#include <vector>
#include <deque>
#include <cstdlib>
#include <limits>
#include <iostream>

#define ENABLE_LOGS 1

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& other) { (void)other; }

PmergeMe& PmergeMe::operator=(const PmergeMe& other) 
{
	(void)other;
	return *this;
}

PmergeMe::~PmergeMe() {}

static std::vector<size_t> jacobsthalOrder(size_t count)
{
	std::vector<size_t> order;
	if (count == 0)
		return order;

	std::vector<size_t> jacobsthalNumbers;
	size_t prev = 0;
	size_t curr = 1;

	while (curr < count)
	{
		jacobsthalNumbers.push_back(curr);
		if (curr > (std::numeric_limits<size_t>::max() - prev) / 2)
			break;
		size_t next = curr + 2 * prev;
		if (next >= count)
			break;
		prev = curr;
		curr = next;
	}

	std::vector<char> inserted(count, 0);
	if (count > 0)
	{
		order.push_back(0);
		inserted[0] = 1;
	}

	for (size_t i = 0; i < jacobsthalNumbers.size(); ++i)
	{
		size_t currentJacobsthal = jacobsthalNumbers[i];
		size_t previousJacobsthal = (i > 0) ? jacobsthalNumbers[i - 1] : 0;
		
		for (size_t idx = currentJacobsthal; idx > previousJacobsthal; --idx)
		{
			if (idx < count && !inserted[idx])
			{
				order.push_back(idx);
				inserted[idx] = 1;
			}
		}
	}

	for (size_t i = 0; i < count; ++i)
		if (!inserted[i])
			order.push_back(i);

	return order;
}

void binaryInsert(std::vector<int> &result, int value, size_t &comparisons)
{
	size_t low = 0;
	size_t high = result.size() > 0 ? result.size() - 1 : 0;

	if (result.empty())
	{
		result.push_back(value);
		return;
	}

	while (low <= high)
	{
		size_t mid = low + (high - low) / 2;
		comparisons++;
		if (value <= result[mid])
		{
			if (mid == 0)
				break;
			high = mid - 1;
		}
		else
			low = mid + 1;
	}
	result.insert(result.begin() + low, value);
}

void binaryInsert(std::deque<int> &result, int value, size_t &comparisons)
{
	size_t low = 0;
	size_t high = result.size() > 0 ? result.size() - 1 : 0;

	if (result.empty())
	{
		result.push_back(value);
		return;
	}

	while (low <= high)
	{
		size_t mid = low + (high - low) / 2;
		comparisons++;
		if (value <= result[mid])
		{
			if (mid == 0)
				break;
			high = mid - 1;
		}
		else
			low = mid + 1;
	}
	result.insert(result.begin() + low, value);
}

std::vector<int> PmergeMe::mergeInsertSort(const std::vector<int> &data)
{
	static size_t originalSize = 0;
	static bool isTopLevel = true;
	static size_t comparisons = 0;
	
	if (isTopLevel)
	{
		originalSize = data.size();
		isTopLevel = false;
		comparisons = 0;
	}
	
	size_t size = data.size();
	if (size <= 1)
	{
		if (size == originalSize)
		{
			std::cout << "Number of comparisons (vector): " << comparisons << std::endl;
			isTopLevel = true;
		}
		return data;
	}

	std::vector<std::pair<int, int> > pairs;

	size_t index = 0;
	for (; index + 1 < size; index += 2)
	{
		int first = data[index];
		int second = data[index + 1];
		if (first > second)
			pairs.push_back(std::make_pair(first, second));
		else
			pairs.push_back(std::make_pair(second, first));
	}

	bool hasStraggler = (index < size);
	int straggler = hasStraggler ? data[index] : 0;

	std::vector<int> largerElements;
	for (size_t i = 0; i < pairs.size(); ++i)
		largerElements.push_back(pairs[i].first);

	std::vector<int> sortedLarger = mergeInsertSort(largerElements);

	std::vector<int> smallerElements;
	for (size_t i = 0; i < sortedLarger.size(); ++i)
	{
		for (size_t j = 0; j < pairs.size(); ++j)
		{
			if (pairs[j].first == sortedLarger[i])
			{
				smallerElements.push_back(pairs[j].second);
				pairs[j].first = -1;
				break;
			}
		}
	}

	std::vector<int> result;

	if (!smallerElements.empty())
		result.push_back(smallerElements[0]);
	result.insert(result.end(), sortedLarger.begin(), sortedLarger.end());

	std::vector<size_t> insertionOrder = jacobsthalOrder(smallerElements.size());
	
#if ENABLE_LOGS
	if (size == originalSize && originalSize > 2)
	{
		for (size_t i = 0; i < smallerElements.size(); ++i)
		{
			std::cout << "  smallerElements[" << i << "] = " << smallerElements[i];
			for (size_t j = 0; j < insertionOrder.size(); ++j)
			{
				if (insertionOrder[j] == i)
				{
					std::cout << "  <-- insert order " << j;
					break;
				}
			}
			std::cout << std::endl;
		}
	}
#endif
	
	for (size_t i = 0; i < insertionOrder.size(); ++i)
	{
		size_t idx = insertionOrder[i];
		if (idx > 0 && idx < smallerElements.size())
			binaryInsert(result, smallerElements[idx], comparisons);
	}

	if (hasStraggler)
		binaryInsert(result, straggler, comparisons);

	if (size == originalSize)
	{
		std::cout << "Number of comparisons (vector): " << comparisons << std::endl;
		isTopLevel = true;
	}

	return result;
}

std::deque<int> PmergeMe::mergeInsertSort(const std::deque<int> &data)
{
	static size_t originalSize = 0;
	static bool isTopLevel = true;
	static size_t comparisons = 0;
	
	if (isTopLevel)
	{
		originalSize = data.size();
		isTopLevel = false;
		comparisons = 0;
	}
	
	size_t size = data.size();
	if (size <= 1)
	{
		if (size == originalSize)
		{
			std::cout << "Number of comparisons (deque): " << comparisons << std::endl;
			isTopLevel = true;
		}
		return data;
	}

	std::vector<std::pair<int, int> > pairs;

	size_t index = 0;
	for (; index + 1 < size; index += 2)
	{
		int first = data[index];
		int second = data[index + 1];
		if (first > second)
			pairs.push_back(std::make_pair(first, second));
		else
			pairs.push_back(std::make_pair(second, first));
	}

	bool hasStraggler = (index < size);
	int straggler = hasStraggler ? data[index] : 0;

	std::deque<int> largerElements;
	for (size_t i = 0; i < pairs.size(); ++i)
		largerElements.push_back(pairs[i].first);

	std::deque<int> sortedLarger = mergeInsertSort(largerElements);

	std::deque<int> smallerElements;
	for (size_t i = 0; i < sortedLarger.size(); ++i)
	{
		for (size_t j = 0; j < pairs.size(); ++j)
		{
			if (pairs[j].first == sortedLarger[i])
			{
				smallerElements.push_back(pairs[j].second);
				pairs[j].first = -1;
				break;
			}
		}
	}

	std::deque<int> result;

	if (!smallerElements.empty())
		result.push_back(smallerElements[0]);
	result.insert(result.end(), sortedLarger.begin(), sortedLarger.end());

	std::vector<size_t> insertionOrder = jacobsthalOrder(smallerElements.size());
	
	for (size_t i = 0; i < insertionOrder.size(); ++i)
	{
		size_t idx = insertionOrder[i];
		if (idx > 0 && idx < smallerElements.size())
			binaryInsert(result, smallerElements[idx], comparisons);
	}

	if (hasStraggler)
		binaryInsert(result, straggler, comparisons);

	if (size == originalSize)
	{
		std::cout << "Number of comparisons (deque): " << comparisons << std::endl;
		isTopLevel = true;
	}

	return result;
}
