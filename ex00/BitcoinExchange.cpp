#include "BitcoinExchange.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include <cstdlib>

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other) {this->exchangeRates = other.exchangeRates;}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this != &other)
		this->exchangeRates = other.exchangeRates;
	return (*this);
}

BitcoinExchange::~BitcoinExchange() {}

bool BitcoinExchange::loadData(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		return false;

	std::string line;
	if (!std::getline(file, line))
		return false;

	std::string header = trim(line);
	size_t comma = header.find(',');
	if (comma == std::string::npos)
		return false;

	std::string h1 = trim(header.substr(0, comma));
	std::string h2 = trim(header.substr(comma + 1));

	if (h1 != "date" || h2 != "exchange_rate")
		return false;

	std::map<std::string, double> tempRates;
	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string datePart;
		if (!std::getline(ss, datePart, ','))
			continue;

		std::string ratePart;
		std::getline(ss, ratePart);

		std::string date = trim(datePart);
		std::string rateStr = trim(ratePart);

		if (date.empty() || rateStr.empty())
			continue;
		if (!isValidDate(date))
			continue;

		double rate;
		if (!parseDouble(rateStr, rate))
			continue;

		if (tempRates.find(date) == tempRates.end())
			tempRates[date] = rate;
	}

	file.close();
	if (tempRates.empty())
		return false;

	this->exchangeRates = tempRates;
	return true;
}

bool BitcoinExchange::getExchangeRate(const std::string &date, double &rate) const
{
	std::map<std::string, double>::const_iterator it = exchangeRates.lower_bound(date);

	if (it != exchangeRates.end() && it->first == date)
	{
		rate = it->second;
		return (true);
	}

	if (it == exchangeRates.begin())
		return (false);

	--it;
	rate = it->second;
	return (true);
}

void BitcoinExchange::printExchangeRates() const
{
	for (std::map<std::string, double>::const_iterator it = exchangeRates.begin(); it != exchangeRates.end(); ++it)
		std::cout << it->first << " => " << it->second << std::endl;
}

std::string BitcoinExchange::trim(const std::string &s)
{
	size_t start = s.find_first_not_of(" \t");
	if (start == std::string::npos)
		return "";
	size_t end = s.find_last_not_of(" \t");
	return s.substr(start, end - start + 1);
}

bool BitcoinExchange::isValidDate(const std::string &date)
{
	if (date.size() != 10 || date[4] != '-' || date[7] != '-')
		return false;

	std::string ys = date.substr(0, 4);
	std::string ms = date.substr(5, 2);
	std::string ds = date.substr(8, 2);

	for (size_t i = 0; i < ys.size(); ++i)
		if (!std::isdigit(static_cast<unsigned char>(ys[i]))) return false;
	for (size_t i = 0; i < ms.size(); ++i)
		if (!std::isdigit(static_cast<unsigned char>(ms[i]))) return false;
	for (size_t i = 0; i < ds.size(); ++i)
		if (!std::isdigit(static_cast<unsigned char>(ds[i]))) return false;

	int y = std::atoi(ys.c_str());
	int m = std::atoi(ms.c_str());
	int d = std::atoi(ds.c_str());

	if (y < 1000 || y > 9999)
		return false;
	if (m < 1 || m > 12)
		return false;

	static const int mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int maxDay = mdays[m - 1];

	if (m == 2 && (y % 4 == 0 && ((y % 100 != 0) || (y % 400 == 0))))
		maxDay = 29;

	if (d < 1 || d > maxDay)
		return false;
	return true;
}

bool BitcoinExchange::parseDouble(const std::string &text, double &out)
{
	std::istringstream iss(text);
	iss >> out;
	if (iss.fail())
		return false;
	char c;
	if (iss >> c)
		return false;
	return true;
}