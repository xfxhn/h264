

#include "Macroblock.h"
#include "ParseSlice.h"
#include "SliceData.h"

MB_TYPE_SLICES_I mb_type_slices_I[27] =
{
	{ 0,     H264_MB_TYPE::I_NxN,            0,     H264_MB_PART_PRED_MODE::Intra_4x4,      NA,  NA,   NA },
	{ 0,     H264_MB_TYPE::I_NxN,            1,     H264_MB_PART_PRED_MODE::Intra_8x8,      NA,  NA,   NA },
	{ 1,     H264_MB_TYPE::I_16x16_0_0_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    0,    0,    0 },
	{ 2,     H264_MB_TYPE::I_16x16_1_0_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    1,    0,    0 },
	{ 3,     H264_MB_TYPE::I_16x16_2_0_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    2,    0,    0 },
	{ 4,     H264_MB_TYPE::I_16x16_3_0_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    3,    0,    0 },
	{ 5,     H264_MB_TYPE::I_16x16_0_1_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    0,    1,    0 },
	{ 6,     H264_MB_TYPE::I_16x16_1_1_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    1,    1,    0 },
	{ 7,     H264_MB_TYPE::I_16x16_2_1_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    2,    1,    0 },
	{ 8,     H264_MB_TYPE::I_16x16_3_1_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    3,    1,    0 },
	{ 9,     H264_MB_TYPE::I_16x16_0_2_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    0,    2,    0 },
	{ 10,    H264_MB_TYPE::I_16x16_1_2_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    1,    2,    0 },
	{ 11,    H264_MB_TYPE::I_16x16_2_2_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    2,    2,    0 },
	{ 12,    H264_MB_TYPE::I_16x16_3_2_0,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    3,    2,    0 },
	{ 13,    H264_MB_TYPE::I_16x16_0_0_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    0,    0,    15 },
	{ 14,    H264_MB_TYPE::I_16x16_1_0_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    1,    0,    15 },
	{ 15,    H264_MB_TYPE::I_16x16_2_0_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    2,    0,    15 },
	{ 16,    H264_MB_TYPE::I_16x16_3_0_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    3,    0,    15 },
	{ 17,    H264_MB_TYPE::I_16x16_0_1_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    0,    1,    15 },
	{ 18,    H264_MB_TYPE::I_16x16_1_1_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    1,    1,    15 },
	{ 19,    H264_MB_TYPE::I_16x16_2_1_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    2,    1,    15 },
	{ 20,    H264_MB_TYPE::I_16x16_3_1_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    3,    1,    15 },
	{ 21,    H264_MB_TYPE::I_16x16_0_2_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    0,    2,    15 },
	{ 22,    H264_MB_TYPE::I_16x16_1_2_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    1,    2,    15 },
	{ 23,    H264_MB_TYPE::I_16x16_2_2_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    2,    2,    15 },
	{ 24,    H264_MB_TYPE::I_16x16_3_2_1,    NA,    H264_MB_PART_PRED_MODE::Intra_16x16,    3,    2,    15 },
	{ 25,    H264_MB_TYPE::I_PCM,            NA,    H264_MB_PART_PRED_MODE::NA,				NA,   NA,   NA }
};
MB_TYPE_SLICES_SP_P mb_type_sleces_sp_p[6] =
{
	{0,		H264_MB_TYPE::P_L0_16x16,		1,	H264_MB_PART_PRED_MODE::Pred_L0,	H264_MB_PART_PRED_MODE::NA,		  16,  16},
	{1,		H264_MB_TYPE::P_L0_L0_16x8,		2,	H264_MB_PART_PRED_MODE::Pred_L0,	H264_MB_PART_PRED_MODE::Pred_L0,  16,  8},
	{2,		H264_MB_TYPE::P_L0_L0_8x16,		2,	H264_MB_PART_PRED_MODE::Pred_L0,	H264_MB_PART_PRED_MODE::Pred_L0,  8,   16},
	{3,		H264_MB_TYPE::P_8x8,			4,	H264_MB_PART_PRED_MODE::NA,			H264_MB_PART_PRED_MODE::NA,		  8,   8},
	{4,		H264_MB_TYPE::P_8x8ref0,		4,	H264_MB_PART_PRED_MODE::NA,			H264_MB_PART_PRED_MODE::NA,		  8,   8},
	{5,		H264_MB_TYPE::P_Skip,			1,	H264_MB_PART_PRED_MODE::Pred_L0,	H264_MB_PART_PRED_MODE::NA,		  16,   16}
};

