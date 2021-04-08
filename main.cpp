#include "AnnexBReader.h"

//#include <cstdlib>
//#include <iostream>
//#include "AnnexBReader.h"
//using namespace std;
// NALUµ¥Ôª



int main()
{
	/*AnnexBReader aaa;
	 


	uint8_t* data = new uint8_t[22]; 
	uint8_t bb[] = { 0x67, 0x64, 0x00, 0x00, 0x03, 0x01, 0x41, 0x70, 0xC6, 0x84, 0x00, 0x00, 0x03, 0x03, 0x00, 0x03, 0xA9, 0x80, 0x3C, 0x58, 0xB6, 0x58 };

	for (size_t i = 0; i < 22; i++)
	{
		data[i] = bb[i];
	}
	for (size_t i = 0; i < 22; i++)
	{
		cout << (int)data[i]<<"," ;
	}
	cout << endl;

	uint8_t* ccc = nullptr;
	size_t size = aaa.unescape(data, 22);

	for (size_t i = 0; i < size; i++)
	{
		cout << (int)data[i] << ",";
	}
	cout << endl;
	cout << size << endl;*/
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


	AnnexBReader reader;

	bool isOpen = reader.open("./test.264");

;	return EXIT_SUCCESS;
}


