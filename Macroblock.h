#pragma once
#include "Common.h"
#include "ResidualBlockCavlc.h"



struct MB_TYPE_SLICES_I
{
	uint8_t					mb_type;
	H264_MB_TYPE			name;
	bool					transform_size_8x8_flag;
	H264_MB_PART_PRED_MODE  MbPartPredMode;
	uint8_t					Intra16x16PredMode;
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

struct MB_TYPE_SLICES_B
{
	uint8_t					mb_type;
	H264_MB_TYPE			name;
	uint8_t					NumMbPart;
	H264_MB_PART_PRED_MODE  MbPartPredMode0;
	H264_MB_PART_PRED_MODE	MbPartPredMode1;
	uint8_t					MbPartWidth;
	uint8_t					MbPartHeight;
};

struct MB_TYPE_SLICES_SI
{
	uint8_t					mb_type;
	H264_MB_TYPE			name;
	H264_MB_PART_PRED_MODE  MbPartPredMode;
	uint8_t                 Intra16x16PredMode;
	uint8_t                 CodedBlockPatternChroma;
	uint8_t                 CodedBlockPatternLuma;
};




//宏块类型mb_type
//宏块类型表示的是宏块不同的分割和编码方式，在h.264的语法结构中，
//宏块类型在宏块层（macroblock_layer）中用mb_type表示（请参考h.264语法结构分析中的macroblock_layer）。
//而mb_type是与当前宏块所在的slice相关的，相同数值的mb_type在不同类型的slice中表示的类型会不一样。
class ParseSlice;
class Macroblock
{
public:

	H264_MB_TYPE    mbType;
	uint32_t		pcm_alignment_zero_bit; // 3 f(1)
	uint32_t		pcm_sample_luma[256]; //3 u(v)
	uint32_t* pcm_sample_chroma;
	bool			transform_size_8x8_flag;
	uint32_t		mb_type;




	uint32_t		coded_block_pattern;
	uint32_t		CodedBlockPatternLuma;
	uint32_t		CodedBlockPatternChroma;

	uint32_t		mb_qp_delta;

	int             QPY;
	int				QP1Y;

	int QP1C;
	int QPC;


	int QSY;
	int QSC;



	/*int QPCb;
	int QP1Cb;
	int QPCr;
	int QP1Cr;*/

	uint32_t		prev_intra4x4_pred_mode_flag[16];
	uint32_t		rem_intra4x4_pred_mode[16];

	uint32_t		prev_intra8x8_pred_mode_flag[4];
	uint32_t		rem_intra8x8_pred_mode[4];

	uint32_t		intra_chroma_pred_mode;



	int     i16x16DClevel[16]; //DCT变换后的直流系数
	int     i16x16AClevel[16][16]; //DCT变换后的交流系数

	int     level4x4[16][16];     //存储亮度的残差数据
	int     level8x8[4][64];



	int     ChromaDCLevel[2][16]; //存储DC色度u和v的残差数据
	int     ChromaACLevel[2][16][15]; //存储AC色度u和v的残差数据


	uint8_t     mb_luma_4x4_non_zero_count_coeff[16];//存储4x4亮度宏块非0系数
	uint8_t     mb_luma_8x8_non_zero_count_coeff[4];//存储8x8亮度宏块非0系数
	uint8_t     mb_chroma_4x4_non_zero_count_coeff[2][16];//存储色度宏块非0系数


	H264_MB_PART_PRED_MODE mode;//当前宏块的预测模式
	SLIECETYPE   fix_slice_type;


	bool TransformBypassModeFlag;


	uint8_t Intra4x4PredMode[16];//存储当前宏块的16个4x4子宏块预测模式的值，范围[0,8]
	uint8_t Intra16x16PredMode;//存储当前宏块的1个16x16宏块预测模式的值，范围[0,4]

	uint8_t lumaPredSamples[16][4][4]; //存储字块亮度预测样点值
	uint8_t luma16x16PredSamples[16][16]; //存储16*16亮度块预测样点值
	uint8_t chromaPredSamples[8][16]; //存储色度块预测样点值  //最大422 8*16

	uint8_t predL[16][16];  //存储亮度预测样点值


public:
	Macroblock(ParseSlice& slice);
	bool macroblock_layer(BitStream& bs);





	~Macroblock();
private:
	H264_MB_PART_PRED_MODE MbPartPredMode(uint32_t mb_type, SLIECETYPE slice_type, uint32_t mbPartIdx);
	H264_MB_PART_PRED_MODE MbPartPredMode2(uint32_t mb_type, SLIECETYPE slice_type, uint32_t mbPartIdx);
	int fixed_mb_type(uint32_t slice_type, uint32_t& fix_mb_type, SLIECETYPE& fix_slice_type);
	bool is_I_NxN(uint32_t mb_type, SLIECETYPE slice_type);
	bool mb_pred(BitStream& bs, uint32_t mb_type, uint32_t numMbPart);
	bool residual(BitStream& bs, int startIdx, int endIdx);
	bool sub_mb_pred(uint32_t mb_type);
	uint32_t NumMbPart(uint32_t mb_type, SLIECETYPE slice_type);


	int residual_luma(BitStream& bs, int i16x16DClevel[16], int i16x16AClevel[16][16], int level4x4[16][16],
		int level8x8[4][64], int startIdx, int endIdx);

	//void getChromaQuantisationParameters();
private:
	bool isAe;


	ParseSlice& sliceBase;
};

