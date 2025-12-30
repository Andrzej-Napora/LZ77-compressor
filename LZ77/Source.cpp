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
		const char flaga_l = -1;
		const char flaga_p = -2;
		auto size = ifile.tellg();
		ifile.seekg(0);
		fullText.resize(size);
		ifile.read(&fullText[0], size);
		for (int cursor1 = 0; cursor1 < size; cursor1++)
		{
			unsigned short dystans{};
			unsigned char dlugosc{};
			int max{0};
			for (int cursor2 = 0; cursor2 < cursor1; cursor2++)
			{
				int count{};
				if(fullText[cursor2] == fullText[cursor1])
				{
					int temp_cursor1 = cursor1;
					int temp_cursor2 = cursor2;
					while (fullText[temp_cursor1++] == fullText[temp_cursor2++])
						count++;
					if(count > max)
					{
						max = count;
						dlugosc = max;
						dystans = cursor1 - cursor2;
					}
				}
			}
			if (max<3)
			{
				ofile.put(flaga_l);
				ofile.put(fullText[cursor1]);
			}
			else if(max>=3)
			{
				cursor1 += dlugosc-1;
				ofile.put(flaga_p);
				ofile.write(reinterpret_cast<const char*>(&dystans), sizeof(dystans));
				ofile.write(reinterpret_cast<const char*>(&dlugosc), sizeof(dlugosc));
				max = 0;
			}
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

		const char flaga_l = -1;
		const char flaga_p = -2;

		std::string dekoded;
		int cursor = 0;
		while(cursor < size)
		{
			if (fullText[cursor] == flaga_l)
			{
				cursor++;
				dekoded.push_back(fullText[cursor]);
				ofile.put(fullText[cursor]);
				cursor++;
				continue;
			}
			if (fullText[cursor] == flaga_p)
			{
				cursor++;
				unsigned short dystans = *reinterpret_cast<const unsigned short*>(&fullText[cursor]);
				cursor += sizeof(unsigned short);
				unsigned char dlugosc = *reinterpret_cast<const unsigned char*>(&fullText[cursor]);
				cursor += sizeof(unsigned char);

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