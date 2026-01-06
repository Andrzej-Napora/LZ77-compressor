#include <iostream>
#include <string>
#include <map>
#include <iomanip>
#include <vector>
#include "../LZ77/Header.h"

using namespace std;

int main()
{
	string ipath = "../resources/proba.txt";
	string binOpath = "../resources/Binary.txt";
	string binIpath = "../resources/test.txt";

	AlgorytmKodujacyLZ77(ipath, binOpath);
	AlgorytmDekodujacyLZ77(binOpath, binIpath);


}