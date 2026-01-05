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
		auto stream_size = ifile.tellg();
		long long size = (long long)stream_size;
		ifile.seekg(0);
		unsigned short twoBajt = std::numeric_limits<unsigned short>::max();
		long long file_cursor{};
		long long cursor1{};

		std::vector<long long> head(twoBajt, -1);
		std::vector<long long> prev(2*twoBajt, -1);

		long long pos = ofile.tellp();
		unsigned char flagBajt{};
		ofile.put(flagBajt);		//place holder
		int bit_count{};
		if(size>=2*twoBajt)
		{
			fullText.resize(2 * twoBajt);
			ifile.read(&fullText[0], 2 * twoBajt);
		}
		else
		{
			fullText.resize(size);
			ifile.read(&fullText[0], size);
		}

		while(file_cursor < size)
		{
			if(cursor1>=fullText.size())
			{
				for (int i = 0; i < twoBajt; i++)
				{
					fullText[i] = fullText[twoBajt + i];
				}
				if (file_cursor + twoBajt >= size)
				{
					unsigned short lastBytes = static_cast<unsigned short>(size - file_cursor);
					fullText.resize(twoBajt + lastBytes);
					ifile.read(&fullText[twoBajt], lastBytes);
				}
				else
					ifile.read(&fullText[twoBajt], twoBajt);
				prevSwap(prev);
				headSwap(head);
				cursor1 = twoBajt;
			}

			std::vector<Token> tokenList;
			for (int i = 0; i <1; i++)		//szukanie najlepszego dopasowania 2-znaki do przodu
			{
				if (cursor1 + i < fullText.size())
				{
					tokenList.push_back(hashSearch(cursor1 + i, fullText, head, prev));
				}
			}
			unsigned char max_dlugosc{};
			long long tokenIndex{};
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
				{
					headPrevUpdate(cursor1, fullText, head, prev);
					literalFlagUpdate(ofile, flagBajt, bit_count, fullText[cursor1]);
					flagSwap(ofile, pos, flagBajt, bit_count);
					cursor1++;
					file_cursor++;
				}
				else if (max_dlugosc >= 3)
				{
					for(int i=0;i<max_dlugosc;i++)
						headPrevUpdate(cursor1+i, fullText, head, prev);
					tokenFlagUpdate(ofile, flagBajt, bit_count, tokenIndex, tokenList, max_dlugosc);
					flagSwap(ofile, pos, flagBajt, bit_count);
					cursor1 += max_dlugosc;
					file_cursor += max_dlugosc;
				}
			}
			else if (tokenIndex > 0)
			{
				for (int i = 0; i <= tokenIndex; i++)	//obsluguje tylko i=0 oraz i=1, wiec petla wydaje sie nie
				{										//potrzebna, ale gdybym w przyszlosci chcial oblugiwac wieksza 
					if (i<tokenIndex)					//glebokosc iteracji zostawiam to w formie petli
					{
						headPrevUpdate(cursor1, fullText, head, prev);
						literalFlagUpdate(ofile, flagBajt, bit_count, fullText[cursor1]);
						flagSwap(ofile, pos, flagBajt, bit_count);
						cursor1++;
						file_cursor++;
					}
					else if (i == tokenIndex && max_dlugosc < 3)
					{
						headPrevUpdate(cursor1, fullText, head, prev);
						literalFlagUpdate(ofile, flagBajt, bit_count, fullText[cursor1]);
						flagSwap(ofile, pos, flagBajt, bit_count);
						cursor1++;
						file_cursor++;
					}
					else if (i == tokenIndex && max_dlugosc >= 3)
					{
						for (int i = 0; i < max_dlugosc; i++)
							headPrevUpdate(cursor1 + i, fullText, head, prev);
						tokenFlagUpdate(ofile, flagBajt, bit_count, tokenIndex, tokenList, max_dlugosc);
						flagSwap(ofile, pos, flagBajt, bit_count);
						cursor1 += max_dlugosc;
						file_cursor += max_dlugosc;
					}
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

void headPrevUpdate(long long cursor1, const std::string& fullText,
	std::vector<long long>& head, std::vector<long long>& prev)
{
	if (cursor1 + 2 <= fullText.size())
	{
		unsigned short hash = hashCalculation(fullText[cursor1], fullText[cursor1 + 1], fullText[cursor1 + 2]);

		if (head[hash] == -1)
			head[hash] = cursor1;
		else
		{
			prev[cursor1] = head[hash];
			head[hash] = cursor1;
		}
	}
}

Token hashSearch
(long long cursor1, const std::string& fullText,
	std::vector<long long>& head, std::vector<long long>& prev)
{
	unsigned short dystans{};
	unsigned char dlugosc{};
	int max{};
	unsigned short window = std::numeric_limits<unsigned short>::max();

	if (cursor1 + 2 <= fullText.size())
	{
		unsigned short hash = hashCalculation(fullText[cursor1], fullText[cursor1 + 1], fullText[cursor1 + 2]);
		{
			long long cursor2 = head[hash];
			int steps = 256;
			while (cursor2 != -1 && cursor1 - cursor2 < window && steps-- > 0)
			{
				bool match = false;
				if (cursor1 + 2 <= fullText.size())
				{
					if (fullText[cursor2] == fullText[cursor1] &&
						fullText[cursor2 + 1] == fullText[cursor1 + 1] &&
						fullText[cursor2 + 2] == fullText[cursor1 + 2])
					{
						match = true;
					}
				}
				if (match)
				{
					unsigned short count{};
					int temp_cursor1 = cursor1;
					int temp_cursor2 = cursor2;
					while (temp_cursor1 < fullText.size() && fullText[temp_cursor1++] == fullText[temp_cursor2++])
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
				if (cursor2 == prev[cursor2])
					break;
				else
					cursor2 = prev[cursor2];
			}
		}
	}
	Token token(dystans, dlugosc);
	return token;
}

void prevSwap(std::vector<long long>& prev)
{
	unsigned short twoBajt = std::numeric_limits<unsigned short>::max();
	for (int i=0;i<twoBajt;i++)
	{
		prev[i] = prev[twoBajt + i];
		if(prev[i]!=-1)
			prev[i] -= twoBajt;
		prev[twoBajt + i] = -1;
	}
}

void headSwap(std::vector<long long>& head)
{
	unsigned short twoBajt = std::numeric_limits<unsigned short>::max();
	for (int i = 0; i < twoBajt; i++)
	{
		if (head[i] != -1)
		{
			head[i] -= twoBajt;
			if (head[i] < 0)
				head[i] = -1;
		}
	}
}

void literalFlagUpdate
(std::ofstream& ofile,unsigned char& flagBajt, int& bit_count, char sign)
{
	flagBajt <<= 1;
	bit_count++;
	ofile.put(sign);
}

void tokenFlagUpdate
(std::ofstream& ofile, unsigned char& flagBajt, int& bit_count, int i,
	const std::vector<Token>& tokenList, unsigned char max_dlugosc)
{
	flagBajt <<= 1;
	flagBajt |= 1;
	bit_count++;
	ofile.write(reinterpret_cast<const char*>(&tokenList[i].dystans), sizeof(tokenList[i].dystans));
	ofile.write(reinterpret_cast<const char*>(&tokenList[i].dlugosc), sizeof(tokenList[i].dlugosc));
}

void flagSwap(std::ofstream& ofile, long long& pos, unsigned char& flagBajt, int& bit_count)
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

unsigned short hashCalculation(const unsigned char c1, const unsigned char c2, const unsigned char c3)
{
	unsigned short twoBajt{};
	twoBajt |= c1;
	twoBajt <<= 8;
	unsigned short temp{};
	temp |= c2;
	twoBajt |= temp;
	temp = 0;
	temp |= c3;
	temp <<= 4;
	twoBajt ^= temp;
	return twoBajt;

}

void AlgorytmDekodujacyLZ77(const std::string& ipath, const std::string& opath)
{
	std::string fullText;

	std::ifstream ifile(ipath, std::ios::binary | std::ios::ate);	//otwieram plik na koncu, zeby szybko sprawdzic jego rozmiar
	std::ofstream ofile(opath, std::ios::binary);
	if(ifile.good() && ofile.good())
	{
		auto stream_size = ifile.tellg();
		ifile.seekg(0);					//przechodze na poczatek pliku
		long long size = (long long)stream_size;
		unsigned short twoBajt = std::numeric_limits<unsigned short>::max();//ustalam zakres mojego przesuwanego okna,
																			//bedzie wynosilo 2*twoBajt
		if(size> twoBajt)
		{
			fullText.resize(2*twoBajt);
			ifile.read(&fullText[0], 2*twoBajt);
		}
		else
		{
			fullText.resize(size);
			ifile.read(&fullText[0], size);
		}

		unsigned char flagBajt{};
		int bit_count = 8;

		int cursor{};			//cursor sluzy do poruszania sie w przesuwanym oknie, 
		long long file_cursor{};//file_cursor bedzie sluzyc do poruszania sie w pliku, na orginalnych danych
		std::string dekoded;	//buffor, do ktorego zapisuje dane, zanim zapisze je do pliku, plus  w nim bede sledzil historie
		bool first = true;		//flaga pierwszego przesuniecia okna

		while(file_cursor < size)
		{		
			if (bit_count == 8)	//pobranie nowej flagi bitowej, po wyczerpaniu poprzedniej
			{
				bit_count = 0;
				flagBajt = fullText[cursor];
				cursor++;
				readSwap(cursor, twoBajt, file_cursor, fullText, size, ifile);//funkcja wywolywana po kazdej aktualizacji cursor,
				file_cursor++;												//sprawdzajaca czy cursor nie wyszedl poza zakres
			}
			bool bit = (flagBajt & 128)!=0;	//wyciagniecie flagi bitowej z flagBajt
			flagBajt <<= 1;
			bit_count++;
			if (bit == 0)	//flaga 0 oznacza literal; obsluga literalu
			{
				dekoded.push_back(fullText[cursor]);
				dekodedSwap(dekoded, twoBajt, first, ofile);//sprawdzam, czy dekoded nie wyszedl poza zakres
				cursor += sizeof(unsigned char);
				readSwap(cursor, twoBajt, file_cursor, fullText, size, ifile);
				file_cursor += sizeof(unsigned char);
				continue;
			}
			else if (bit == 1)	//flaga 1 oznacza pare dystans,dlugosc
			{
				unsigned short dystans = *reinterpret_cast<const unsigned short*>(&fullText[cursor]);//wyluskanie dwoch bajtow informacji bezposrednio ze stringa
				cursor+=sizeof(unsigned short);
				readSwap(cursor, twoBajt, file_cursor, fullText, size, ifile);
				file_cursor += sizeof(unsigned short);
				unsigned char dlugosc = *reinterpret_cast<const unsigned char*>(&fullText[cursor]);//jak wyzej
				cursor++;
				readSwap(cursor, twoBajt, file_cursor, fullText, size, ifile);
				file_cursor += sizeof(unsigned char);

				if (dystans == 0 && dlugosc == 0)		//warunek zakonczenia programu
				{
					if(!first)								//wczytanie reszty danych w dwoch wariantach, gdy jestesmy przed przesunieciem
						ofile.write(&dekoded[twoBajt], dekoded.size() - twoBajt);	//okna, albo juz po pierwszym przesunieciu
					else
						ofile.write(&dekoded[0], dekoded.size());
					break;
				}

				if (dystans > dekoded.size())	//sprawdzenie poprawnosci danych
				{
					std::cout << "Blad odczytu danych" << std::endl;
					return;
				}
				int index = dekoded.size() - dystans;	//wprowadzam dane do dekoded, liczone od konca dekoded przez cala "dlugosc",
				for (int i = 0; i < dlugosc; i++)		//dane pobieram z samego dekoded
				{
					dekoded.push_back(dekoded[index + i]);
				}
				dekodedSwap(dekoded, twoBajt, first, ofile);//sprawdzam, czy dekoded nie wyszedl poza zakres
				continue;
			}
		}
	}
	else
	{
		std::cout << "Blad odczytu pliku przy dekodowaniu" << std::endl;
	}
}

//funkcja sluzaca do przesuniecia okna wczytanych danych, wywolywana przy kazdej aktualizacji cursor++
void readSwap(int& cursor, const unsigned short twoBajt,const long long& file_cursor, std::string& fullText,
	const long long& size, std::ifstream& ifile)
{
	const char safe_margin = 64;				//potrzebuje margines bledu, zeby nie przeciac informacji w polowie
	if (cursor + safe_margin >= 2 * twoBajt)
	{
		int rounded_margin = 2 * twoBajt - cursor;//margines bledu musi zaczac sie od pozycji kursora, a nie pozycji wyznaczonej 
		if (size - file_cursor >= 2 * twoBajt)	  //arbitralnie, w przeciwnym razie znowu moge otrzymac przeciete informacje
		{
			for (int i = 0; i < rounded_margin; i++)
				fullText[i] = fullText[fullText.size() - rounded_margin + i];//przenosze informacje z marginesu do poczatku nowego stringa
			fullText.resize(2 * twoBajt);
			ifile.read(&fullText[rounded_margin], 2 * twoBajt- rounded_margin);
			cursor = 0;
		}
		else
		{
			unsigned short lastBytes = static_cast<unsigned short>(size - file_cursor);//sytuacja, w ktorej do konca pliku zostalo mniej
			for (int i = 0; i < rounded_margin; i++)									//informacji niz twoBajt
				fullText[i] = fullText[fullText.size() - rounded_margin + i];
			fullText.resize(lastBytes);
			ifile.read(&fullText[rounded_margin], lastBytes- rounded_margin);
			cursor = 0;
		}
	}
}

//funkcja przesuwajaca i zapisujaca wczytane dane
void dekodedSwap(std::string& dekoded,const unsigned short twoBajt, bool& first, std::ofstream& ofile)
{
	if (dekoded.size() > 2 * twoBajt)
	{
		int size = dekoded.size();
		if (first)								//Obsluga pierwszego przejscia przez warunek glowny
		{
			first = false;
			ofile.write(&dekoded[0], size);
		}
		else
			ofile.write(&dekoded[twoBajt], size-twoBajt);	//zapisuje wszystko od polowy do konca
		for (int i = 0; i < twoBajt; i++)
			dekoded[i] = dekoded[size - twoBajt + i];		//przenosze wszystko, od polowy do konca, na poczatek
		dekoded.resize(twoBajt);
	}
}
