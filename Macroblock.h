#pragma once
#include "Common.h"
#include "ResidualBlockCavlc.h"
#include "Cabac.h"


class SliceData;
class Picture;
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

struct SUB_MB_TYPE_MBS_P
{
	uint8_t					sub_mb_type;
	H264_MB_TYPE			name;
	uint8_t					NumSubMbPart;
	H264_MB_PART_PRED_MODE  SubMbPredMode;
	uint8_t                 SubMbPartWidth;
	uint8_t                 SubMbPartHeight;
};


struct SUB_MB_TYPE_MBS_B
{
	uint8_t					sub_mb_type;
	H264_MB_TYPE			name;
	uint8_t					NumSubMbPart;
	H264_MB_PART_PRED_MODE  SubMbPredMode;
	uint8_t                 SubMbPartWidth;
	uint8_t                 SubMbPartHeight;
};

//宏块类型mb_type
//宏块类型表示的是宏块不同的分割和编码方式，在h.264的语法结构中，
//宏块类型在宏块层（macroblock_layer）中用mb_type表示（请参考h.264语法结构分析中的macroblock_layer）。
//而mb_type是与当前宏块所在的slice相关的，相同数值的mb_type在不同类型的slice中表示的类型会不一样。
class ParseSlice;
class Macroblock
{
public:
	Picture* currRefPtr;


	uint8_t			mb_type;
	uint8_t			sub_mb_type[4];
	H264_MB_TYPE    mbType;
	H264_MB_TYPE    subMbType[4];
	H264_MB_PART_PRED_MODE mode;//当前宏块的预测模式
	H264_MB_PART_PRED_MODE subMode[4];//子宏块的预测模式

	uint8_t			NumMbPart;
	uint8_t         NumSubMbPart[4];
	uint8_t			MbPartWidth;
	uint8_t			MbPartHeight;
	uint8_t			SubMbPartWidth[4];
	uint8_t			SubMbPartHeight[4];

	uint32_t		pcm_alignment_zero_bit; // 3 f(1)
	uint32_t		pcm_sample_luma[256]; //3 u(v)
	uint32_t* pcm_sample_chroma;
	bool			transform_size_8x8_flag;



	uint8_t     ref_idx_l0[4];
	uint8_t     ref_idx_l1[4];
	int     mvd_l0[4][4][2];
	int     mvd_l1[4][4][2];



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



	SLIECETYPE   fix_slice_type;
	uint8_t		 fix_mb_type;

	bool TransformBypassModeFlag;


	uint8_t Intra4x4PredMode[16];//存储当前宏块的16个4x4子宏块预测模式的值，范围[0,8]
	uint8_t Intra8x8PredMode[4];//存储当前宏块的4个8x8子宏块预测模式的值，范围[0,8]
	uint8_t Intra16x16PredMode;//存储当前宏块的1个16x16宏块预测模式的值，范围[0,4]

	uint8_t lumaPredSamples[16][4][4]; //存储16*16亮度块预测样点值
	uint8_t luma8x8PredSamples[4][8][8]; //存储8*8亮度块预测样点值
	uint8_t luma16x16PredSamples[16][16]; //存储16*16亮度块预测样点值
	uint8_t chromaPredSamples[8][16]; //存储色度块预测样点值  //最大422 8*16

	//uint8_t predL[16][16];  //存储亮度预测样点值



	uint32_t sliceNumber;

	int8_t     FilterOffsetA;
	int8_t     FilterOffsetB;


	int  mb_skip_flag;

	uint8_t     coded_block_flag_DC_pattern; //3个bit位表示CABAC残差相应4x4子宏块中的DC直流系数block的coded_block_flag值，(b7,...,b2,b1,b0)=(x,...,cr,cb,luma)
	uint16_t    coded_block_flag_AC_pattern[3]; //16个bit位表示CABAC残差相应4x4子宏块中的AC交流系数block的coded_block_flag值(0或1)(全部默认为1)，[0]-luma,[1]-cb,[2]-cr
	int                  predFlagL0[4];//predFlagL0和predFlagL1分别代表两个方向的refIdx是否为非负值
	int                  predFlagL1[4];//最多4种宏块分割，子宏块也是
	int                  mvL0[4][4][2];//两个方向的运动矢量mvL0和mvL1
	int                  mvL1[4][4][2];
	int                  refIdxL0[4];
	int                  refIdxL1[4];
public:
	Macroblock();
	bool macroblock_layer(BitStream& bs, ParseSlice* Slice, SliceData* slice_data, Cabac& cabac, int nal_cnt);
	bool macroblock_layer_skip(ParseSlice* Slice, SliceData* slice_data);


	static H264_MB_PART_PRED_MODE getMbPartPredMode(SLIECETYPE fix_slice_type, uint8_t fix_mb_type, bool transform_size_8x8_flag, uint8_t mbPartIdx);
	~Macroblock();
private:

	H264_MB_PART_PRED_MODE MbPartPredMode(uint8_t mbPartIdx);

	int fixed_mb_type(uint32_t slice_type, uint8_t& fix_mb_type, SLIECETYPE& fix_slice_type);

	bool mb_pred(BitStream& bs, Cabac& cabac, int nal_cnt);

	bool residual(BitStream& bs, int startIdx, int endIdx, Cabac& cabac);
	bool sub_mb_pred(BitStream& bs, Cabac& cabac, int nal_cnt);


	int residual_luma(BitStream& bs, int i16x16DClevel[16], int i16x16AClevel[16][16], int level4x4[16][16],
		int level8x8[4][64], int startIdx, int endIdx, Cabac& cabac);

	//void getChromaQuantisationParameters();
private:
	bool isAe;


	ParseSlice* sliceBase;
};

