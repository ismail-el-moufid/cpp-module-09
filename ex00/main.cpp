#include "BitcoinExchange.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <cctype>
#include <cstdlib>

std::string getDataFilePath()
{
	DIR *dir = opendir(".");
	if (!dir)
		return "";

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string fname(entry->d_name);

		if (fname == "." || fname == "..")
			continue;

		if (fname.size() >= 4)
		{
			std::string ext = fname.substr(fname.size() - 4);
			for (size_t i = 0; i < ext.size(); ++i)
				ext[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(ext[i])));

			if (ext == ".csv")
			{
				struct stat st;
				if (stat(fname.c_str(), &st) == 0 && S_ISREG(st.st_mode))
				{
					closedir(dir);
					return fname;
				}
			}
		}
	}

	closedir(dir);
	return "";
}


int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << (argc > 0 ? argv[0] : "./btc") << " <input_file>" << std::endl;
		return 1;
	}

	BitcoinExchange bitcoinExchange;
	std::string dataFile = getDataFilePath();
	if (dataFile.empty() || !bitcoinExchange.loadData(dataFile))
	{
		std::cerr << "Error: could not load database." << std::endl;
		return 1;
	}

	std::ifstream in(argv[1]);
	if (!in.is_open())
	{
		std::cerr << "Error: could not open input file." << std::endl;
		return 1;
	}

	std::string line;
	if (!std::getline(in, line))
	{
		std::cerr << "Error: input file is empty." << std::endl;
		return 1;
	}
	
	std::string trimmedLine = line;
	size_t start = trimmedLine.find_first_not_of(" \t");
	if (start != std::string::npos)
	{
		size_t end = trimmedLine.find_last_not_of(" \t");
		trimmedLine = trimmedLine.substr(start, end - start + 1);
	}
	
	if (trimmedLine != "date | value")
	{
		std::cerr << "Error: invalid input file header." << std::endl;
		return 1;
	}

	while (std::getline(in, line))
	{
		start = line.find_first_not_of(" \t");
		if (start == std::string::npos)
			continue;
		size_t end = line.find_last_not_of(" \t");
		line = line.substr(start, end - start + 1);
		
		if (line.empty())
			continue ;

		size_t bar = line.find('|');
		if (bar == std::string::npos)
		{
			std::cerr << "Error: invalid input => " << line << std::endl;
			continue ;
		}

		std::string date = line.substr(0, bar);
		start = date.find_first_not_of(" \t");
		if (start != std::string::npos) {
			end = date.find_last_not_of(" \t");
			date = date.substr(start, end - start + 1);
		}
		
		std::string valueStr = line.substr(bar + 1);
		start = valueStr.find_first_not_of(" \t");
		if (start != std::string::npos) {
			end = valueStr.find_last_not_of(" \t");
			valueStr = valueStr.substr(start, end - start + 1);
		}

		if (date.size() != 10 || date[4] != '-' || date[7] != '-')
		{
			std::cerr << "Error: invalid input => " << line << std::endl;
			continue ;
		}

		double value;
		std::istringstream iss(valueStr);
		iss >> value;
		if (iss.fail())
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue ;
		}
		char c;
		if (iss >> c)
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
	return 0;
}