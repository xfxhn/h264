#pragma once
#include <cstdint>
#include <iostream>
#include "BitStream.h"
#include <cassert>
#include "NaluType.h" 

//��Ч��ֵ�á�na����ʾ
constexpr auto NA = -1;


using namespace std;

void rbsp_trailing_bits(BitStream& bs);

//inline bool isMbUsable(int mbAddr, int CurrMbAddr, int CurrMbAddrSliceNumber, int mbAddrSliceNumber)
//{
//	//��ַΪmbAddr�ĺ��͵�ַΪCurrMbAddr�ĺ�����ڲ�ͬ������,����Ȳ����жϣ���û����֤��ȷ��
//	if (mbAddr < 0 || mbAddr > CurrMbAddr || CurrMbAddrSliceNumber != mbAddrSliceNumber) {
//		return true;
//	}
//	return false;
//}


inline void printError(const char* err) {

	cout << err << endl;
}

inline int Clip3(int x, int y, int z)
{
	if (z < x)
	{
		return x;
	}
	else if (z > y)
	{
		return y;
	}
	else
	{
		return z;
	}
}

inline int InverseRasterScan(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e)
{
	int ret = 0;

	if (e == 0)
	{
		ret = (a % (d / b)) * b;
	}
	else //if (e == 1)
	{
		ret = (a / (d / b)) * c;
	}
	return ret;
}



//Direct ��ʾֱ��Ԥ��ģʽ
inline bool isInterMode(H264_MB_PART_PRED_MODE mode)
{
	return (mode == H264_MB_PART_PRED_MODE::Intra_4x4) || (mode == H264_MB_PART_PRED_MODE::Intra_8x8) || (mode == H264_MB_PART_PRED_MODE::Intra_16x16);
}
inline bool isInterframe(H264_MB_PART_PRED_MODE mode)
{
	return (mode == H264_MB_PART_PRED_MODE::Pred_L0) || (mode == H264_MB_PART_PRED_MODE::Pred_L1) || (mode == H264_MB_PART_PRED_MODE::BiPred);
}

string printHex(unsigned char* data, int len);



int h264_log2(int32_t value);


void scaling_list(BitStream& bs, int32_t* scalingList, int sizeOfScalingList, bool& useDefaultScalingMatrixFlag);

//���
bool byte_aligned(BitStream& bs);


