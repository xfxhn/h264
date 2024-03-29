#pragma once
#include "Common.h"
#include "Cabac.h"
#include "SliceHeader.h"
#include "Macroblock.h"
#include "DPB.h"

class ParseSlice;
class SliceData
{
public:
	uint32_t  cabac_alignment_one_bit;
	uint32_t  mb_skip_run;
	uint32_t  CurrMbAddr;
	//标识在帧场自适应模式中，该宏块使用帧编码还是场编码。其上下文模型索引通过对比左、上方相邻块的mb_field_decoding_flag值取0、1或2；
	bool  mb_field_decoding_flag; //2 u(1) | ae(v)    0-a frame macroblock pair; 1-a field macroblock pair;
	bool  mb_skip_flag;  //2 ae(v)
	bool end_of_slice_flag;

public:

	SliceData();
	int NextMbAddress(const SliceHeader& sHeader, uint32_t n);
	bool slice_data(BitStream& bs, ParseSlice* Slice, DPB& bpb, int nal_cnt);
	~SliceData();
};