MB_TYPE_SLICES_B mb_type_sleces_b[24] =
{
	{ 0,     H264_MB_TYPE::B_Direct_16x16,   NA,   H264_MB_PART_PRED_MODE::Direct,     H264_MB_PART_PRED_MODE::NA,		   8,     8 },
	{ 1,     H264_MB_TYPE::B_L0_16x16,       1,    H264_MB_PART_PRED_MODE::Pred_L0,    H264_MB_PART_PRED_MODE::NA,         16,    16 },
	{ 2,     H264_MB_TYPE::B_L1_16x16,       1,    H264_MB_PART_PRED_MODE::Pred_L1,    H264_MB_PART_PRED_MODE::NA,         16,    16 },
	{ 3,     H264_MB_TYPE::B_Bi_16x16,       1,    H264_MB_PART_PRED_MODE::BiPred,     H264_MB_PART_PRED_MODE::NA,         16,    16 },
	{ 4,     H264_MB_TYPE::B_L0_L0_16x8,     2,    H264_MB_PART_PRED_MODE::Pred_L0,    H264_MB_PART_PRED_MODE::Pred_L0,    16,    8 },
	{ 5,     H264_MB_TYPE::B_L0_L0_8x16,     2,    H264_MB_PART_PRED_MODE::Pred_L0,    H264_MB_PART_PRED_MODE::Pred_L0,    8,     16 },
	{ 6,     H264_MB_TYPE::B_L1_L1_16x8,     2,    H264_MB_PART_PRED_MODE::Pred_L1,    H264_MB_PART_PRED_MODE::Pred_L1,    16,    8 },
	{ 7,     H264_MB_TYPE::B_L1_L1_8x16,     2,    H264_MB_PART_PRED_MODE::Pred_L1,    H264_MB_PART_PRED_MODE::Pred_L1,    8,     16 },
	{ 8,     H264_MB_TYPE::B_L0_L1_16x8,     2,    H264_MB_PART_PRED_MODE::Pred_L0,    H264_MB_PART_PRED_MODE::Pred_L1,    16,    8 },
	{ 9,     H264_MB_TYPE::B_L0_L1_8x16,     2,    H264_MB_PART_PRED_MODE::Pred_L0,    H264_MB_PART_PRED_MODE::Pred_L1,    8,     16 },
	{ 10,    H264_MB_TYPE::B_L1_L0_16x8,     2,    H264_MB_PART_PRED_MODE::Pred_L1,    H264_MB_PART_PRED_MODE::Pred_L0,    16,    8 },
	{ 11,    H264_MB_TYPE::B_L1_L0_8x16,     2,    H264_MB_PART_PRED_MODE::Pred_L1,    H264_MB_PART_PRED_MODE::Pred_L0,    8,     16 },
	{ 12,    H264_MB_TYPE::B_L0_Bi_16x8,     2,    H264_MB_PART_PRED_MODE::Pred_L0,    H264_MB_PART_PRED_MODE::BiPred,     16,    8 },
	{ 13,    H264_MB_TYPE::B_L0_Bi_8x16,     2,    H264_MB_PART_PRED_MODE::Pred_L0,    H264_MB_PART_PRED_MODE::BiPred,     8,     16 },
	{ 14,    H264_MB_TYPE::B_L1_Bi_16x8,     2,    H264_MB_PART_PRED_MODE::Pred_L1,    H264_MB_PART_PRED_MODE::BiPred,     16,    8 },
	{ 15,    H264_MB_TYPE::B_L1_Bi_8x16,     2,    H264_MB_PART_PRED_MODE::Pred_L1,    H264_MB_PART_PRED_MODE::BiPred,     8,     16 },
	{ 16,    H264_MB_TYPE::B_Bi_L0_16x8,     2,    H264_MB_PART_PRED_MODE::BiPred,     H264_MB_PART_PRED_MODE::Pred_L0,    16,    8 },
	{ 17,    H264_MB_TYPE::B_Bi_L0_8x16,     2,    H264_MB_PART_PRED_MODE::BiPred,     H264_MB_PART_PRED_MODE::Pred_L0,    8,     16 },
	{ 18,    H264_MB_TYPE::B_Bi_L1_16x8,     2,    H264_MB_PART_PRED_MODE::BiPred,     H264_MB_PART_PRED_MODE::Pred_L1,    16,    8 },
	{ 19,    H264_MB_TYPE::B_Bi_L1_8x16,     2,    H264_MB_PART_PRED_MODE::BiPred,     H264_MB_PART_PRED_MODE::Pred_L1,    8,     16 },
	{ 20,    H264_MB_TYPE::B_Bi_Bi_16x8,     2,    H264_MB_PART_PRED_MODE::BiPred,     H264_MB_PART_PRED_MODE::BiPred,     16,    8 },
	{ 21,    H264_MB_TYPE::B_Bi_Bi_8x16,     2,    H264_MB_PART_PRED_MODE::BiPred,     H264_MB_PART_PRED_MODE::BiPred,     8,     16 },
	{ 22,    H264_MB_TYPE::B_8x8,            4,    H264_MB_PART_PRED_MODE::NA,		   H264_MB_PART_PRED_MODE::NA,         8,     8 },
	{ 23,    H264_MB_TYPE::B_Skip,           NA,   H264_MB_PART_PRED_MODE::Direct,     H264_MB_PART_PRED_MODE::NA,         8,     8 }
};
MB_TYPE_SLICES_SI mb_type_sleces_si[1] =
{
	{ 0,     H264_MB_TYPE::SI,     H264_MB_PART_PRED_MODE::Intra_4x4,    NA,    NA,    NA }
};
SUB_MB_TYPE_MBS_P sub_mb_type_mbs_p[4] =
{
	{ 0,    H264_MB_TYPE::P_L0_8x8,    1,    H264_MB_PART_PRED_MODE::Pred_L0,    8,    8 },
	{ 1,    H264_MB_TYPE::P_L0_8x4,    2,    H264_MB_PART_PRED_MODE::Pred_L0,    8,    4 },
	{ 2,    H264_MB_TYPE::P_L0_4x8,    2,    H264_MB_PART_PRED_MODE::Pred_L0,    4,    8 },
	{ 3,    H264_MB_TYPE::P_L0_4x4,    4,    H264_MB_PART_PRED_MODE::Pred_L0,    4,    4 }
};


SUB_MB_TYPE_MBS_B sub_mb_type_mbs_b[13] =
{
		{ 0,     H264_MB_TYPE::B_Direct_8x8,    4,    H264_MB_PART_PRED_MODE::Direct,     4,    4 },
		{ 1,     H264_MB_TYPE::B_L0_8x8,        1,    H264_MB_PART_PRED_MODE::Pred_L0,    8,    8 },
		{ 2,     H264_MB_TYPE::B_L1_8x8,        1,    H264_MB_PART_PRED_MODE::Pred_L1,    8,    8 },
		{ 3,     H264_MB_TYPE::B_Bi_8x8,        1,    H264_MB_PART_PRED_MODE::BiPred,     8,    8 },
		{ 4,     H264_MB_TYPE::B_L0_8x4,        2,    H264_MB_PART_PRED_MODE::Pred_L0,    8,    4 },
		{ 5,     H264_MB_TYPE::B_L0_4x8,        2,    H264_MB_PART_PRED_MODE::Pred_L0,    4,    8 },
		{ 6,     H264_MB_TYPE::B_L1_8x4,        2,    H264_MB_PART_PRED_MODE::Pred_L1,    8,    4 },
		{ 7,     H264_MB_TYPE::B_L1_4x8,        2,    H264_MB_PART_PRED_MODE::Pred_L1,    4,    8 },
		{ 8,     H264_MB_TYPE::B_Bi_8x4,        2,    H264_MB_PART_PRED_MODE::BiPred,     8,    4 },
		{ 9,     H264_MB_TYPE::B_Bi_4x8,        2,    H264_MB_PART_PRED_MODE::BiPred,     4,    8 },
		{ 10,    H264_MB_TYPE::B_L0_4x4,        4,    H264_MB_PART_PRED_MODE::Pred_L0,    4,    4 },
		{ 11,    H264_MB_TYPE::B_L1_4x4,        4,    H264_MB_PART_PRED_MODE::Pred_L1,    4,    4 },
		{ 12,    H264_MB_TYPE::B_Bi_4x4,        4,    H264_MB_PART_PRED_MODE::BiPred,     4,    4 }
};

