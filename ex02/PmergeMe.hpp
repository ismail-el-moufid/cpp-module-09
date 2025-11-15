#pragma once

#include <vector>
#include <deque>

class PmergeMe
{

private:
	PmergeMe();
	PmergeMe(const PmergeMe& other);
	PmergeMe& operator=(const PmergeMe& other);
	~PmergeMe();

public:
	static std::vector<int> mergeInsertSort(const std::vector<int>& data);
	static std::deque<int> mergeInsertSort(const std::deque<int>& data);

};