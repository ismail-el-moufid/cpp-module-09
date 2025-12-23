#include "PmergeMe.hpp"
#include <vector>
#include <deque>
#include <algorithm>
#include <cstddef>

struct Pair
{
	int winner;
	int loser;

	Pair(int winnerValue = 0, int loserValue = 0) : winner(winnerValue), loser(loserValue) {}
};

// Jacobsthal order
std::vector<std::size_t> jacobsthalOrder(std::size_t size)
{
	std::vector<std::size_t> insertionOrder;

	if (size == 0)
		return insertionOrder;

	// Generate Jacobsthal numbers
	std::vector<std::size_t> jacobsthal;
	jacobsthal.push_back(0);
	jacobsthal.push_back(1);

	while (jacobsthal.back() < size)
	{
		std::size_t next = jacobsthal.back() + 2 * jacobsthal[jacobsthal.size() - 2];
		jacobsthal.push_back(next);
	}

	// Build insertion order following Jacobsthal sequence
	std::vector<bool> added(size, false);

	for (std::size_t j = 2; j < jacobsthal.size(); ++j)
	{
		std::size_t start = std::min(jacobsthal[j], size);
		std::size_t end = jacobsthal[j - 1];

		for (std::size_t i = start; i > end; --i)
		{
			if (i - 1 < size && !added[i - 1])
			{
				insertionOrder.push_back(i - 1);
				added[i - 1] = true;
			}
		}
	}

	// Add any remaining indices not covered by Jacobsthal sequence
	for (std::size_t i = 0; i < size; ++i)
		if (!added[i])
			insertionOrder.push_back(i);

	return insertionOrder;
}

// Vector helper functions
std::vector<Pair> createPairsVector(const std::vector<int>& data)
{
	std::vector<Pair> pairs;
	pairs.reserve((data.size() + 1) / 2);

	for (std::size_t i = 0; i + 1 < data.size(); i += 2)
	{
		if (data[i] > data[i + 1])
			pairs.push_back(Pair(data[i], data[i + 1]));
		else
			pairs.push_back(Pair(data[i + 1], data[i]));
	}

	return pairs;
}

std::vector<int> extractWinnersVector(const std::vector<Pair>& pairs)
{
	std::vector<int> winners;
	winners.reserve(pairs.size());

	for (std::size_t i = 0; i < pairs.size(); ++i)
		winners.push_back(pairs[i].winner);

	return winners;
}

std::vector<Pair> mapSortedPairsVector(const std::vector<int>& sortedWinners, const std::vector<Pair>& pairs)
{
	std::vector<Pair> sortedPairs;
	sortedPairs.reserve(pairs.size());

	std::vector<bool> used(pairs.size(), false);

	for (std::size_t i = 0; i < sortedWinners.size(); ++i)
	{
		for (std::size_t j = 0; j < pairs.size(); ++j)
		{
			if (!used[j] && pairs[j].winner == sortedWinners[i])
			{
				sortedPairs.push_back(pairs[j]);
				used[j] = true;
				break;
			}
		}
	}

	return sortedPairs;
}

std::vector<int> buildInitialChainVector(const std::vector<Pair>& sortedPairs)
{
	std::vector<int> sortedChain;

	// Insert first loser (smallest among all elements)
	if (!sortedPairs.empty())
		sortedChain.push_back(sortedPairs[0].loser);

	// Insert all winners in sorted order
	for (std::size_t i = 0; i < sortedPairs.size(); ++i)
		sortedChain.push_back(sortedPairs[i].winner);

	return sortedChain;
}