Macroblock::Macroblock()
{
	sliceBase = nullptr;
	pcm_alignment_zero_bit = 0;
	transform_size_8x8_flag = 0;
	memset(pcm_sample_luma, 0, sizeof(uint8_t) * 256);
	pcm_sample_chroma = nullptr;

	mb_type = 0;
	memset(sub_mb_type, 0, sizeof(uint8_t) * 4);
	mbType = H264_MB_TYPE::NA;
	memset(subMbType, 0, sizeof(H264_MB_TYPE) * 4);
	mode = H264_MB_PART_PRED_MODE::NA;
	memset(subMode, 0, sizeof(H264_MB_PART_PRED_MODE) * 4);

	NumMbPart = 0;
	memset(NumSubMbPart, 0, sizeof(uint8_t) * 4);
	MbPartWidth = 0;
	MbPartHeight = 0;
	memset(SubMbPartWidth, 0, sizeof(uint8_t) * 4);
	memset(SubMbPartHeight, 0, sizeof(uint8_t) * 4);

	coded_block_pattern = 0;

	CodedBlockPatternChroma = -1;
	CodedBlockPatternLuma = -1;
	mb_qp_delta = 0;


	isAe = false;

	fix_mb_type = 0;
	fix_slice_type = SLIECETYPE::H264_SLIECE_TYPE_I;


	memset(prev_intra4x4_pred_mode_flag, 0, sizeof(uint8_t) * 16);
	memset(rem_intra4x4_pred_mode, 0, sizeof(uint8_t) * 16);
	memset(prev_intra8x8_pred_mode_flag, 0, sizeof(uint8_t) * 4);
	memset(rem_intra8x8_pred_mode, 0, sizeof(uint8_t) * 4);




	memset(mb_luma_4x4_non_zero_count_coeff, 0, sizeof(uint8_t) * 16);
	memset(mb_luma_8x8_non_zero_count_coeff, 0, sizeof(uint8_t) * 4);


	//预测模式
	memset(Intra4x4PredMode, 0, sizeof(uint8_t) * 16);
	memset(Intra8x8PredMode, 0, sizeof(uint8_t) * 4);
	Intra16x16PredMode = 0;


	//样点预测值
	memset(lumaPredSamples, 0, sizeof(uint8_t) * 16 * 4 * 4);
	memset(luma8x8PredSamples, 0, sizeof(uint8_t) * 4 * 8 * 8);
	memset(luma16x16PredSamples, 0, sizeof(uint8_t) * 16 * 16);
	//色度样点预测值
	memset(chromaPredSamples, 0, sizeof(uint8_t) * 8 * 16);



	intra_chroma_pred_mode = 0;
	QPY = 0;
	QP1Y = 0;

	QP1C = 0;
	QPC = 0;


	QSY = 0;
	QSC = 0;



	sliceNumber = 0;

	mb_skip_flag = 0;


	FilterOffsetA = 0;
	FilterOffsetB = 0;

	//coded_block_flag_DC_pattern = 0x07;
	//coded_block_flag_AC_pattern[0] = 0xFFFF; //cabac: coded_block_flag-luma
	//coded_block_flag_AC_pattern[1] = 0xFFFF; //cabac: coded_block_flag-cb
	//coded_block_flag_AC_pattern[2] = 0xFFFF; //cabac: coded_block_flag-cr

	coded_block_flag_DC_pattern = 0;
	coded_block_flag_AC_pattern[0] = 0; //cabac: coded_block_flag-luma
	coded_block_flag_AC_pattern[1] = 0; //cabac: coded_block_flag-cb
	coded_block_flag_AC_pattern[2] = 0; //cabac: coded_block_flag-cr


	memset(ref_idx_l0, 0, sizeof(uint8_t) * 4);
	memset(ref_idx_l1, 0, sizeof(uint8_t) * 4);
	memset(mvd_l0, 0, sizeof(int) * 4 * 4 * 2);
	memset(mvd_l1, 0, sizeof(int) * 4 * 4 * 2);



}


//slice type 五种类型
//I slice中的宏块类型只能是I宏块类型
//P slice中包含了I宏块类型,与P宏块类型
//B slice中包含了I宏块类型,与B宏块类型
//sp slice中包含了I宏块类型,与p宏块类型
//si 仅包含si宏块
//Intra16x16PredMode  如果当前宏块类型采用的预测方式为Intra_16x16，那么该字段有效，它用0~3表示了Intra_16x16中的四种模式


//CodedBlockPatternLuma  如果当前宏块类型采用的预测方式为Intra_16x16，那么该字段有效，它表示了Luma宏块中的CBP。
//从h.264语法结构分析的residual部分，我们知道当预测模式为Intra_16x16时，宏块需要分开AC level与DC level进行熵编码。
//0：表示宏块内的16个4x4块中的AC level全部都为0，15：宏块内的16个4x4块中至少有一个块的AC level不全为0



