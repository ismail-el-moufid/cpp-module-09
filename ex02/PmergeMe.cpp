#include "PmergeMe.hpp"
#include <vector>
#include <deque>
#include <algorithm>
#include <cstdlib>

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

	std::vector<int> mainChain;
	std::vector<int> pendingInsertions;
	size_t i = 0;

	// Step 1: Pair elements
	for (; i + 1 < n; i += 2)
	{
		if (data[i] < data[i + 1])
		{
			mainChain.push_back(data[i + 1]);
			pendingInsertions.push_back(data[i]);
		}
		else
		{
			mainChain.push_back(data[i]);
			pendingInsertions.push_back(data[i + 1]);
		}
	}

	bool hasLeftover = (i < n);

	// Step 2: Handle leftover if odd
	int leftover = (hasLeftover) ? data[i] : 0;

	// Step 3: Recursively sort the main chain
	std::vector<int> sortedMainChain = mergeInsertSort(mainChain);

	// Step 4: Start building the final sorted chain
	std::vector<int> finalChain = sortedMainChain;
	if (!pendingInsertions.empty())
		finalChain.insert(finalChain.begin(), pendingInsertions[0]);

	// Step 5: Insert remaining smaller elements using binaryInsert
	for (size_t j = 1; j < pendingInsertions.size(); ++j)
		binaryInsert(finalChain, pendingInsertions[j]);

	// Step 6: Insert leftover if present
	if (hasLeftover)
		binaryInsert(finalChain, leftover);

	return finalChain;
}

void binaryInsert(std::deque<int> &mainChain, int value)
{
	std::deque<int>::iterator it = std::lower_bound(mainChain.begin(), mainChain.end(), value);
	mainChain.insert(it, value);
}

std::deque<int> PmergeMe::mergeInsertSort(const std::deque<int> &data)
{
	if (data.empty() || data.size() == 1)
		return data;

	std::deque<int> mainChain;
	std::deque<int> pendingInsertions;
	size_t i = 0;

	// Step 1: Pair elements
	for (; i + 1 < data.size(); i += 2)
	{
		if (data[i] < data[i + 1])
		{
			mainChain.push_back(data[i + 1]);
			pendingInsertions.push_back(data[i]);
		}
		else
		{
			mainChain.push_back(data[i]);
			pendingInsertions.push_back(data[i + 1]);
		}
	}
	
	bool hasLeftover = (i < data.size());

	// Step 2: Handle leftover if odd
	int leftover = (hasLeftover) ? data[i] : 0;

	// Step 3: Recursively sort the main chain
	std::deque<int> sortedMainChain = mergeInsertSort(mainChain);

	// Step 4: Start building the final sorted chain
	std::deque<int> finalChain = sortedMainChain;
	if (!pendingInsertions.empty())
		finalChain.push_front(pendingInsertions[0]);

	// Step 5: Insert remaining smaller elements using binaryInsert
	for (size_t j = 1; j < pendingInsertions.size(); ++j)
		binaryInsert(finalChain, pendingInsertions[j]);

	// Step 6: Insert leftover if present
	if (hasLeftover)
		binaryInsert(finalChain, leftover);

	return finalChain;
}
