#pragma once

#include <map>
#include <string>

class BitcoinExchange
{

private:
	std::map<std::string, double> exchangeRates;

public:
	BitcoinExchange();
	BitcoinExchange(const BitcoinExchange &other);
	BitcoinExchange &operator=(const BitcoinExchange &other);
	~BitcoinExchange();

	bool loadData(const std::string &filename);
	bool getExchangeRate(const std::string &date, double &rate) const;

	void printExchangeRates() const;

};