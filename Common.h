#pragma once
#pragma warning(disable:4996)
#include <cstdint>
#include <iostream>
#include "BitStream.h"
#include <cassert>
#include "NaluType.h" 





//无效的值用”na”表示
constexpr auto NA = -1;


using namespace std;

void rbsp_trailing_bits(BitStream& bs);

//inline bool isMbUsable(int mbAddr, int CurrMbAddr, int CurrMbAddrSliceNumber, int mbAddrSliceNumber)
//{
//	//地址为mbAddr的宏块和地址为CurrMbAddr的宏块属于不同的条带,这个先不做判断，还没有验证正确性
//	if (mbAddr < 0 || mbAddr > CurrMbAddr || CurrMbAddrSliceNumber != mbAddrSliceNumber) {
//		return true;
//	}
//	return false;
//}


inline void printError(const char* err) {

	std::cout << err << std::endl;
}
inline int Median(int x, int y, int z)
{
	return x + y + z - std::min(x, std::min(y, z)) - std::max(x, std::max(y, z));
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

inline int InverseRasterScan(int a, int b, int c, int d, int e)
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



//Direct 表示直接预测模式
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

//填充
bool byte_aligned(BitStream& bs);