//CodedBlockPatternChroma :如果当前宏块类型采用的预测方式为Intra_16x16，那么该字段有效,它表示了Luma宏块中的CBP
//
//一个宏块的色度分量的coded_block_pattern，Cb、Cr的CodedBlockPatternChroma相同
bool Macroblock::macroblock_layer(BitStream& bs, ParseSlice* Slice, SliceData* sliceData, Cabac& cabac)
{

	sliceBase = Slice;

	SliceHeader& sHeader = sliceBase->sHeader;

	sliceNumber = sliceBase->sliceNumber;

	FilterOffsetA = sHeader.FilterOffsetA;
	FilterOffsetB = sHeader.FilterOffsetB;
	mb_skip_flag = sliceData->mb_skip_flag;

	isAe = sHeader.pps.entropy_coding_mode_flag;  //ae(v)表示CABAC编码


	if (isAe) // ae(v) 表示CABAC编码
	{
		mb_type = cabac.decode_mb_type(bs, Slice);
	}
	else // ue(v) 表示CAVLC编码
	{
		//确定该 MB 是帧内或帧间（P 或 B）编码模式，确定该 MB 分割的尺寸
		mb_type = bs.readUE(); //2 ue(v) | ae(v)
	}



	uint8_t	 slice_type = sHeader.slice_type;

	//修正过后的
	fix_mb_type = mb_type;

	//当前宏块是什么类型   //修正过后的
	fix_slice_type = (SLIECETYPE)(slice_type);



	//修正mb_type，因为p,SI里可能包含了i宏块
	fixed_mb_type(slice_type, fix_mb_type, fix_slice_type);


	//获取当前宏块的预测模式
	mode = MbPartPredMode(0);

	/*uint8_t  numMbPart = NumMbPart(fix_mb_type, fix_slice_type);*/

	if (mbType == H264_MB_TYPE::I_PCM)  //I_PCM 不经过预测，变换，量化
	{
		while (!byte_aligned(bs))
		{
			/*当熵编码模式是CABAC时,
			  此时要求数据字节对齐, 即数据从下一个字节的第一个比特开始,
			  如果还没有字节对齐将出现若干个 pcm_alignment_zero_bit 作为填充。*/
			pcm_alignment_zero_bit = bs.readBit(); //2 f(1)
		}

		for (size_t i = 0; i < 256; i++)
		{
			int v = sHeader.sps.BitDepthY;
			/*pcm_sample_luma[i]是一个样点值。第一个 pcm_sample_luma[i]值代表宏块里光栅扫描中的亮度样点值。
			  比特的数目通常代表这些样点每一个都是BitDepthY 。
			  当 profile_idc 不等于 100, 110, 122 或 144 时， pcm_sample_luma[i]不能等于0*/
			pcm_sample_luma[i] = bs.readMultiBit(v); //3 u(v)
		}

		//MbWidthC   MbHeightC色度阵列的宽度和高度
		pcm_sample_chroma = new uint32_t[2 * sHeader.sps.MbWidthC * sHeader.sps.MbHeightC]();
		for (size_t i = 0; i < 2 * sHeader.sps.MbWidthC * sHeader.sps.MbHeightC; i++)
		{
			int v = sHeader.sps.BitDepthC;
			/*pcm_sample_ chroma[i]是一个样点值。色度
			第一个 MbWidthC* MbHeightC pcm_sample_ chroma[i]值代表宏块里光栅扫描中的Cb样点值且其余的MbWidthC* MbHeightC
			pcm_sample_chroma[i]值代表宏块里光栅扫描中 的 Cr 样点值。比特的数目通常代表这些样点每一个都是 BitDepthC
			当 profile_idc 不等于 100, 110, 122 或 144 时，pcm_sample_ chroma[i]不能等于0。
			*/
			pcm_sample_chroma[i] = bs.readMultiBit(v); //3 u(v)
		}
	}
	else
	{

		bool noSubMbPartSizeLessThan8x8Flag = true;
		//子宏块  //（只对 8×8MB 分割的帧内 MB）确定每一子宏块的子宏块分割，每一宏块分割的表 0 和 / 或表1的参考图象；每一宏块子分割的差分编码运动矢量。
		if (mbType != H264_MB_TYPE::I_NxN &&
			mode != H264_MB_PART_PRED_MODE::Intra_16x16 &&
			NumMbPart == 4)
		{
			sub_mb_pred(bs, cabac);
			for (int mbPartIdx = 0; mbPartIdx < 4; mbPartIdx++)
			{
				if (subMbType[mbPartIdx] != H264_MB_TYPE::B_Direct_8x8)
				{
					/*int NumSubMbPart = 0;
					Macroblock::getSubMbPredMode(subMbType[mbPartIdx], NumSubMbPart);*/
					if (NumSubMbPart[mbPartIdx] > 1)
					{
						noSubMbPartSizeLessThan8x8Flag = false;
					}
				}
				else if (!sHeader.sps.direct_8x8_inference_flag)
				{
					noSubMbPartSizeLessThan8x8Flag = false;

				}
			}
		}
		else
		{
			//8x8解码
			if (sHeader.pps.transform_8x8_mode_flag && mbType == H264_MB_TYPE::I_NxN)
			{

				//使用8x8变换解码
				if (isAe)
				{
					transform_size_8x8_flag = cabac.decode_transform_size_8x8_flag(bs, Slice);
				}
				else
				{
					/*等于1表示对于当前宏块，亮度样点中变换系数解码过程和图像构建过程的调用，
					  应在残余的 8x8 块的去块效应滤波过程之前完成。transform_size_8x8_flag 等于 0 表示对于当前宏块，
					  亮度样点当中变换系数解码过程和图像构建过程的调用在剩余的4x4块的去块效应滤波过程之前完成。
					  如果 transform_size_8x8_flag 在比特流中不存在，则默认其值为0。*/
					transform_size_8x8_flag = bs.readBit();
				}

				mode = MbPartPredMode(0);
			}
			if (sliceBase->CurrMbAddr >= 8)
			{
				int a = 0;
			}
			mb_pred(bs, cabac);

		}



		if (mode != H264_MB_PART_PRED_MODE::Intra_16x16)
		{
			if (isAe)
			{

				coded_block_pattern = cabac.decode_coded_block_pattern(bs, Slice);
			}
			else
			{
				//宏块预测模式等于 Intra_4x4, Intra_8x8 或者  Inter 时，coded_block_pattern 的值不同
				coded_block_pattern = bs.readME(sHeader.sps.ChromaArrayType, mode);

			}
			//公式7-33
			//1、8x8 亮度块中的四个 4x4 亮度块的所有变换系数幅值都等于 0 👉残差（DC、AC）全部不编码
			//2、8x8 亮度块中的一个或多个 4x4 亮度块的一个或多个变换系数幅值不为 0👉残差（DC、AC）全部编码
			CodedBlockPatternLuma = coded_block_pattern % 16;

			/*CodedBlockPatternChroma
				描述
				0		所有色度变换系数幅值都等于 0。
				1		一个或多个色度   DC 变换系数幅值不为0。所有色度AC变换系数幅值都等 于 0。
				2		零个或多个色度   DC 变换系数幅值不为0。一个或多个色度AC变换系数幅 值不为 0。*/
			CodedBlockPatternChroma = coded_block_pattern / 16;

			if (
				CodedBlockPatternLuma > 0 &&
				sHeader.pps.transform_8x8_mode_flag &&
				mbType != H264_MB_TYPE::I_NxN &&
				noSubMbPartSizeLessThan8x8Flag &&
				(mbType != H264_MB_TYPE::B_Direct_16x16 || sHeader.sps.direct_8x8_inference_flag)
				)
			{
				if (isAe)
				{
					transform_size_8x8_flag = cabac.decode_transform_size_8x8_flag(bs, Slice);
				}
				else
				{
					transform_size_8x8_flag = bs.readBit();
				}
				mode = MbPartPredMode(0);

			}
		}

		//16x16
		if (CodedBlockPatternLuma > 0 || CodedBlockPatternChroma > 0 || mode == H264_MB_PART_PRED_MODE::Intra_16x16)
		{
			if (isAe)
			{
				mb_qp_delta = cabac.decode_mb_qp_delta(bs, Slice);
			}
			else
			{
				//能改变宏块层 中 QPY 的值 。mb_qp_delta 解码后的值应包含在–( 26 + QpBdOffsetY / 2) 到 +( 25 + QpBdOffsetY / 2 )范围内。
				//当任何宏块（包括 P_Skip 和 B_Skip 宏块类型）中都不存在 mb_qp_delta 时， mb_qp_delta 默认为0
				//量化参数的改变值。
				mb_qp_delta = bs.readSE();
			}

			residual(bs, 0, 15, cabac);
		}
	}



	//QPY = ( ( QPY,PREV + mb_qp_delta + 52 + 2 * QpBdOffsetY ) % ( 52 + QpBdOffsetY ) ) －QpBdOffsetY
	//QpBdOffsetY  亮度偏移
	QPY = ((sHeader.QPY_prev + mb_qp_delta + 52 + 2 * sHeader.sps.QpBdOffsetY) % (52 + sHeader.sps.QpBdOffsetY)) - sHeader.sps.QpBdOffsetY;
	//对于条带中的第一个宏块，QPY_prev 的初始值为SliceQPY,QPY_prev是当前条带中按解码顺序排列的前一个宏块的亮度量化参数 QPY
	sHeader.QPY_prev = QPY;

	QP1Y = QPY + sHeader.sps.QpBdOffsetY;

	//只有HP模式下才有qpprime_y_zero_transform_bypass_flag这个变量
	//qpprime_y_zero_transform_bypass_flag 等于 1 是指当 QP1Y 等于 0 时变换系数解码过程的变换旁路操作和图像构建过程将会在第8.5 节给出的去块效应滤波过程之前执行
	if (sHeader.sps.qpprime_y_zero_transform_bypass_flag && QP1Y == 0)
	{
		TransformBypassModeFlag = true;
	}
	else
	{
		TransformBypassModeFlag = false;
	}

	return false;
}

bool Macroblock::macroblock_layer_skip(ParseSlice* Slice, SliceData* slice_data)
{
	sliceBase = Slice;
	SliceHeader& sHeader = Slice->sHeader;
	sliceNumber = sliceBase->sliceNumber;

	FilterOffsetA = sHeader.FilterOffsetA;
	FilterOffsetB = sHeader.FilterOffsetB;
	mb_skip_flag = slice_data->mb_skip_flag;

	isAe = Slice->sHeader.pps.entropy_coding_mode_flag;
	SLIECETYPE sliceTtpy = (SLIECETYPE)(Slice->sHeader.slice_type);

	if (sliceTtpy == SLIECETYPE::H264_SLIECE_TYPE_P || sliceTtpy == SLIECETYPE::H264_SLIECE_TYPE_SP)
	{
		mb_type = 5; //inferred: P_Skip
	}
	else if (sliceTtpy == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		mb_type = 23; //inferred: B_Skip
	}


	//修正过后的
	fix_mb_type = mb_type;

	//当前宏块是什么类型   //修正过后的
	fix_slice_type = (SLIECETYPE)(sliceTtpy);

	MbPartPredMode(0);

	mb_qp_delta = 0;

	QPY = ((sHeader.QPY_prev + mb_qp_delta + 52 + 2 * sHeader.sps.QpBdOffsetY) % (52 + sHeader.sps.QpBdOffsetY)) - sHeader.sps.QpBdOffsetY; // (7-37)
	sHeader.QPY_prev = QPY;
	QP1Y = QPY + sHeader.sps.QpBdOffsetY;


	if (sHeader.sps.qpprime_y_zero_transform_bypass_flag && QP1Y == 0)
	{
		TransformBypassModeFlag = true;
	}
	else
	{
		TransformBypassModeFlag = false;
	}
	return false;
}


