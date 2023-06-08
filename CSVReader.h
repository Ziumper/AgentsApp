#pragma once
#include <vector>
#include <string>
#include <istream>
enum class CSVState {
    UnquotedField,
    QuotedField,
    QuotedQuote
};

class CSVReader
{
public:
	std::vector<std::string> ReadCSVRow(const std::string& row);
	std::vector<std::vector<std::string>> ReadCSV(std::istream& in);
};

