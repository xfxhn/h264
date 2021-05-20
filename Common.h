#pragma once
#include <cstdint>
#include <iostream>
#include "BitStream.h"
#include <cassert>
#include "NaluType.h" 

//无效的值用”na”表示
constexpr auto NA = -1;


using namespace std;



inline void printError(const char* err) {

	cout << err << endl;
}

inline bool isInterMode(H264_MB_PART_PRED_MODE mode)
{
	return (mode == H264_MB_PART_PRED_MODE::Intra_4x4) || (mode == H264_MB_PART_PRED_MODE::Intra_8x8) || (mode == H264_MB_PART_PRED_MODE::Intra_16x16);
}


string printHex(unsigned char* data, int len);



int h264_log2(int32_t value);


int scaling_list(BitStream& bs, int32_t* scalingList, int sizeOfScalingList, int32_t& useDefaultScalingMatrixFlag);

//填充
bool byte_aligned(BitStream& bs);

bool more_rbsp_data(BitStream& bs);

