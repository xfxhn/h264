#pragma once
#include "Common.h"
#include "Cabac.h"
#include "SliceHeader.h"
#include "Macroblock.h"

class ParseSlice;
class SliceData
{
public:
	uint32_t  cabac_alignment_one_bit;
	uint32_t  mb_skip_run;
	uint32_t  CurrMbAddr;
	//��ʶ��֡������Ӧģʽ�У��ú��ʹ��֡���뻹�ǳ����롣��������ģ������ͨ���Ա����Ϸ����ڿ��mb_field_decoding_flagֵȡ0��1��2��
	uint32_t  mb_field_decoding_flag; //2 u(1) | ae(v)    0-a frame macroblock pair; 1-a field macroblock pair;
	uint32_t  mb_skip_flag;  //2 ae(v)


public:

	SliceData();
	int NextMbAddress(SliceHeader* sHeader, uint32_t n);
	bool slice_data(BitStream& bs, ParseSlice* Slice);
	~SliceData();
};

