#include <iostream>
#include <string>
#include <map>
#include <iomanip>
#include <cctype>
#include <vector>
#include <utility>
#include <fstream>
#include "../LZ77/Header.h"

void AlgorytmKodujacyLZ77(const std::string& ipath, const std::string& opath)
{
	std::string fullText;
	std::ifstream ifile(ipath, std::ios::binary | std::ios::ate);
	std::ofstream ofile(opath, std::ios::binary);
	if(ifile.good() && ofile.good())
	{
		auto size = ifile.tellg();
		ifile.seekg(0);
		fullText.resize(size);
		ifile.read(&fullText[0], size);


		std::vector<unsigned long long> head(std::numeric_limits<unsigned short>::max(), -1);
		std::vector<unsigned long long> prev(size, -1);

		unsigned long long pos = ofile.tellp();
		unsigned char flagBajt{};
		ofile.put(flagBajt);		//place holder
		int bit_count{};
		unsigned long long cursor1 = 0;

		while(cursor1 < size)
		{
			std::vector<Token> tokenList;
			for (int i = 0; i < 1; i++)		//szukanie najlepszego dopasowania 2-znaki do przodu
			{
				if (cursor1 + i < size)
				{
				tokenList.push_back(bestHashFinder(cursor1  + i, fullText,head,prev));
				}
			}
			unsigned char max_dlugosc{};
			int tokenIndex{};
			for(int i=0;i<tokenList.size();i++)		//szukam najdluzszego tokenu
			{
				if (tokenList[i].dlugosc > max_dlugosc)
				{
					max_dlugosc = tokenList[i].dlugosc;
					tokenIndex = i;
				}
			}
			if(tokenIndex==0)
			{
				if (max_dlugosc < 3)
					literalFlagUpdate(ofile, fullText, flagBajt, bit_count, cursor1,pos);
				else if (max_dlugosc >= 3)
					tokenFlagUpdate(ofile, flagBajt, bit_count, 0, tokenList, cursor1, max_dlugosc,pos);
			}
			else if (tokenIndex > 0)
			{
				for (int i = 0; i <= tokenIndex; i++)	//obsluguje tylko i=0 oraz i=1, wiec petla wydaje sie nie
				{										//potrzebna, ale gdybym w przyszlosci chcial oblugiwac wieksza 
					if (i<tokenIndex)					//glebokosc iteracji zostawiam to w formie petli
						literalFlagUpdate(ofile, fullText, flagBajt, bit_count, cursor1,pos);
					else if (i == tokenIndex && max_dlugosc < 3)
						literalFlagUpdate(ofile, fullText, flagBajt, bit_count, cursor1,pos);
					else if (i == tokenIndex && max_dlugosc >= 3)
						tokenFlagUpdate(ofile, flagBajt, bit_count, i, tokenList, cursor1, max_dlugosc,pos);
				}
			}
		}
		flagBajt <<= 1;
		flagBajt |= 1;
		bit_count++;
		flagBajt <<= 8 - bit_count;
		unsigned long long temp_pos;
		temp_pos = ofile.tellp();
		ofile.seekp(pos);
		ofile.put(flagBajt);
		ofile.seekp(temp_pos);
		unsigned short dystans{};
		unsigned char dlugosc{};
		ofile.write(reinterpret_cast<const char*>(&dystans), sizeof(dystans));
		ofile.write(reinterpret_cast<const char*>(&dlugosc), sizeof(dlugosc));
	}
	else
	{
		std::cout << "Blad odczytu pliku przy kodowaniu" << std::endl;
	}
}

void literalFlagUpdate
(std::ofstream& ofile,const std::string& fullText,
	unsigned char& flagBajt, int& bit_count, unsigned long long& cursor1, unsigned long long& pos)
{
	flagBajt <<= 1;
	bit_count++;
	ofile.put(fullText[cursor1++]);
	flagSwap(ofile, pos, flagBajt, bit_count);
}

void tokenFlagUpdate
(std::ofstream& ofile, unsigned char& flagBajt, int& bit_count, int i,
	const std::vector<Token>& tokenList, unsigned long long& cursor1, unsigned char max_dlugosc, unsigned long long& pos)
{
	flagBajt <<= 1;
	flagBajt |= 1;
	bit_count++;
	cursor1 += max_dlugosc;
	ofile.write(reinterpret_cast<const char*>(&tokenList[i].dystans), sizeof(tokenList[i].dystans));
	ofile.write(reinterpret_cast<const char*>(&tokenList[i].dlugosc), sizeof(tokenList[i].dlugosc));
	flagSwap(ofile, pos, flagBajt, bit_count);
}

