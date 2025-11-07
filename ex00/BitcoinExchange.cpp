#include "BitcoinExchange.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

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
		double rate;

	std::ifstream file(filename.c_str());
	if (!file.is_open())
		return (false);
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string date;
		if (std::getline(ss, date, ',') && ss >> rate)
			exchangeRates[date] = rate;
	}
	file.close();
	return (true);
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