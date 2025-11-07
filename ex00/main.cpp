#include "BitcoinExchange.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static std::string trim(const std::string &s)
{
	size_t start = s.find_first_not_of(" 0\t");

	if (start == std::string::npos)
		return ("");

	size_t end = s.find_last_not_of(" \t");
	return (s.substr(start, end - start + 1));
}

static int	daysInMonth(int y, int m)
{
	static const int mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (m == 2)
		return ((y % 4 == 0 && ((y % 100 != 0) || (y % 400 == 0))) ? 29 : 28);

	if (m >= 1 && m <= 12)
		return (mdays[m - 1]);

	return (0);
}

static bool	isAllDigits(const std::string &s)
{
	if (s.empty())
		return (false);

	for (size_t i = 0; i < s.size(); ++i)
		if (!std::isdigit(static_cast<unsigned char>(s[i])))
			return (false);

	return (true);
}

static bool	isValidDate(const std::string &date)
{
	int	y;
	int	m;
	int	d;

	if (date.size() != 10)
		return (false);

	std::string ys = date.substr(0, date.find_first_of('-'));
	std::string ms = date.substr(date.find_first_of('-') + 1,  date.find_last_of('-')  - date.find_first_of('-') - 1);
	std::string ds = date.substr(date.find_last_of('-') + 1);

	if (ys.size() + ms.size() + ds.size() + 2 != date.size() ||
		!isAllDigits(ys) || !isAllDigits(ms) || !isAllDigits(ds))
		return false;

	y = std::atoi(ys.c_str());
	m = std::atoi(ms.c_str());
	d = std::atoi(ds.c_str());

	if (m < 1 || m > 12)
		return false;

	if (d < 1 || d > daysInMonth(y, m))
		return false;

	return true;
}

static bool	parseDouble(const std::string &text, double &out)
{
	char	c;

	std::istringstream iss(text);
	iss >> out;

	if (iss.fail())
		return false;

	if (iss >> c)
		return false;

	return true;
}

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << (argc > 0 ? argv[0] : "./btc") << " <input_file>" << std::endl;
		return 1;
	}

	BitcoinExchange bitcoinExchange;
	if (!bitcoinExchange.loadData("data.csv"))
	{
		std::cerr << "Error: Could not load data from file." << std::endl;
		return 1;
	}

	std::ifstream in(argv[1]);
	if (!in.is_open())
	{
		std::cerr << "Error: could not open file." << std::endl;
		return 1;
	}

	std::string line;
	std::getline(in, line);
	while (std::getline(in, line))
	{
		line = trim(line);
		if (line.empty())
			continue ;

		size_t bar = line.find('|');
		if (bar == std::string::npos)
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue ;
		}

		std::string date = trim(line.substr(0, bar));
		std::string valueStr = trim(line.substr(bar + 1));

		if (!isValidDate(date))
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue ;
		}

		double value;
		if (!parseDouble(valueStr, value))
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue ;
		}
		if (value < 0)
		{
			std::cerr << "Error: not a positive number." << std::endl;
			continue ;
		}
		if (value > 1000)
		{
			std::cerr << "Error: too large a number." << std::endl;
			continue ;
		}

		double rate;
		if (!bitcoinExchange.getExchangeRate(date, rate))
		{
			std::cerr << "Error: no exchange rate available for " << date << std::endl;
			continue ;
		}

		double result = value * rate;
		std::cout << date << " => " << value << " = " << result << std::endl;
	}
	in.close();

}