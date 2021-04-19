#include "Macroblock.h"


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
	{2,		H264_MB_TYPE::P_L0_L0_8x16,		3,	H264_MB_PART_PRED_MODE::Pred_L0,	H264_MB_PART_PRED_MODE::Pred_L0,  8,   16},
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



Macroblock::Macroblock()
{
	pcm_alignment_zero_bit = 0;
	transform_size_8x8_flag = 0;
	memset(pcm_sample_luma, 0, sizeof(uint8_t) * 256);
	pcm_sample_chroma = nullptr;
	mb_type = 0;
	coded_block_pattern = 0;

	CodedBlockPatternChroma = -1;
	CodedBlockPatternLuma = -1;
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
//一个宏块的色度分量的coded_block_pattern，Cb、Cr的CodedBlockPatternChroma相同。
bool Macroblock::macroblock_layer(BitStream& bs, const SliceHeader& sHeader)
{
	bool isAe = sHeader.pps.entropy_coding_mode_flag;  //ae(v)表示CABAC编码



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

	uint32_t	 slice_type = sHeader.slice_type;

	uint32_t	 fix_mb_type = mb_type;		//修正过后的

	SLIECETYPE   fix_slice_type = (SLIECETYPE)(slice_type % 5);   //修正过后的

	//修正mb_type，因为p里可能包含了i宏块
	fixed_mb_type(slice_type, fix_mb_type, fix_slice_type);



	//获取当前宏块的预测模式
	H264_MB_PART_PRED_MODE mode = MbPartPredMode(fix_mb_type, transform_size_8x8_flag, fix_slice_type, false);


	if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_I && fix_mb_type == 25)  //I_PCM 不经过预测，变换，量化
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

		if (!is_I_NxN(fix_mb_type, fix_slice_type) &&
			mode != H264_MB_PART_PRED_MODE::Intra_16x16 &&
			NumMbPart(fix_mb_type, fix_slice_type) == 4)
		{
			//........
		}
		else
		{
			if (sHeader.pps.transform_8x8_mode_flag && is_I_NxN(fix_mb_type, fix_slice_type))
			{
				//使用8x8变换解码
				if (isAe)
				{

				}
				else
				{
					/*等于1表示对于当前宏块，亮度样点中变换系数解码过程和图像构建过程的调用，
					  应在残余的 8x8 块的去块效应滤波过程之前完成。transform_size_8x8_flag 等于 0 表示对于当前宏块，
					  亮度样点当中变换系数解码过程和图像构建过程的调用在剩余的4x4块的去块效应滤波过程之前完成。
					  如果 transform_size_8x8_flag 在比特流中不存在，则默认其值为0。*/
					transform_size_8x8_flag = bs.readBit();
				}

				//........
			}

			mb_pred(fix_mb_type);
		}

		if (mode != H264_MB_PART_PRED_MODE::Intra_16x16)
		{

			if (isAe)
			{
				//cabac
			}
			else
			{
				//表示六个亮度和色度8x8块可能含有非零的变换系数幅值。对预测模式不等于 Intra_16x16 的 宏块
				coded_block_pattern = bs.readME(sHeader.sps.ChromaArrayType, mode);
				//一个宏块的亮度分量的coded_block_pattern
				CodedBlockPatternLuma = coded_block_pattern % 16;

				/*CodedBlockPatternChroma
					描述
					0		所有色度变换系数幅值都等于 0。
					1		一个或多个色度   DC 变换系数幅值不为0。所有色度   AC 变换系数幅值都等 于 0。
					2		零个或多个色度   DC 变换系数幅值不为0。一个或多个色度   AC 变换系数幅 值不为 0。*/
				CodedBlockPatternChroma = coded_block_pattern / 16;
			}
			//slice_type == SLIECETYPE::H264_SLIECE_TYPE_B && mb_type == 0

			if (
				CodedBlockPatternLuma > 0 && 
				sHeader.pps.transform_8x8_mode_flag && 
				!is_I_NxN(fix_mb_type, fix_slice_type) && 
				noSubMbPartSizeLessThan8x8Flag && 
				(!(fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_B && mb_type == 0) || sHeader.sps.direct_8x8_inference_flag)
				)
			{
				//transform_size_8x8_flag
			}
		}

		if (CodedBlockPatternLuma>0 || CodedBlockPatternChroma>0 || mode == H264_MB_PART_PRED_MODE::Intra_16x16)
		{

		}
	}


	return false;
}
//获得当前宏块类型所采用的Intra预测方式
H264_MB_PART_PRED_MODE Macroblock::MbPartPredMode(uint32_t mb_type, int transform_size_8x8_flag, SLIECETYPE slice_type, bool flag)
{
	H264_MB_PART_PRED_MODE mode;

	if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_I)
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
			mode = mb_type_slices_I[mb_type].MbPartPredMode;
		}
		else {
			mode = mb_type_slices_I[mb_type + 1].MbPartPredMode;
		}
	}
	else if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_P || slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP)
	{
		if (flag)
		{
			mode = mb_type_sleces_sp_p[mb_type].MbPartPredMode1;
		}
		else {
			mode = mb_type_sleces_sp_p[mb_type].MbPartPredMode0;
		}
	}
	else if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		if (flag)
		{
			mode = mb_type_sleces_b[mb_type].MbPartPredMode1;
		}
		else {
			mode = mb_type_sleces_b[mb_type].MbPartPredMode0;
		}
	}
	else if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		mode = mb_type_sleces_si[mb_type].MbPartPredMode;
	}
	return mode;
}

//宏块被分割成多少部分
int Macroblock::NumMbPart(uint32_t mb_type, SLIECETYPE slice_type)
{
	int ret = 0;
	if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_P || slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP)
	{
		ret = mb_type_sleces_sp_p[mb_type].NumMbPart;
	}
	else if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		ret = mb_type_sleces_b[mb_type].NumMbPart;
	}
	return ret;
}

//修正slice_type和mb_type
int Macroblock::fixed_mb_type(uint32_t slice_type, uint32_t& fix_mb_type, SLIECETYPE& fix_slice_type)
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
		(SLIECETYPE)(slice_type % 5) != SLIECETYPE::H264_SLIECE_TYPE_P)
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

bool Macroblock::is_I_NxN(uint32_t mb_type, SLIECETYPE slice_type)
{
	if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_I && mb_type == 0)
	{
		return true;
	}

	return false;
}

//宏块预测语法
bool Macroblock::mb_pred(uint32_t mb_type)
{
	return false;
}

Macroblock::~Macroblock()
{
}
