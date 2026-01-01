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

		long long pos = ofile.tellp();
		unsigned char flagBajt{};
		ofile.put(flagBajt);		//place holder
		int bit_count{};

		for (int cursor1 = 0; cursor1 < size; cursor1++)
		{
			unsigned short dystans{};
			unsigned char dlugosc{};
			unsigned short max{};
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
			for (int cursor2 = 0; cursor2 < cursor1; cursor2++)
			{
				unsigned short count{};
				if(fullText[cursor2] == fullText[cursor1])
				{
					int temp_cursor1 = cursor1;
					int temp_cursor2 = cursor2;
					while (fullText[temp_cursor1++] == fullText[temp_cursor2++] && temp_cursor2<cursor1-1)
						count++;
					if(count > max)
					{
						if (count > 255)
							count = 255;
						max = count;
						dlugosc = max;
						dystans = cursor1 - cursor2;
					}
				}
			}
			if (max<3)
			{
				flagBajt <<= 1;
				bit_count++;
				ofile.put(fullText[cursor1]);
			}
			else if(max>=3)
			{
				flagBajt <<= 1;
				flagBajt |= 1;
				bit_count++;
				cursor1 += dlugosc-1;
				ofile.write(reinterpret_cast<const char*>(&dystans), sizeof(dystans));
				ofile.write(reinterpret_cast<const char*>(&dlugosc), sizeof(dlugosc));
				max = 0;
			}
		}
		if (bit_count == 8)
		{
			long long temp_pos = ofile.tellp();
			ofile.seekp(pos);
			ofile.put(flagBajt);
			ofile.seekp(temp_pos);
			pos = temp_pos;
			flagBajt = 0;
			bit_count = 0;
			ofile.put(flagBajt);
		}
		if (bit_count < 8)		//warunek zakonczenia programu
		{
			flagBajt <<= 1;
			flagBajt |= 1;
			bit_count++;
			flagBajt <<= 8 - bit_count;
			long long temp_pos;
			temp_pos = ofile.tellp();
			ofile.seekp(pos);
			ofile.put(flagBajt);
			ofile.seekp(temp_pos);
			unsigned short dystans{};
			unsigned char dlugosc{};
			ofile.write(reinterpret_cast<const char*>(&dystans), sizeof(dystans));
			ofile.write(reinterpret_cast<const char*>(&dlugosc), sizeof(dlugosc));
		}
	}
	else
	{
		std::cout << "Blad odczytu pliku przy kodowaniu" << std::endl;
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