void insertRemainingLosersVector(std::vector<int>& sortedChain, const std::vector<Pair>& sortedPairs, int leftover, bool hasLeftover)
{
	// Track current position of each winner in sortedChain
	std::vector<std::size_t> winnerPos(sortedPairs.size());

	for (std::size_t i = 0; i < sortedPairs.size(); ++i)
		winnerPos[i] = i + 1;

	// Insert remaining losers using Jacobsthal order with bounded binary search
	std::size_t numLosersToInsert = sortedPairs.size() - 1;

	if (hasLeftover)
		numLosersToInsert++;

	std::vector<std::size_t> insertionOrder = jacobsthalOrder(numLosersToInsert);

	for (std::size_t i = 0; i < insertionOrder.size(); ++i)
	{
		int valueToInsert;
		std::vector<int>::iterator searchEnd;

		// Check if this is the leftover element
		if (insertionOrder[i] == sortedPairs.size() - 1 && hasLeftover)
		{
			valueToInsert = leftover;
			searchEnd = sortedChain.end();
		}
		else
		{
			// Regular loser from a pair (offset by 1 since first loser already inserted)
			valueToInsert = sortedPairs[insertionOrder[i] + 1].loser;

			// Bound search using corresponding winner's position
			searchEnd = sortedChain.begin() + winnerPos[insertionOrder[i] + 1];
		}

		// Bounded binary search for insertion point
		std::vector<int>::iterator insertPos = std::lower_bound(sortedChain.begin(), searchEnd, valueToInsert);

		sortedChain.insert(insertPos, valueToInsert);

		// Update all winner positions after insertion point
		std::size_t insertedAt = insertPos - sortedChain.begin();

		for (std::size_t j = 0; j < winnerPos.size(); ++j)
			if (winnerPos[j] >= insertedAt)
				winnerPos[j]++;
	}
}

// Deque helper functions
std::deque<Pair> createPairsDeque(const std::deque<int>& data)
{
	std::deque<Pair> pairs;

	for (std::size_t i = 0; i + 1 < data.size(); i += 2)
	{
		if (data[i] > data[i + 1])
			pairs.push_back(Pair(data[i], data[i + 1]));
		else
			pairs.push_back(Pair(data[i + 1], data[i]));
	}

	return pairs;
}

std::deque<int> extractWinnersDeque(const std::deque<Pair>& pairs)
{
	std::deque<int> winners;

	for (std::size_t i = 0; i < pairs.size(); ++i)
		winners.push_back(pairs[i].winner);

	return winners;
}

std::deque<Pair> mapSortedPairsDeque(const std::deque<int>& sortedWinners, const std::deque<Pair>& pairs)
{
	std::deque<Pair> sortedPairs;

	std::vector<bool> used(pairs.size(), false);

	for (std::size_t i = 0; i < sortedWinners.size(); ++i)
	{
		for (std::size_t j = 0; j < pairs.size(); ++j)
		{
			if (!used[j] && pairs[j].winner == sortedWinners[i])
			{
				sortedPairs.push_back(pairs[j]);
				used[j] = true;
				break;
			}
		}
	}

	return sortedPairs;
}

std::deque<int> buildInitialChainDeque(const std::deque<Pair>& sortedPairs)
{
	std::deque<int> sortedChain;

	// Insert first loser (smallest among all elements)
	if (!sortedPairs.empty())
		sortedChain.push_back(sortedPairs[0].loser);

	// Insert all winners in sorted order
	for (std::size_t i = 0; i < sortedPairs.size(); ++i)
		sortedChain.push_back(sortedPairs[i].winner);

	return sortedChain;
}

