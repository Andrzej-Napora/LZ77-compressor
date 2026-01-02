#pragma once
#include <iostream>
#include <string>
#include <map>
#include <iomanip>
#include <vector>
#include <utility>

struct Token
{
	unsigned short dystans{};
	unsigned char dlugosc{};
	Token(unsigned short dystans, unsigned char dlugosc)
	{
		this->dystans = dystans;
		this->dlugosc = dlugosc;
	}
};

void AlgorytmKodujacyLZ77(const std::string& ipath,const std::string& opath);
void AlgorytmDekodujacyLZ77(const std::string& ipath, const std::string& opath);
void flagSwap(std::ofstream& ofile, long long& pos, unsigned char& flagBajt, int& bit_count);
Token bestHashFinder
(long long cursor1, const std::string& fullText,
	std::vector<long long>& head, std::vector<long long>& prev,const long long& size);
void tokenFlagUpdate
(std::ofstream& ofile, unsigned char& flagBajt, int& bit_count, int i, 
	const std::vector<Token>& tokenList, long long& cursor1, unsigned char max_dlugosc, long long& pos);
void literalFlagUpdate
(std::ofstream& ofile, const std::string& fullText,
	unsigned char& flagBajt, int& bit_count, long long& cursor1, long long& pos);
unsigned short hashCalculation(const std::string& wyraz);