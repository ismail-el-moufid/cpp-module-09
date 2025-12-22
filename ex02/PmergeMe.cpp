#include "PmergeMe.hpp"
#include <vector>
#include <deque>
#include <algorithm>
#include <cstddef>

#include <iostream>

// =======================================================
// Pair structure
// =======================================================
struct Pair
{
	int winner;
	int loser;
	Pair(int winnerValue = 0, int loserValue = 0) : winner(winnerValue), loser(loserValue) {}
};

// =======================================================
// Jacobsthal order
// =======================================================
static std::vector<std::size_t> jacobsthalOrder(std::size_t size)
{
	std::vector<std::size_t> insertionOrder;

	if (size == 0)
		return insertionOrder;

	// Generate Jacobsthal numbers
	std::vector<std::size_t> jacobsthalNumbers;
	jacobsthalNumbers.push_back(0);
	jacobsthalNumbers.push_back(1);
	while (jacobsthalNumbers.back() < size)
	{
		std::size_t nextJacobsthal = jacobsthalNumbers.back() + 2 * jacobsthalNumbers[jacobsthalNumbers.size() - 2];
		jacobsthalNumbers.push_back(nextJacobsthal);
	}

	// Build insertion order following Jacobsthal sequence
	std::vector<bool> isIndexAdded(size, false);
	for (std::size_t jacobIndex = 2; jacobIndex < jacobsthalNumbers.size(); ++jacobIndex)
	{
		std::size_t rangeStart = std::min(jacobsthalNumbers[jacobIndex], size);
		std::size_t rangeEnd = jacobsthalNumbers[jacobIndex - 1];
		for (std::size_t currentIndex = rangeStart; currentIndex > rangeEnd; --currentIndex)
		{
			if (currentIndex - 1 < size && !isIndexAdded[currentIndex - 1])
			{
				insertionOrder.push_back(currentIndex - 1);
				isIndexAdded[currentIndex - 1] = true;
			}
		}
	}

	// Add any remaining indices not covered by Jacobsthal sequence
	for (std::size_t index = 0; index < size; ++index)
		if (!isIndexAdded[index])
			insertionOrder.push_back(index);

	return insertionOrder;
}

// =======================================================
// Ford-Johnson merge-insert sort (recursive)
// =======================================================
std::vector<int> PmergeMe::mergeInsertSort(const std::vector<int>& data)
{
	const std::size_t dataSize = data.size();
	if (dataSize <= 1)
		return data;

	if (dataSize == 2)
	{
		if (data[0] <= data[1])
			return data;
		else
		{
			std::vector<int> tmp;
			tmp.push_back(data[1]);
			tmp.push_back(data[0]);
			return tmp;
		}
	}

	// Create pairs
	std::vector<Pair> pairs;
	pairs.reserve((dataSize + 1) / 2);

	for (std::size_t index = 0; index + 1 < dataSize; index += 2)
	{
		if (data[index] > data[index + 1])
			pairs.push_back(Pair(data[index], data[index + 1]));
		else
			pairs.push_back(Pair(data[index + 1], data[index]));
	}

	// Track leftover element if data size is odd
	int leftover;
	bool hasLeftover = false;
	if (dataSize % 2 == 1)
	{
		leftover = data[dataSize - 1];
		hasLeftover = true;
	}

	// Extract winners for recursive sorting
	std::vector<int> winners;
	winners.reserve(pairs.size());
	for (std::size_t index = 0; index < pairs.size(); ++index)
		winners.push_back(pairs[index].winner);

	// RECURSIVE CALL: Sort the winners
	std::vector<int> sortedWinners = mergeInsertSort(winners);

	// Create a mapping from sorted winners back to their original pairs
	std::vector<Pair> sortedPairs;
	sortedPairs.reserve(pairs.size());
	std::vector<bool> pairUsed(pairs.size(), false);
	
	for (std::size_t index = 0; index < sortedWinners.size(); ++index)
	{
		// Find which pair this winner came from
		for (std::size_t pairIndex = 0; pairIndex < pairs.size(); ++pairIndex)
		{
			if (!pairUsed[pairIndex] && pairs[pairIndex].winner == sortedWinners[index])
			{
				sortedPairs.push_back(pairs[pairIndex]);
				pairUsed[pairIndex] = true;
				break;
			}
		}
	}

	// Build sorted chain using Ford-Johnson insertion
	std::vector<int> sortedChain;
	sortedChain.reserve(dataSize);

	// Insert first loser (smallest among winners)
	if (!sortedPairs.empty())
		sortedChain.push_back(sortedPairs[0].loser);

	// Insert all winners
	for (std::size_t index = 0; index < sortedPairs.size(); ++index)
		sortedChain.push_back(sortedPairs[index].winner);

	// Track current position of each winner in sortedChain
	std::vector<std::size_t> winnerPositions(sortedPairs.size());
	for (std::size_t i = 0; i < sortedPairs.size(); ++i)
		winnerPositions[i] = i + 1; // Initial position: 1 for first loser + i

	// Insert remaining losers using Jacobsthal order with bounded binary search
	// Determine how many losers need to be inserted (excluding first loser already inserted)
	std::size_t numLosersToInsert = sortedPairs.size() - 1;
	if (hasLeftover)
		numLosersToInsert++; // Include leftover in the count

	std::vector<std::size_t> insertionOrder = jacobsthalOrder(numLosersToInsert);

	for (std::size_t orderIndex = 0; orderIndex < insertionOrder.size(); ++orderIndex)
	{
		int valueToInsert;
		std::vector<int>::iterator searchEnd;

		// Check if this is the leftover element
		if (insertionOrder[orderIndex] == sortedPairs.size() - 1 && hasLeftover)
		{
			valueToInsert = leftover;
			searchEnd = sortedChain.end(); // No bound for leftover
		}
		else
		{
			// Regular loser from a pair (offset by 1 because first loser already inserted)
			valueToInsert = sortedPairs[insertionOrder[orderIndex] + 1].loser;

			// Use tracked position of the corresponding winner to bound the search
			searchEnd = sortedChain.begin() + winnerPositions[insertionOrder[orderIndex] + 1];
		}

		std::vector<int>::iterator insertPos = std::lower_bound(sortedChain.begin(), searchEnd, valueToInsert);
		sortedChain.insert(insertPos, valueToInsert);

		// Update positions: all winners after the insertion point shift by 1
		std::size_t insertedAt = insertPos - sortedChain.begin();
		for (std::size_t i = 0; i < winnerPositions.size(); ++i)
			if (winnerPositions[i] >= insertedAt)
				winnerPositions[i]++;

	}

	return sortedChain;
}

// =======================================================
// Deque wrapper
// =======================================================
std::deque<int> PmergeMe::mergeInsertSort(const std::deque<int>& data)
{
	return data;
}