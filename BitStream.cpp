#include "BitStream.h"




BitStream::BitStream(uint8_t* buf, int _size)
{
	start = buf;
	currentPtr = buf;
	size = _size;
	postion = 0;
}

uint16_t BitStream::readBit()
{
	--bitsLeft;
	uint16_t result = (*currentPtr >> bitsLeft) & 1;

	if (bitsLeft == 0) {
		currentPtr++;
		postion++;
		bitsLeft = 8;
	}
	return result;
}

int BitStream::readMultiBit(int n)
{
	int result = 0;
	for (size_t i = 0; i < n; i++) {
		//把前n位移到后面n位来
		result = result | (readBit() << (n - i - 1));
	}
	return result;
}

int BitStream::readUE()
{
	int result = 0;
	size_t i = 0;
	while ((readBit() == 0) && (i < 32)) {
		i++;
	}
	result = readMultiBit(i);
	//因为上面readBit多读了一位，把分割的1放在原来的位置上
	result += (1 << i) - 1;
	return result;
}

int BitStream::readSE()
{
	int r = readUE();
	if (r & 0x01) {
		r = (r + 1) / 2;
	}
	else {
		r = -(r / 2);
	}
	return r;
	/*int k = readUE();

	return (pow(-1, k + 1) * ceil((double)(k / 2)));*/
}

bool BitStream::isEmpty()
{
	bool flag = true;
	if (postion == size)
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (((*currentPtr >> i) & 1) != 0)
			{
				flag = (i != 7);
			}
		}
	}

	return flag;
}

BitStream::~BitStream()
{
}
