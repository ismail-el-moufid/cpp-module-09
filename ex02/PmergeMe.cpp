#include "PmergeMe.hpp"
#include <vector>
#include <deque>
#include <algorithm>


static std::vector<std::size_t> jacobsthalOrder(std::size_t n)
{
	std::vector<std::size_t> order;
	if (n == 0) return order;

	// Generate Jacobsthal numbers up to n
	std::vector<std::size_t> jac;
	jac.push_back(0);
	jac.push_back(1);
	while (jac.back() < n)
	{
		std::size_t next = jac.back() + 2 * jac[jac.size() - 2];
		if (next > n) break;
		jac.push_back(next);
	}

	// Build insertion order: for each Jacobsthal block, insert backwards
	for (std::size_t i = 2; i < jac.size(); ++i)
	{
		std::size_t start = std::min(jac[i], n);
		std::size_t end = jac[i - 1] + 1;
		for (std::size_t k = start; k >= end && k > 0; --k)
			order.push_back(k - 1);
	}

	// Add any remaining elements
	if (!jac.empty() && jac.back() < n)
		for (std::size_t k = n; k > jac.back(); --k)
			order.push_back(k - 1);

	return order;
}

template<typename T>
struct Pair
{
	T winner;
	T loser;
	Pair() : winner(T()), loser(T()) {}
	Pair(T w, T l) : winner(w), loser(l) {}
	bool operator<(const Pair &other) const { return winner < other.winner; }
};

// ----------------------------------------------------------------
// Vector version of Ford-Johnson algorithm
// ----------------------------------------------------------------

static void fordJohnsonVector(std::vector<int> &data)
{
	typedef int T;
	std::size_t n = data.size();

	// Base cases
	if (n <= 1)
		return;
	if (n == 2)
	{
		if (data[0] > data[1])
			std::swap(data[0], data[1]);
		return;
	}

	// Step 1: Pair and compare
	std::vector<Pair<T> > pairs;
	pairs.reserve(n / 2 + 1);
	for (std::size_t i = 0; i + 1 < n; i += 2)
	{
		if (data[i] > data[i + 1])
			pairs.push_back(Pair<T>(data[i], data[i + 1]));
		else
			pairs.push_back(Pair<T>(data[i + 1], data[i]));
	}

	// Handle odd element
	T straggler = T();
	bool hasStraggler = (n % 2 == 1);
	if (hasStraggler)
		straggler = data[n - 1];

	// Step 2: Recursively sort winners
	std::vector<int> winners;
	winners.reserve(pairs.size());
	for (std::size_t i = 0; i < pairs.size(); ++i)
		winners.push_back(pairs[i].winner);

	fordJohnsonVector(winners);

	// Reorder pairs based on sorted winners
	std::vector<Pair<T> > sortedPairs;
	sortedPairs.reserve(pairs.size());
	std::vector<bool> used(pairs.size(), false);
	for (std::size_t i = 0; i < winners.size(); ++i)
	{
		for (std::size_t j = 0; j < pairs.size(); ++j)
		{
			if (!used[j] && pairs[j].winner == winners[i])
			{
				sortedPairs.push_back(pairs[j]);
				used[j] = true;
				break;
			}
		}
	}
	pairs = sortedPairs;

	// Step 3: Build initial main chain
	std::vector<int> chain;
	chain.reserve(n);
	chain.push_back(pairs[0].loser); // b_1 inserted for free
	for (std::size_t i = 0; i < pairs.size(); ++i)
		chain.push_back(pairs[i].winner);

	// Step 4: Insert remaining losers using Jacobsthal order
	std::size_t pendSize = pairs.size() - 1 + (hasStraggler ? 1 : 0);
	std::vector<std::size_t> order = jacobsthalOrder(pendSize);
	for (std::size_t i = 0; i < order.size(); ++i)
	{
		std::size_t idx = order[i];
		T valueToInsert;
		std::size_t maxPos;
		if (idx < pairs.size() - 1)
		{
			valueToInsert = pairs[idx + 1].loser;
			maxPos = idx + 2;
		}
		else
		{
			valueToInsert = straggler;
			maxPos = chain.size();
		}

		// Binary search with bounded range
		std::vector<int>::iterator pos = std::lower_bound(
			chain.begin(),
			chain.begin() + std::min(maxPos, chain.size()),
			valueToInsert
		);
		chain.insert(pos, valueToInsert);
	}

	data = chain;
}

// ----------------------------------------------------------------
// Deque version of Ford-Johnson algorithm
// ----------------------------------------------------------------

static void fordJohnsonDeque(std::deque<int> &data)
{
	typedef int T;
	std::size_t n = data.size();

	// Base cases
	if (n <= 1)
		return;
	if (n == 2)
	{
		if (data[0] > data[1])
			std::swap(data[0], data[1]);
		return;
	}

	// Step 1: Pair and compare
	std::vector<Pair<T> > pairs;
	for (std::size_t i = 0; i + 1 < n; i += 2)
	{
		if (data[i] > data[i + 1])
			pairs.push_back(Pair<T>(data[i], data[i + 1]));
		else
			pairs.push_back(Pair<T>(data[i + 1], data[i]));
	}

	// Handle odd element
	T straggler = T();
	bool hasStraggler = (n % 2 == 1);
	if (hasStraggler)
		straggler = data[n - 1];

	// Step 2: Recursively sort winners
	std::deque<int> winners;
	for (std::size_t i = 0; i < pairs.size(); ++i)
		winners.push_back(pairs[i].winner);

	fordJohnsonDeque(winners);

	// Reorder pairs based on sorted winners
	std::vector<Pair<T> > sortedPairs;
	std::vector<bool> used(pairs.size(), false);
	for (std::size_t i = 0; i < winners.size(); ++i)
	{
		for (std::size_t j = 0; j < pairs.size(); ++j)
		{
			if (!used[j] && pairs[j].winner == winners[i])
			{
				sortedPairs.push_back(pairs[j]);
				used[j] = true;
				break;
			}
		}
	}
	pairs = sortedPairs;

	// Step 3: Build initial main chain
	std::deque<int> chain;
	chain.push_back(pairs[0].loser); // b_1 inserted for free
	for (std::size_t i = 0; i < pairs.size(); ++i)
		chain.push_back(pairs[i].winner);

	// Step 4: Insert remaining losers using Jacobsthal order
	std::size_t pendSize = pairs.size() - 1 + (hasStraggler ? 1 : 0);
	std::vector<std::size_t> order = jacobsthalOrder(pendSize);
	for (std::size_t i = 0; i < order.size(); ++i)
	{
		std::size_t idx = order[i];
		T valueToInsert;
		std::size_t maxPos;
		if (idx < pairs.size() - 1)
		{
			valueToInsert = pairs[idx + 1].loser;
			maxPos = idx + 2;
		}
		else
		{
			valueToInsert = straggler;
			maxPos = chain.size();
		}

		// Binary search with bounded range
		std::deque<int>::iterator pos = std::lower_bound(
			chain.begin(),
			chain.begin() + std::min(maxPos, chain.size()),
			valueToInsert
		);
		chain.insert(pos, valueToInsert);
	}

	data = chain;
}

std::vector<int> PmergeMe::mergeInsertSort(const std::vector<int> &data)
{
	std::vector<int> result = data;
	fordJohnsonVector(result);
	return result;
}

std::deque<int> PmergeMe::mergeInsertSort(const std::deque<int> &data)
{
	std::deque<int> result = data;
	fordJohnsonDeque(result);
	return result;
}