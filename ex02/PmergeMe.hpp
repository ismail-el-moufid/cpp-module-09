#pragma once

#include <vector>
#include <deque>

class PmergeMe
{

public:
	static std::vector<int> mergeInsertSort(const std::vector<int>& data);
	static std::deque<int> mergeInsertSort(const std::deque<int>& data);

};