void insertRemainingLosersDeque(std::deque<int>& sortedChain, const std::deque<Pair>& sortedPairs, int leftover, bool hasLeftover)
{
	// Track current position of each winner in sortedChain
	std::vector<std::size_t> winnerPos(sortedPairs.size());

	for (std::size_t i = 0; i < sortedPairs.size(); ++i)
		winnerPos[i] = i + 1;

	// Insert remaining losers using Jacobsthal order with bounded binary search
	std::size_t numLosersToInsert = sortedPairs.size() - 1;

	if (hasLeftover)
		numLosersToInsert++;

	std::vector<std::size_t> insertionOrder = jacobsthalOrder(numLosersToInsert);

	for (std::size_t i = 0; i < insertionOrder.size(); ++i)
	{
		int valueToInsert;
		std::deque<int>::iterator searchEnd;

		// Check if this is the leftover element
		if (insertionOrder[i] == sortedPairs.size() - 1 && hasLeftover)
		{
			valueToInsert = leftover;
			searchEnd = sortedChain.end();
		}
		else
		{
			// Regular loser from a pair (offset by 1 since first loser already inserted)
			valueToInsert = sortedPairs[insertionOrder[i] + 1].loser;

			// Bound search using corresponding winner's position
			searchEnd = sortedChain.begin() + winnerPos[insertionOrder[i] + 1];
		}

		// Bounded binary search for insertion point
		std::deque<int>::iterator insertPos = std::lower_bound(sortedChain.begin(), searchEnd, valueToInsert);

		sortedChain.insert(insertPos, valueToInsert);

		// Update all winner positions after insertion point
		std::size_t insertedAt = insertPos - sortedChain.begin();

		for (std::size_t j = 0; j < winnerPos.size(); ++j)
			if (winnerPos[j] >= insertedAt)
				winnerPos[j]++;
	}
}

// Main sorting functions

// Ford-Johnson merge-insert sort for vector (recursive)
std::vector<int> PmergeMe::mergeInsertSort(const std::vector<int>& data)
{
	const std::size_t dataSize = data.size();

	if (dataSize <= 1)
		return data;

	if (dataSize == 2)
	{
		if (data[0] <= data[1])
			return data;

		std::vector<int> sorted;
		sorted.push_back(data[1]);
		sorted.push_back(data[0]);
		return sorted;
	}

	// Create pairs: larger element as winner, smaller as loser
	std::vector<Pair> pairs = createPairsVector(data);

	// Handle leftover element if data size is odd
	int leftover = 0;
	bool hasLeftover = false;

	if (dataSize % 2 == 1)
	{
		leftover = data[dataSize - 1];
		hasLeftover = true;
	}

	// Extract winners for recursive sorting
	std::vector<int> winners = extractWinnersVector(pairs);

	// Sort the winners
	std::vector<int> sortedWinners = mergeInsertSort(winners);

	// Map sorted winners back to their original pairs
	std::vector<Pair> sortedPairs = mapSortedPairsVector(sortedWinners, pairs);

	// Build sorted chain using Ford-Johnson insertion
	std::vector<int> sortedChain = buildInitialChainVector(sortedPairs);

	// Insert remaining losers using Jacobsthal order with bounded binary search
	insertRemainingLosersVector(sortedChain, sortedPairs, leftover, hasLeftover);

	return sortedChain;
}

// Ford-Johnson merge-insert sort for deque (recursive)
std::deque<int> PmergeMe::mergeInsertSort(const std::deque<int>& data)
{
	const std::size_t dataSize = data.size();

	if (dataSize <= 1)
		return data;

	if (dataSize == 2)
	{
		if (data[0] <= data[1])
			return data;

		std::deque<int> sorted;
		sorted.push_back(data[1]);
		sorted.push_back(data[0]);
		return sorted;
	}

	// Create pairs: larger element as winner, smaller as loser
	std::deque<Pair> pairs = createPairsDeque(data);

	// Handle leftover element if data size is odd
	int leftover = 0;
	bool hasLeftover = false;

	if (dataSize % 2 == 1)
	{
		leftover = data[dataSize - 1];
		hasLeftover = true;
	}

	// Extract winners for recursive sorting
	std::deque<int> winners = extractWinnersDeque(pairs);

	// Sort the winners
	std::deque<int> sortedWinners = mergeInsertSort(winners);

	// Map sorted winners back to their original pairs
	std::deque<Pair> sortedPairs = mapSortedPairsDeque(sortedWinners, pairs);

	// Build sorted chain using Ford-Johnson insertion
	std::deque<int> sortedChain = buildInitialChainDeque(sortedPairs);

	// Insert remaining losers using Jacobsthal order with bounded binary search
	insertRemainingLosersDeque(sortedChain, sortedPairs, leftover, hasLeftover);

	return sortedChain;
}