//宏块预测语法
bool Macroblock::mb_pred(BitStream& bs, Cabac& cabac)
{
	const SliceHeader& sHeader = sliceBase->sHeader;
	//Direct 直接预测：一种不用解码运动矢量的块的帧间预测模式。针对空域预测和时域预测又两种直接预测模式。
	if (mode == H264_MB_PART_PRED_MODE::Intra_4x4 || mode == H264_MB_PART_PRED_MODE::Intra_8x8 || mode == H264_MB_PART_PRED_MODE::Intra_16x16)
	{

		if (mode == H264_MB_PART_PRED_MODE::Intra_4x4)
		{
			for (size_t luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++)
			{

				//表示序号为 luma4x4BlkIdx = 0到15 的4x4 亮度块的帧内Intra_4x4 预测。
				if (isAe) // ae(v) 表示CABAC编码
				{
					prev_intra4x4_pred_mode_flag[luma4x4BlkIdx] = cabac.decode_prev_intra4x4_pred_mode_flag_or_prev_intra8x8_pred_mode_flag(bs);
				}
				else
				{

					//表示帧内预测模式预测标识。如果该标识位为1，表示帧内预测模式的预测值就是实际的模式，否则就需要另外传递实际的帧内预测模式。
					prev_intra4x4_pred_mode_flag[luma4x4BlkIdx] = bs.readBit();
				}

				if (!prev_intra4x4_pred_mode_flag[luma4x4BlkIdx]) {
					if (isAe) // ae(v) 表示CABAC编码
					{
						rem_intra4x4_pred_mode[luma4x4BlkIdx] = cabac.decode_rem_intra4x4_pred_mode_or_rem_intra8x8_pred_mode(bs);
					}
					else
					{
						//有9种预测模式
						/*模式0：垂直模式，条件：A~D可用。
						  模式1：水平模式，条件：I~L可用。
						  模式2：DC模式，条件：A~D或I~L可用。
						  模式3~8：方向模式，各个像素是由A到L像素通过权重不等的公式加权计算的。*/
						  //表示额外传递的实际帧内预测模式。
						rem_intra4x4_pred_mode[luma4x4BlkIdx] = bs.readMultiBit(3);
					}

				}
			}
		}


		if (mode == H264_MB_PART_PRED_MODE::Intra_8x8)
		{
			for (size_t luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++) {
				//表示序号为 luma8x8BlkIdx = 0到3 的8x8 亮度块的Intra_8x8 预测
				if (isAe) // ae(v) 表示CABAC编码
				{
					prev_intra8x8_pred_mode_flag[luma8x8BlkIdx] = cabac.decode_prev_intra4x4_pred_mode_flag_or_prev_intra8x8_pred_mode_flag(bs);
				}
				else
				{
					prev_intra8x8_pred_mode_flag[luma8x8BlkIdx] = bs.readBit();
				}

				if (!prev_intra8x8_pred_mode_flag[luma8x8BlkIdx]) {
					if (isAe) // ae(v) 表示CABAC编码
					{
						rem_intra8x8_pred_mode[luma8x8BlkIdx] = cabac.decode_rem_intra4x4_pred_mode_or_rem_intra8x8_pred_mode(bs);
					}
					else
					{
						rem_intra8x8_pred_mode[luma8x8BlkIdx] = bs.readMultiBit(3);
					}

				}
			}
		}


		// YUV 4 : 2 : 0 || YUV 4 : 2 : 2，yuv一起编码
		if (sHeader.sps.ChromaArrayType == 1 || sHeader.sps.ChromaArrayType == 2)
		{
			//宏块中用于色度的空间预测类型使用Intra_4x4 或 Intra_16x16 预测
			//intra_chroma_pred_mode 的取值范围为0 到  3。
			//  0 = DC 又被称之为均值模式。均值模式下，4 x 4 子块中 16 个像素都是相同的值，
			//         是上方的四个像素 A B C D 和左边四个像素 I J K L 的均值。
			//	1 = 水平的
			//	2 = 垂直的
			//	3 = 平面的
			if (isAe) // ae(v) 表示CABAC编码
			{
				intra_chroma_pred_mode = cabac.decode_intra_chroma_pred_mode(bs, sliceBase);
			}
			else
			{
				intra_chroma_pred_mode = bs.readUE();
			}
		}
	}
	else if (mode != H264_MB_PART_PRED_MODE::Direct)
	{

		for (size_t mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			//ref_idx_l0[ mbPartIdx ]   用参考帧队列 L0 进行预测，即前向预测时
			//不支持帧场自适应变量mb_field_decoding_flag=false
			bool mb_field_decoding_flag = false;
			if ((sHeader.num_ref_idx_l0_active_minus1 > 0 || false)//(mb_field_decoding_flag)
				&& MbPartPredMode(mbPartIdx) != H264_MB_PART_PRED_MODE::Pred_L1
				)
			{
				if (isAe)
				{
					bool is_ref_idx_l0 = true;
					cabac.decode_ref_idx_lX(sliceBase, bs, mbPartIdx, is_ref_idx_l0, ref_idx_l0[mbPartIdx]);
				}
				else
				{

					//sHeader.num_ref_idx_l1_active_minus1 - 1;
					//ref_idx_l0[ mbPartIdx ]的取值范围将是0到num_ref_idx_l0_active_minus
					ref_idx_l0[mbPartIdx] = bs.readTE(sHeader.num_ref_idx_l0_active_minus1);
				}
			}
		}
		//这个句法元素用于参考帧队列 L1，即后向预测
		for (size_t mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			if ((sHeader.num_ref_idx_l1_active_minus1 > 0 || false) && MbPartPredMode(mbPartIdx) != H264_MB_PART_PRED_MODE::Pred_L0)
			{
				if (isAe)
				{
					bool is_ref_idx_l0 = false;
					cabac.decode_ref_idx_lX(sliceBase, bs, mbPartIdx, is_ref_idx_l0, ref_idx_l1[mbPartIdx]);
				}
				else
				{
					ref_idx_l1[mbPartIdx] = bs.readTE(sHeader.num_ref_idx_l1_active_minus1);
				}

			}
		}

		for (size_t mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			if (MbPartPredMode(mbPartIdx) != H264_MB_PART_PRED_MODE::Pred_L1)
			{
				for (size_t compIdx = 0; compIdx < 2; compIdx++)
				{
					if (isAe)
					{
						int mvd_flag = compIdx;
						int subMbPartIdx = 0;
						cabac.decode_mvd_lX(sliceBase, bs, mbPartIdx, subMbPartIdx, mvd_flag, mvd_l0[mbPartIdx][0][compIdx]);
					}
					else
					{
						mvd_l0[mbPartIdx][0][compIdx] = bs.readSE();
					}
				}
			}
		}



		for (size_t mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			if (MbPartPredMode(mbPartIdx) != H264_MB_PART_PRED_MODE::Pred_L0)
			{
				for (size_t compIdx = 0; compIdx < 2; compIdx++)
				{
					if (isAe)
					{
						int mvd_flag = 2 + compIdx;
						int subMbPartIdx = 0;
						cabac.decode_mvd_lX(sliceBase, bs, mbPartIdx, subMbPartIdx, mvd_flag, mvd_l1[mbPartIdx][0][compIdx]);
					}
					else
					{
						mvd_l1[mbPartIdx][0][compIdx] = bs.readSE();
					}
				}
			}
		}

	}
	return false;
}
//获得当前宏块类型所采用的Intra预测方式
H264_MB_PART_PRED_MODE Macroblock::MbPartPredMode(uint8_t mbPartIdx)
{
	H264_MB_PART_PRED_MODE mode = H264_MB_PART_PRED_MODE::NA;

	if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_I)
	{
		//mb_type == I_NxN
		if (fix_mb_type == 0) {
			if (transform_size_8x8_flag) {
				mode = mb_type_slices_I[1].MbPartPredMode;
				mbType = mb_type_slices_I[1].name;
			}
			else {
				mode = mb_type_slices_I[0].MbPartPredMode;
				mbType = mb_type_slices_I[1].name;
			}
		}
		else if (fix_mb_type == 25)
		{
			mode = mb_type_slices_I[fix_mb_type].MbPartPredMode;  //na 
			mbType = mb_type_slices_I[fix_mb_type].name;
		}
		else {
			mode = mb_type_slices_I[fix_mb_type + 1].MbPartPredMode;
			CodedBlockPatternLuma = mb_type_slices_I[fix_mb_type + 1].CodedBlockPatternLuma;
			CodedBlockPatternChroma = mb_type_slices_I[fix_mb_type + 1].CodedBlockPatternChroma;
			Intra16x16PredMode = mb_type_slices_I[fix_mb_type + 1].Intra16x16PredMode;
			mbType = mb_type_slices_I[fix_mb_type + 1].name;
		}


	}
	else if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_P || fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP)
	{

		if (mbPartIdx == 1)
		{
			mode = mb_type_sleces_sp_p[fix_mb_type].MbPartPredMode1;
		}
		else {
			mode = mb_type_sleces_sp_p[fix_mb_type].MbPartPredMode0;
		}
		mbType = mb_type_sleces_sp_p[fix_mb_type].name;

		MbPartWidth = mb_type_sleces_sp_p[fix_mb_type].MbPartWidth;
		MbPartHeight = mb_type_sleces_sp_p[fix_mb_type].MbPartHeight;


		NumMbPart = mb_type_sleces_sp_p[mb_type].NumMbPart;
	}
	else if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		if (mbPartIdx == 1)
		{
			mode = mb_type_sleces_b[fix_mb_type].MbPartPredMode1;
		}
		else {
			mode = mb_type_sleces_b[fix_mb_type].MbPartPredMode0;
		}
		mbType = mb_type_sleces_b[fix_mb_type].name;
		MbPartWidth = mb_type_sleces_b[fix_mb_type].MbPartWidth;
		MbPartHeight = mb_type_sleces_b[fix_mb_type].MbPartHeight;

		NumMbPart = mb_type_sleces_b[mb_type].NumMbPart;
	}
	else if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		mode = mb_type_sleces_si[fix_mb_type].MbPartPredMode;
		mbType = mb_type_sleces_si[fix_mb_type].name;
	}
	return mode;
}

