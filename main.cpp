//#include "AnnexBReader.h"
using namespace std;
#include <cstdlib>
#include <iostream>
#include "AnnexBReader.h"

// NALUµ¥Ôª

typedef struct _NaluUnit
{
	int type;
	int size;
	uint8_t* data;
}NaluUnit;

int main()
{
	AnnexBReader reader;

	bool isOpen = reader.open("./demo.h264");

	/*NaluUnit bbb[1024];

	uint8_t data[3] = { 22,44,'\0'};

	NaluUnit aaa;
	
	uint8_t* ccc = new uint8_t[3];

	memcpy(ccc,&data,3);


	aaa.data = ccc;
	bbb[0] = aaa;

	cout << aaa.data[3] << endl;*/
;	return EXIT_SUCCESS;
}


