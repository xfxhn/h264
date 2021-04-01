
#include "Common.h"






string printHex(unsigned char* data,int len)
{


	for (size_t i = 0; i < len; i++)
	{
		cout << hex << (int)data[i]<< "-";
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
	return 0;
}


