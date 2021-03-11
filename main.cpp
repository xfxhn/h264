#include "AnnexBReader.h"
#include "BitStream.h"
//#include <cstdlib>
//#include <iostream>
//#include "AnnexBReader.h"
//using namespace std;
// NALUµ¥Ôª


int main()
{

	/*const uint32_t size = 1024 * 1024;
	int* a = new int[20];

	FILE* f = fopen("./demo.h264", "rb");
	if (f == nullptr) {
		return false;
	}

	uint8_t* buffer = new uint8_t[size];
	memset(buffer, 0, size);
	uint32_t count;

	
	while ((count = fread(buffer, 1, size, f)) > 0)
	{
		cout << count << endl;
		
		if (count < size)
			break;

	} 


	cout << _msize(buffer) << endl;
	cout << sizeof(buffer) << endl;*/


	/*AnnexBReader reader;

	bool isOpen = reader.open("./demo.h264");*/



	uint8_t* a = new uint8_t[5];

	/*for (size_t i = 0; i < 5; i++)
	{
		a[i] = 10 + i;
	}*/

	a[0] = 9;
	a[1] = 33;


	BitStream bit(a, 5);


	//uint16_t b= bit.readMultiBit(4);

	/*for (size_t i = 0; i < 8; i++)
	{
		int b = bit.readBit();
		cout << b << endl;
	}*/
	int b = bit.readUE();
	//int b = bit.readMultiBit(4);
	cout << b << endl;

;	return EXIT_SUCCESS;
}


