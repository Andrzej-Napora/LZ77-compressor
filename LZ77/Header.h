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
Token hashSearch
(long long cursor1, const std::string& fullText, std::vector<long long>& head,
	std::vector<long long>& prev);
void tokenFlagUpdate
(std::ofstream& ofile, unsigned char& flagBajt, int& bit_count, int i, const std::vector<Token>& tokenList,
	unsigned char max_dlugosc);
void literalFlagUpdate
(std::ofstream& ofile, unsigned char& flagBajt, int& bit_count, char sign);
unsigned short hashCalculation(const unsigned char c1, const unsigned char c2, const unsigned char c3);
void prevSwap(std::vector<long long>& prev);
void headSwap(std::vector<long long>& head);
void headPrevUpdate(long long cursor1, const std::string& fullText,
	std::vector<long long>& head, std::vector<long long>& prev);