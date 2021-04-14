#include "Macroblock.h"


MB_TYPE_SLICES_I mb_type_slices_I[27] =
{
	{ 0,     H264_MB_TYPE::I_NxN,            0,     H264_MB_PART_PRED_MODE::Intra_4x4,      NA,  -1,   -1 },
	{ 0,     H264_MB_TYPE::I_NxN,            1,     H264_MB_PART_PRED_MODE::Intra_8x8,      NA,  -1,   -1 },
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
	{2,		H264_MB_TYPE::P_L0_L0_8x16,		3,	H264_MB_PART_PRED_MODE::Pred_L0,	H264_MB_PART_PRED_MODE::Pred_L0,  8,   16},
	{3,		H264_MB_TYPE::P_8x8,			4,	H264_MB_PART_PRED_MODE::NA,			H264_MB_PART_PRED_MODE::NA,		  8,   8},
	{4,		H264_MB_TYPE::P_8x8ref0,		4,	H264_MB_PART_PRED_MODE::NA,			H264_MB_PART_PRED_MODE::NA,		  8,   8},
	{5,		H264_MB_TYPE::P_Skip,			1,	H264_MB_PART_PRED_MODE::Pred_L0,	H264_MB_PART_PRED_MODE::NA,		  16,   16}
};



Macroblock::Macroblock()
{
	pcm_alignment_zero_bit = 0;
	transform_size_8x8_flag = 0;
	memset(pcm_sample_luma, 0, sizeof(uint8_t) * 256);
	pcm_sample_chroma = nullptr;
	mb_type = 0;
}
//slice type 五种类型
//I slice中的宏块类型只能是I宏块类型
//P slice中包含了I宏块类型,与P宏块类型
//B slice中包含了I宏块类型,与B宏块类型
//sp slice中包含了I宏块类型,与p宏块类型
//si 仅包含si宏块
bool Macroblock::macroblock_layer(BitStream& bs, const SliceHeader& sHeader)
{
	bool isAe = sHeader.pps.entropy_coding_mode_flag;  //ae(v)表示CABAC编码

	uint32_t slice_type = sHeader.slice_type;


	if (isAe) // ae(v) 表示CABAC编码
	{
		//ret = cabac.CABAC_decode_mb_type(picture, bs, mb_type); //2 ue(v) | ae(v)
		//RETURN_IF_FAILED(ret != 0, ret);
	}
	else // ue(v) 表示CAVLC编码
	{
		//当前宏块是什么类型，采用什么解码方式 mb_type宏块类型的数值，I slice共有26个数值
		mb_type = bs.readUE(); //2 ue(v) | ae(v)
	}

	if (mb_type == 25)  //I_PCM 不经过预测，变换，量化
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
			int32_t v = sHeader.sps.BitDepthY;
			/*pcm_sample_luma[i]是一个样点值。第一个 pcm_sample_luma[i]值代表宏块里光栅扫描中的亮度样点值。
			  比特的数目通常代表这些样点每一个都是BitDepthY 。
			  当 profile_idc 不等于 100, 110, 122 或 144 时， pcm_sample_luma[i]不能等于0*/
			pcm_sample_luma[i] = bs.readMultiBit(v); //3 u(v)
		}

		//MbWidthC   MbHeightC色度阵列的宽度和高度
		pcm_sample_chroma = new uint8_t[2 * sHeader.sps.MbWidthC * sHeader.sps.MbHeightC]();
		for (size_t i = 0; i < 2 * sHeader.sps.MbWidthC * sHeader.sps.MbHeightC; i++)
		{
			int32_t v = sHeader.sps.BitDepthC;
			/*pcm_sample_ chroma[i]是一个样点值。色度
			第一个 MbWidthC* MbHeightC pcm_sample_ chroma[i]值代表宏块里光栅扫描中的Cb样点值且其余的MbWidthC* MbHeightC
			pcm_sample_chroma[i]值代表宏块里光栅扫描中 的 Cr 样点值。比特的数目通常代表这些样点每一个都是 BitDepthC
			当 profile_idc 不等于 100, 110, 122 或 144 时，pcm_sample_ chroma[i]不能等于0。
			*/
			pcm_sample_chroma[i] = bs.readMultiBit(v); //3 u(v)
		}
	}
	else {
		bool noSubMbPartSizeLessThan8x8Flag = true;


		if ((H264_MB_TYPE)mb_type != H264_MB_TYPE::I_NxN &&
			MbPartPredMode(mb_type, transform_size_8x8_flag, slice_type, false) != H264_MB_PART_PRED_MODE::Intra_16x16 &&
			NumMbPart(mb_type, slice_type) == 4)
		{

		}
	}


	return false;
}
//
H264_MB_PART_PRED_MODE Macroblock::MbPartPredMode(uint8_t mb_type, int transform_size_8x8_flag, uint8_t slice_type,bool flag)
{
	H264_MB_PART_PRED_MODE mode;
	if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_I)
	{
		if (mb_type == 0) {
			if (transform_size_8x8_flag) {
				mode = mb_type_slices_I[1].MbPartPredMode;
			}
			else {
				mode = mb_type_slices_I[0].MbPartPredMode;
			}

		}
		else if (mb_type == 25)
		{
			mode = mb_type_slices_I[25].MbPartPredMode;
			/*printError("mb_type=25");
			exit(0);*/
		}
		else {
			mode = mb_type_slices_I[mb_type + 1].MbPartPredMode;
		}
	}
	else if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_P || (SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_SP)
	{
		if (flag)
		{
			mode = mb_type_sleces_sp_p[mb_type].MbPartPredMode1;
		}
		else {
			mode = mb_type_sleces_sp_p[mb_type].MbPartPredMode0;
		}
	}
	else if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_B)
	{

	}
	else if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_SI)
	{

	}


	return mode;
}

int Macroblock::NumMbPart(uint8_t mb_type, uint8_t slice_type)
{
	int ret = 0;
	if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_P)
	{

	}
	else if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_B)
	{

	}
	return ret;
}

Macroblock::~Macroblock()
{
}
