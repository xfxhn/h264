#include "BitStream.h"
#include <iostream>

using namespace std;


maping_coded_block_pattern intra_cbp[48] =
{
	{0,  47,   0},
	{1,  31,   16},
	{2,  15,   1},
	{3,  0,   2},
	{4,  23,   4},
	{5,  27,   8},
	{6,  29,   32},
	{7,  30,   3},
	{8,  7,   5},
	{9,  11,   10},
	{10,  13,   12},
	{11,  14,   15},
	{12,  39,   47},
	{13,  43,   7},
	{14,  45,   11},
	{15,  46,   13},
	{16,  16,   14},
	{17,  3,   6},
	{18,  5,   9},
	{19,  10,   31},
	{20,  12,   35},
	{21,  19,   37},
	{22,  21,   42},
	{23,  26,   44},
	{24,  28,   33},
	{25,  35,   34},
	{26,  37,   36},
	{27,  42,   40},
	{28,  44,   39},
	{29,  1,   43},
	{30,  2,   45},
	{31,  4,   46},
	{32,  8,   17},
	{33,  17,   18},
	{34,  18,   20},
	{35,  20,   24},
	{36,  24,   19},
	{37,  6,   21},
	{38,  9,   26},
	{39,  22,   28},
	{40,  25,   23},
	{41,  32,   27},
	{42,  33,   29},
	{43,  34,   30},
	{44,  36,   22},
	{45,  40,   25},
	{46,  38,   38},
	{47,  41,   41}
};

maping_coded_block_pattern_equal0 intra_cbp_equal0[16] =
{
	{0,  15,   0},
	{1,  0,   1},
	{2,  7,   2},
	{3,  11,   4},
	{4,  13,   8},
	{5,  14,   3},
	{6,  3,   5},
	{7,  5,   10},
	{8,  10,   12},
	{9,  12,   15},
	{10,  1,   7},
	{11,  2,   11},
	{12,  4,   13},
	{13,  8,   14},
	{14,  6,   6},
	{15,  9,   9},
};



BitStream::BitStream(uint8_t* buf, int _size)
{
	start = buf;
	currentPtr = buf;
	endPtr = buf + _size - 1;
	size = _size;
	postion = 0;
}

int BitStream::readBit()
{
	--bitsLeft;
	int result = ((*currentPtr) >> bitsLeft) & 1;

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
	for (size_t i = 0; i < n; ++i) {
		//��ǰnλ�Ƶ�����nλ��
		result = result | (readBit() << (n - i - 1));
	}
	return result;
}

int BitStream::getMultiBit(int n)
{

	int ret = 0;

	uint8_t* tempPtr = currentPtr;
	size_t tempBitsLeft = bitsLeft;
	uint8_t tempPostion = postion;
	ret = readMultiBit(n);
	currentPtr = tempPtr;
	bitsLeft = tempBitsLeft;
	postion = tempPostion;
	return ret;
}

int BitStream::readUE()
{
	int result = 0;
	size_t i = 0;
	while ((readBit() == 0) && (i < 32)) {
		i++;
	}
	result = readMultiBit(i);
	//��Ϊ����readBit�����һλ���ѷָ��1����ԭ����λ����
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

	return (pow(-1, k + 1) * ceil((k / 2)));*/
}
int BitStream::readME(int ChromaArrayType, H264_MB_PART_PRED_MODE mode)
{
	int uev = readUE();
	if (ChromaArrayType == 1 || ChromaArrayType == 2)
	{
		if (mode == H264_MB_PART_PRED_MODE::Intra_4x4 || mode == H264_MB_PART_PRED_MODE::Intra_8x8)
		{
			return intra_cbp[uev].Intra_4x4_or_Intra_8x8;
		}
		else
		{
			return intra_cbp[uev].Inter;
		}

	}
	else
	{
		if (mode == H264_MB_PART_PRED_MODE::Intra_4x4 || mode == H264_MB_PART_PRED_MODE::Intra_8x8)
		{
			return intra_cbp_equal0[uev].Intra_4x4_or_Intra_8x8;
		}
		else
		{
			return intra_cbp_equal0[uev].Inter;
		}
	}

}

int BitStream::readTE(int range)
{
	if (range <= 0)
	{
		return 0;
	}
	else if (range == 1)
	{
		return !readBit();
	}
	else
	{
		return readUE();
	}
}

//�Ƿ������ݣ��з���true�����򷵻�false
bool BitStream::more_rbsp_data()
{

	if (currentPtr > endPtr
		|| (currentPtr == endPtr && bitsLeft == 0)
		)
	{
		return false;
	}

	uint8_t* end = endPtr;
	while (end > currentPtr && *end == 0) //�Ӻ���ǰ�ң�ֱ���ҵ���һ����0ֵ�ֽ�λ��Ϊֹ
	{
		end--;
	}


	if (end > currentPtr)
	{
		return true; //˵����ǰλ��bs.m_p���滹����������
	}
	else
	{
		bool flag = true;
		int i = 0;
		for (i = 0; i < 8; i++) //�ڵ����ֽڵ�8������λ�У��Ӻ���ǰ�ң��ҵ�rbsp_stop_one_bitλ��
		{
			int v = ((*(currentPtr)) >> i) & 0x01;
			if (v == 1)
			{
				i++;
				flag = true;
				break;
			}
		}

		if (flag && i < bitsLeft)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
	//bool flag = true;

	//if (postion + 1 == size)
	//{
	//	size_t i = 0;
	//	for (; i < 8; i++)
	//	{
	//		if (((*currentPtr >> i) & 1) != 0)
	//		{
	//			flag = (i != 7);
	//			//++i;
	//			break;
	//		}
	//	}
	//	flag = (flag && (bitsLeft > i));

	//}

	//return flag;
}

BitStream::~BitStream()
{
}
