#pragma once
#include "Common.h"
#include "ParsePPS.h"
#include "SliceHeader.h"


struct MB_TYPE_SLICES_I
{
	uint8_t					mb_type;
	H264_MB_TYPE			name;
	bool					transform_size_8x8_flag;
	H264_MB_PART_PRED_MODE  MbPartPredMode;
	int32_t					Intra16x16PredMode;
	int32_t					CodedBlockPatternChroma;
	int32_t					CodedBlockPatternLuma;
};

struct MB_TYPE_SLICES_SP_P
{
	uint8_t					mb_type;
	H264_MB_TYPE			name;
	uint8_t					NumMbPart;
	H264_MB_PART_PRED_MODE  MbPartPredMode0;
	H264_MB_PART_PRED_MODE	MbPartPredMode1;
	uint8_t					MbPartWidth;
	uint8_t					MbPartHeight;
};



struct MB_TYPE_P_SP_SLICES
{
	uint8_t				mb_type;
	H264_MB_TYPE_P_SP   name;
	uint8_t             NumMbPart;
};


//�������mb_type
//������ͱ�ʾ���Ǻ�鲻ͬ�ķָ�ͱ��뷽ʽ����h.264���﷨�ṹ�У�
//��������ں��㣨macroblock_layer������mb_type��ʾ����ο�h.264�﷨�ṹ�����е�macroblock_layer����
//��mb_type���뵱ǰ������ڵ�slice��صģ���ͬ��ֵ��mb_type�ڲ�ͬ���͵�slice�б�ʾ�����ͻ᲻һ����
class Macroblock
{
public:
	/*uint8_t* data;
	size_t bitOffset;
	ParsePPS& pps;*/


	uint8_t  pcm_alignment_zero_bit; // 3 f(1)
	uint8_t  pcm_sample_luma[256]; //3 u(v)
	uint8_t* pcm_sample_chroma;
	bool     transform_size_8x8_flag;
	uint8_t  mb_type;

public:
	Macroblock();
	bool macroblock_layer(BitStream& bs, const SliceHeader& sHeader);

	H264_MB_PART_PRED_MODE MbPartPredMode(uint8_t mb_type, int transform_size_8x8_flag, uint8_t slice_type, bool flag);

	int NumMbPart(uint8_t mb_type, uint8_t slice_type);

	~Macroblock();
};