void flagSwap(std::ofstream& ofile, unsigned long long& pos, unsigned char& flagBajt, int& bit_count)
{
	if (bit_count == 8)
	{
		long long temp_pos = ofile.tellp();
		ofile.seekp(pos);
		ofile.put(flagBajt);
		ofile.seekp(temp_pos);
		pos = temp_pos;
		flagBajt = 0;
		bit_count = 0;
		ofile.put(flagBajt);	//place holder
	}
}

void AlgorytmDekodujacyLZ77(const std::string& ipath, const std::string& opath)
{
	std::string fullText;

	std::ifstream ifile(ipath, std::ios::binary | std::ios::ate);
	std::ofstream ofile(opath, std::ios::binary);
	if(ifile.good() && ofile.good())
	{
		auto size = ifile.tellg();
		ifile.seekg(0);
		fullText.resize(size);
		ifile.read(&fullText[0], size);

		unsigned char flagBajt{};
		int bit_count = 8;

		std::string dekoded;
		int cursor = 0;
		while(cursor < size)
		{
			if (bit_count == 8)
			{
				bit_count = 0;
				flagBajt = fullText[cursor];
				cursor++;
			}
			bool bit = (flagBajt & 128)!=0;
			flagBajt <<= 1;
			bit_count++;
			if (bit == 0)
			{
				dekoded.push_back(fullText[cursor]);
				ofile.put(fullText[cursor]);
				cursor+=sizeof(unsigned char);
				continue;
			}
			else if (bit == 1)
			{
				unsigned short dystans = *reinterpret_cast<const unsigned short*>(&fullText[cursor]);
				cursor += sizeof(unsigned short);
				unsigned char dlugosc = *reinterpret_cast<const unsigned char*>(&fullText[cursor]);
				cursor += sizeof(unsigned char);

				if (dystans == 0 && dlugosc == 0)		//warunek zakonczenia programu
					break;

				if (dystans > dekoded.size())
				{
					std::cout << "Blad odczytu danych" << std::endl;
					return;
				}
				int index = dekoded.size() - dystans;
				for (int i = 0; i < dlugosc; i++)
				{
					dekoded.push_back(dekoded[index+i]);
				}
				ofile.write(&dekoded[dekoded.size() - dlugosc], dlugosc);
				continue;
			}
		}
	}
	else
	{
		std::cout << "Blad odczytu pliku przy dekodowaniu" << std::endl;
	}
}

unsigned short hashCalculation(const std::string& wyraz)
{
	unsigned short twoBajt{};
	twoBajt |= wyraz[0];
	twoBajt <<= 8;
	unsigned short temp{};
	temp |= wyraz[1];
	twoBajt |= temp;
	temp = 0;
	temp |= wyraz[2];
	temp <<= 4;
	twoBajt |= temp;
	return twoBajt;

}

Token bestHashFinder
(long long cursor1, const std::string& fullText,
	std::vector<unsigned long long>& head, std::vector<unsigned long long>& prev)
{
	unsigned short dystans{};
	unsigned char dlugosc{};
	int max{};
	unsigned short window = std::numeric_limits<unsigned short>::max();

	std::string temp{};
	for (int i = 0; i < 3; i++)
	{
		temp.push_back(fullText[cursor1 + i]);
	}
	unsigned short hash = hashCalculation(temp);

	if (head[hash] == -1)
	{
		head[hash] = cursor1;
	}
	else
	{
		prev[cursor1] = head[hash];
		head[hash] = cursor1;
		long long cursor2 = prev[cursor1];
		while (cursor2 != -1 && cursor1 - cursor2 < window )
		{
			bool match = false;
			for (int i = 0; i < 3; i++)
			{
				if (fullText[cursor2 + i] != fullText[cursor1 + i])
				{
					match = false;
					break;
				}
				match = true;
			}
			if (match)
			{
				unsigned short count{};
				long long temp_cursor1 = cursor1;
				long long temp_cursor2 = cursor2;
				while (temp_cursor2 < cursor1 && fullText[temp_cursor1++] == fullText[temp_cursor2++])
					count++;
				if (count > max)
				{
					if (count > 255)
						count = 255;
					max = count;
					dlugosc = max;
					dystans = cursor1 - cursor2;
				}
			}
			cursor2 = prev[cursor2];
		}
	}
	
	Token token(dystans, dlugosc);
	return token;
}