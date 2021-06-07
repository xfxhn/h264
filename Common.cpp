
#include "Common.h"






string printHex(unsigned char* data, int len)
{


	for (size_t i = 0; i < len; i++)
	{
		cout << hex << (int)data[i] << "-";
	}
	cout << endl;

	return "";
}

int h264_log2(int32_t value)
{
	assert(value > 0);
	int log2 = 0;
	while (value)
	{
		value >>= 1;
		log2++;
	}
	return log2;
}

int scaling_list(BitStream& bs, int32_t* scalingList, int sizeOfScalingList, int32_t& useDefaultScalingMatrixFlag)
{

	int lastScale = 8;
	int nextScale = 8;

	for (size_t j = 0; j < sizeOfScalingList; j++)
	{
		if (nextScale != 0)
		{
			int delta_scale = bs.readSE();

			nextScale = (lastScale + delta_scale + 256) % 256;
			useDefaultScalingMatrixFlag = (j == 0 && nextScale == 0);

		}
		scalingList[j] = (nextScale == 0) ? lastScale : nextScale;

		lastScale = scalingList[j];

	}
	return 0;
}

bool byte_aligned(BitStream& bs)
{
	if (bs.bitsLeft % 8 == 0)
	{
		return true;
	}
	return false;
}

bool more_rbsp_data(BitStream& bs)
{

	cout << bs.currentPtr << endl;


	return false;
}