H264_MB_PART_PRED_MODE Macroblock::getMbPartPredMode(SLIECETYPE fix_slice_type, uint8_t fix_mb_type, bool transform_size_8x8_flag, uint8_t mbPartIdx)
{
	H264_MB_PART_PRED_MODE mode = H264_MB_PART_PRED_MODE::NA;

	if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_I)
	{
		//mb_type == I_NxN
		if (fix_mb_type == 0) {
			if (transform_size_8x8_flag) {
				mode = mb_type_slices_I[1].MbPartPredMode;
			}
			else {
				mode = mb_type_slices_I[0].MbPartPredMode;
			}
		}
		else if (fix_mb_type == 25)
		{
			mode = mb_type_slices_I[fix_mb_type].MbPartPredMode;  //na 
		}
		else {
			mode = mb_type_slices_I[fix_mb_type + 1].MbPartPredMode;
		}


	}
	else if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_P || fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP)
	{

		if (mbPartIdx == 1)
		{
			mode = mb_type_sleces_sp_p[fix_mb_type].MbPartPredMode1;
		}
		else {
			mode = mb_type_sleces_sp_p[fix_mb_type].MbPartPredMode0;
		}
	}
	else if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		if (mbPartIdx == 1)
		{
			mode = mb_type_sleces_b[fix_mb_type].MbPartPredMode1;
		}
		else {
			mode = mb_type_sleces_b[fix_mb_type].MbPartPredMode0;
		}
	}
	else if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		mode = mb_type_sleces_si[fix_mb_type].MbPartPredMode;
	}

	return mode;
}

//宏块被分割成多少部分
//uint8_t Macroblock::NumMbPart(uint32_t mb_type, SLIECETYPE slice_type)
//{
//	uint8_t ret = 0;
//	if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_P || slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP)
//	{
//		ret = mb_type_sleces_sp_p[mb_type].NumMbPart;
//	}
//	else if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
//	{
//		ret = mb_type_sleces_b[mb_type].NumMbPart;
//	}
//	return ret;
//}


//修正slice_type和mb_type
int Macroblock::fixed_mb_type(uint32_t slice_type, uint8_t& fix_mb_type, SLIECETYPE& fix_slice_type)
{

	if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		if (mb_type >= 1 && mb_type <= 26)//说明包含了i宏块
		{
			fix_mb_type = mb_type - 1;
			fix_slice_type = SLIECETYPE::H264_SLIECE_TYPE_I;  //说明包含了I宏块
		}
	}
	else if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_SP ||
		(SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_P)
	{
		if (mb_type >= 5 && mb_type <= 30)
		{
			fix_mb_type = mb_type - 5;
			fix_slice_type = SLIECETYPE::H264_SLIECE_TYPE_I;  //说明包含了I宏块
		}
	}
	else if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		if (mb_type >= 23 && mb_type <= 48)
		{
			fix_mb_type = mb_type - 23;
			fix_slice_type = SLIECETYPE::H264_SLIECE_TYPE_I;  //说明包含了I宏块
		}
	}


	return 0;
}


