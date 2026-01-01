#pragma once
#include <iostream>
#include <string>
#include <map>
#include <iomanip>
#include <vector>
#include <utility>

struct Token
{
	long long index{};
	unsigned short dystans{};
	unsigned char dlugosc{};
	Token(long long index, unsigned short dystans, unsigned char dlugosc)
	{
		this->index = index;
		this->dystans = dystans;
		this->dlugosc = dlugosc;
	}
};

void AlgorytmKodujacyLZ77(const std::string& ipath,const std::string& opath);
void AlgorytmDekodujacyLZ77(const std::string& ipath, const std::string& opath);
void flagSwap(std::ofstream& ofile, long long& pos, unsigned char& flagBajt, int& bit_count);
Token bestMatchFinder
(long long cursor1, const std::string& fullText);
void tokenFlagUpdate
(std::ofstream& ofile, unsigned char& flagBajt, int& bit_count, int i, 
	const std::vector<Token>& tokenList, long long& cursor1, unsigned char max_dlugosc, long long& pos);
void literalFlagUpdate
(std::ofstream& ofile, const std::string& fullText,
	unsigned char& flagBajt, int& bit_count, long long& cursor1, long long& pos);