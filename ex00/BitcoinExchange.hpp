#pragma once

#include <map>
#include <string>

class BitcoinExchange
{

private:
	std::map<std::string, double> exchangeRates;
	static std::string trim(const std::string &s);
	static bool isValidDate(const std::string &date);
	static bool parseDouble(const std::string &text, double &out);

public:
	BitcoinExchange();
	BitcoinExchange(const BitcoinExchange &other);
	BitcoinExchange &operator=(const BitcoinExchange &other);
	~BitcoinExchange();

	bool loadData(const std::string &filename); //.csv
	bool getExchangeRate(const std::string &date, double &rate) const; 
	void printExchangeRates() const;

};