//计算残差数据
bool Macroblock::residual(BitStream& bs, int startIdx, int endIdx, Cabac& cabac)
{


	int TotalCoeff = 0;
	const SliceHeader& sHeader = sliceBase->sHeader;
	ResidualBlockCavlc residual_block(sliceBase);


	//解析量度
	residual_luma(bs, i16x16DClevel, i16x16AClevel, level4x4, level8x8, startIdx, endIdx, cabac);

	//chroma_format_idc = 0	单色
	//chroma_format_idc = 1	YUV 4 : 2 : 0
	//chroma_format_idc = 2	YUV 4 : 2 : 2
	//chroma_format_idc = 3	YUV 4 : 4 : 4

	//CodedBlockPatternChroma 只会有0,1,2三个值
	//解析色度
	if (sHeader.sps.ChromaArrayType == 1 || sHeader.sps.ChromaArrayType == 2)
	{
		TotalCoeff = 0;
		//sps.SubWidthC=2,  sHeader.sps.SubHeightC=1		//422
		//sps.SubWidthC=2,  sHeader.sps.SubHeightC=2		//420
		int NumC8x8 = 4 / (sHeader.sps.SubWidthC * sHeader.sps.SubHeightC);
		//420对应一个u，一个v
		for (size_t iCbCr = 0; iCbCr < 2; iCbCr++)
		{
			//有CodedBlockPatternChroma这个值才能进行解码  解码器把所有残差系数赋为0
			if ((CodedBlockPatternChroma & 3) && startIdx == 0)
			{

				if (isAe)
				{
					cabac.residual_block_cabac(bs, sliceBase, ChromaDCLevel[iCbCr], 0, 4 * NumC8x8 - 1, 4 * NumC8x8, RESIDUAL_LEVEL::ChromaDCLevel, TotalCoeff, 0, iCbCr);
				}
				else
				{
					//解码色度DC系数，这是必有的  DC是固定码表
					//420最大系数有4个   422最大系数有8个  444最大系数有16个   
					residual_block.residual_block_cavlc(bs, ChromaDCLevel[iCbCr], 0, 4 * NumC8x8 - 1, 4 * NumC8x8, TotalCoeff, RESIDUAL_LEVEL::ChromaDCLevel);
				}
				mb_chroma_4x4_non_zero_count_coeff[iCbCr][0] = TotalCoeff;
			}
			else
			{
				//所有残差都不被传送，解码器把所有残差系数赋为0。
				for (size_t i = 0; i < 4 * NumC8x8; i++)
				{
					ChromaDCLevel[iCbCr][i] = 0;
				}
			}
		}





		for (size_t iCbCr = 0; iCbCr < 2; iCbCr++)
		{
			//420是4个y对应一组uv，422是两个y对应一组uv，如果这里是420的就u和v各读一个,如果422的就各读两个 如果444的就各读四个，所以最多有16个
			for (size_t i8x8 = 0; i8x8 < NumC8x8; i8x8++)
			{
				for (size_t i4x4 = 0; i4x4 < 4; i4x4++)
				{
					const size_t BlkIdx = i8x8 * 4 + i4x4;//第几个块
					//AC系数解码
					if (CodedBlockPatternChroma & 2)
					{
						TotalCoeff = 0;
						if (isAe)
						{
							cabac.residual_block_cabac(bs, sliceBase, ChromaACLevel[iCbCr][BlkIdx], std::max(0, startIdx - 1), endIdx - 1, 15, RESIDUAL_LEVEL::ChromaACLevel, TotalCoeff, BlkIdx, iCbCr);
						}
						else
						{

							RESIDUAL_LEVEL level = iCbCr == 0 ? RESIDUAL_LEVEL::ChromaACLevelCb : RESIDUAL_LEVEL::ChromaACLevelCr;
							//能走到这里DC系数最少被取走了一个，所以这里最多15
							residual_block.residual_block_cavlc(bs, ChromaACLevel[iCbCr][BlkIdx], std::max(0, startIdx - 1), endIdx - 1, 15, TotalCoeff, level, BlkIdx);

						}
						//色度非0系数
						mb_chroma_4x4_non_zero_count_coeff[iCbCr][BlkIdx] = TotalCoeff;
					}
					else
					{
						//所有残差都不被传送，解码器把所有残差系数赋为0。
						for (size_t i = 0; i < 15; i++)
						{
							ChromaACLevel[iCbCr][BlkIdx][i] = 0;
						}

					}

				}
			}
		}



	}
	else if (sHeader.sps.ChromaArrayType == 3)
	{
		//residual_luma(bs, i16x16DClevel, i16x16AClevel, level4x4, level8x8, startIdx, endIdx);
	}
	return false;
}
//亮度块预测
int Macroblock::residual_luma(BitStream& bs, int i16x16DClevel[16], int i16x16AClevel[16][16], int level4x4[16][16], int level8x8[4][64], int startIdx, int endIdx, Cabac& cabac)
{
	ResidualBlockCavlc residual_block(sliceBase);
	const SliceHeader& sHeader = sliceBase->sHeader;
	int TotalCoeff = 0;

	//先解析16*16的DC直流分量
	if (startIdx == 0 && mode == H264_MB_PART_PRED_MODE::Intra_16x16)
	{
		if (isAe)
		{
			cabac.residual_block_cabac(bs, sliceBase, i16x16DClevel, 0, 15, 16, RESIDUAL_LEVEL::Intra16x16DCLevel, TotalCoeff, 0);
		}
		else
		{
			residual_block.residual_block_cavlc(bs, i16x16DClevel, 0, 15, 16, TotalCoeff, RESIDUAL_LEVEL::Intra16x16DCLevel);
		}
		mb_luma_4x4_non_zero_count_coeff[0] = TotalCoeff;
	}



	//先循环外面四个8x8
	for (size_t i8x8 = 0; i8x8 < 4; i8x8++)
	{

		if (!transform_size_8x8_flag || !isAe)//不是8x8解码或者是cavlc
		{
			//在循环里面四个4x4
			for (size_t i4x4 = 0; i4x4 < 4; i4x4++)
			{
				const size_t BlkIdx = i8x8 * 4 + i4x4;//第几个块


				//有亮度分量解析量度分析，4*4是不区分AC和DC
				if (CodedBlockPatternLuma & (1 << i8x8))
				{
					//解析16*16的AC
					if (mode == H264_MB_PART_PRED_MODE::Intra_16x16)
					{
						if (isAe)
						{
							cabac.residual_block_cabac(bs, sliceBase, i16x16AClevel[BlkIdx], max(0, startIdx - 1), endIdx - 1, 15, RESIDUAL_LEVEL::Intra16x16ACLevel, TotalCoeff, BlkIdx);
						}
						else
						{
							//解析16*16AC 交流分量 剩余的最多15个系数
							residual_block.residual_block_cavlc(bs, i16x16AClevel[BlkIdx], max(0, startIdx - 1), endIdx - 1, 15, TotalCoeff, RESIDUAL_LEVEL::Intra16x16ACLevel, BlkIdx);
						}

					}
					else
					{
						//4*4系数，也有可能8*8
						if (isAe)
						{
							cabac.residual_block_cabac(bs, sliceBase, level4x4[BlkIdx], startIdx, endIdx, 16, RESIDUAL_LEVEL::LumaLevel4x4, TotalCoeff, BlkIdx);
						}
						else
						{
							residual_block.residual_block_cavlc(bs, level4x4[BlkIdx], startIdx, endIdx, 16, TotalCoeff, RESIDUAL_LEVEL::LumaLevel4x4, BlkIdx);
						}
					}
					//存储当前4x4块非0系数数目
					mb_luma_4x4_non_zero_count_coeff[BlkIdx] = TotalCoeff;
				}
				else if (mode == H264_MB_PART_PRED_MODE::Intra_16x16)
				{
					//因为取走了一个直流分量所以最多15个
					for (size_t i = 0; i < 15; i++)
					{
						i16x16AClevel[BlkIdx][i] = 0;
					}

				}
				else
				{
					for (size_t i = 0; i < 16; i++)
					{
						level4x4[BlkIdx][i] = 0;
					}
				}

				//cavlc，并且8*8模式
				if (!sHeader.pps.entropy_coding_mode_flag && transform_size_8x8_flag)
				{
					for (size_t i = 0; i < 16; i++)
					{
						//直接把4*4解码的系数给level8x8
						level8x8[i8x8][4 * i + i4x4] = level4x4[i8x8 * 4 + i4x4][i];
					}
					//存储当前8x8块非0系数数目
					mb_luma_8x8_non_zero_count_coeff[i8x8] += mb_luma_4x4_non_zero_count_coeff[i8x8 * 4 + i4x4];
				}
			}

		}
		else if (CodedBlockPatternLuma & (1 << i8x8))
		{
			// 8*8 cabac
			cabac.residual_block_cabac(bs, sliceBase, level8x8[i8x8], 4 * startIdx, 4 * endIdx + 3, 64, RESIDUAL_LEVEL::LumaLevel8x8, TotalCoeff, i8x8);

			mb_luma_8x8_non_zero_count_coeff[i8x8] = TotalCoeff;
		}
		else
		{
			for (size_t i = 0; i < 64; i++)
			{
				level8x8[i8x8][i] = 0;
			}
		}
	}


	return 0;
}
bool Macroblock::sub_mb_pred(BitStream& bs, Cabac& cabac)
{

	for (size_t mbPartIdx = 0; mbPartIdx < 4; mbPartIdx++)
	{
		if (isAe)
		{
			sub_mb_type[mbPartIdx] = cabac.decode_sub_mb_type(bs, sliceBase);
		}
		else
		{
			sub_mb_type[mbPartIdx] = bs.readUE();
		}


		if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_P && sub_mb_type[mbPartIdx] >= 0 && sub_mb_type[mbPartIdx] <= 3)
		{
			subMbType[mbPartIdx] = sub_mb_type_mbs_p[sub_mb_type[mbPartIdx]].name;
			subMode[mbPartIdx] = sub_mb_type_mbs_p[sub_mb_type[mbPartIdx]].SubMbPredMode;
			NumSubMbPart[mbPartIdx] = sub_mb_type_mbs_p[sub_mb_type[mbPartIdx]].NumSubMbPart;
			SubMbPartWidth[mbPartIdx] = sub_mb_type_mbs_p[sub_mb_type[mbPartIdx]].SubMbPartWidth;
			SubMbPartHeight[mbPartIdx] = sub_mb_type_mbs_p[sub_mb_type[mbPartIdx]].SubMbPartHeight;
		}
		else if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_B && sub_mb_type[mbPartIdx] >= 0 && sub_mb_type[mbPartIdx] <= 12)
		{
			subMbType[mbPartIdx] = sub_mb_type_mbs_b[sub_mb_type[mbPartIdx]].name;
			subMode[mbPartIdx] = sub_mb_type_mbs_b[sub_mb_type[mbPartIdx]].SubMbPredMode;
			NumSubMbPart[mbPartIdx] = sub_mb_type_mbs_b[sub_mb_type[mbPartIdx]].NumSubMbPart;
			SubMbPartWidth[mbPartIdx] = sub_mb_type_mbs_b[sub_mb_type[mbPartIdx]].SubMbPartWidth;
			SubMbPartHeight[mbPartIdx] = sub_mb_type_mbs_b[sub_mb_type[mbPartIdx]].SubMbPartHeight;
		}
		else
		{
			printError("子宏块sub_mb_type错误");
			exit(-1);
		}
	}


	for (size_t mbPartIdx = 0; mbPartIdx < 4; mbPartIdx++)
	{
		if ((sliceBase->sHeader.num_ref_idx_l0_active_minus1 > 0 || false)
			&& mbType != H264_MB_TYPE::P_8x8ref0
			&& subMbType[mbPartIdx] != H264_MB_TYPE::B_Direct_8x8
			&& subMode[mbPartIdx] != H264_MB_PART_PRED_MODE::Pred_L1
			)
		{
			if (isAe)
			{
				bool is_ref_idx_l0 = true;
				cabac.decode_ref_idx_lX(sliceBase, bs, mbPartIdx, is_ref_idx_l0, ref_idx_l0[mbPartIdx]);
			}
			else
			{
				//sHeader.num_ref_idx_l1_active_minus1 - 1;
					//ref_idx_l0[ mbPartIdx ]的取值范围将是0到num_ref_idx_l0_active_minus
				ref_idx_l0[mbPartIdx] = bs.readTE(sliceBase->sHeader.num_ref_idx_l0_active_minus1);
			}
		}
	}

	//这个句法元素用于参考帧队列 L1，即后向预测
	for (size_t mbPartIdx = 0; mbPartIdx < 4; mbPartIdx++)
	{
		if ((sliceBase->sHeader.num_ref_idx_l1_active_minus1 > 0 || false)
			&& mbType != H264_MB_TYPE::P_8x8ref0
			&& subMbType[mbPartIdx] != H264_MB_TYPE::B_Direct_8x8
			&& subMode[mbPartIdx] != H264_MB_PART_PRED_MODE::Pred_L0
			)
		{
			if (isAe)
			{
				bool is_ref_idx_l0 = false;
				cabac.decode_ref_idx_lX(sliceBase, bs, mbPartIdx, is_ref_idx_l0, ref_idx_l1[mbPartIdx]);
			}
			else
			{
				//sHeader.num_ref_idx_l1_active_minus1 - 1;
					//ref_idx_l0[ mbPartIdx ]的取值范围将是0到num_ref_idx_l0_active_minus
				ref_idx_l1[mbPartIdx] = bs.readTE(sliceBase->sHeader.num_ref_idx_l1_active_minus1);
			}
		}
	}


	for (size_t mbPartIdx = 0; mbPartIdx < 4; mbPartIdx++)
	{
		if (subMbType[mbPartIdx] != H264_MB_TYPE::B_Direct_8x8
			&& subMode[mbPartIdx] != H264_MB_PART_PRED_MODE::Pred_L1
			)
		{
			for (size_t subMbPartIdx = 0; subMbPartIdx < NumSubMbPart[mbPartIdx]; subMbPartIdx++)
			{
				for (size_t compIdx = 0; compIdx < 2; compIdx++)
				{
					if (isAe)
					{
						int mvd_flag = compIdx;
						cabac.decode_mvd_lX(sliceBase, bs, mbPartIdx, subMbPartIdx, mvd_flag, mvd_l0[mbPartIdx][subMbPartIdx][compIdx]);
					}
					else
					{
						mvd_l0[mbPartIdx][subMbPartIdx][compIdx] = bs.readSE();
					}
				}

			}

		}
	}


	for (size_t mbPartIdx = 0; mbPartIdx < 4; mbPartIdx++)
	{
		if (subMbType[mbPartIdx] != H264_MB_TYPE::B_Direct_8x8
			&& subMode[mbPartIdx] != H264_MB_PART_PRED_MODE::Pred_L0
			)
		{
			for (size_t subMbPartIdx = 0; subMbPartIdx < NumSubMbPart[mbPartIdx]; subMbPartIdx++)
			{
				for (size_t compIdx = 0; compIdx < 2; compIdx++)
				{
					if (isAe)
					{
						int mvd_flag = 2 + compIdx;
						cabac.decode_mvd_lX(sliceBase, bs, mbPartIdx, subMbPartIdx, mvd_flag, mvd_l1[mbPartIdx][subMbPartIdx][compIdx]);
					}
					else
					{
						mvd_l1[mbPartIdx][subMbPartIdx][compIdx] = bs.readSE();
					}
				}

			}

		}
	}
	return false;
}



Macroblock::~Macroblock()
{
}
