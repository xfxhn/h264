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


//宏块类型mb_type
//宏块类型表示的是宏块不同的分割和编码方式，在h.264的语法结构中，
//宏块类型在宏块层（macroblock_layer）中用mb_type表示（请参考h.264语法结构分析中的macroblock_layer）。
//而mb_type是与当前宏块所在的slice相关的，相同数值的mb_type在不同类型的slice中表示的类型会不一样。
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

