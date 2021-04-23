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
	mb_qp_delta = 0;


	isAe = false;


	memset(prev_intra4x4_pred_mode_flag, 0, sizeof(uint8_t) * 16);
	memset(rem_intra4x4_pred_mode, 0, sizeof(uint8_t) * 16);
	memset(prev_intra8x8_pred_mode_flag, 0, sizeof(uint8_t) * 4);
	memset(rem_intra8x8_pred_mode, 0, sizeof(uint8_t) * 4);

	intra_chroma_pred_mode = 0;
}
//slice type ��������
//I slice�еĺ������ֻ����I�������
//P slice�а�����I�������,��P�������
//B slice�а�����I�������,��B�������
//sp slice�а�����I�������,��p�������
//si ������si���
//Intra16x16PredMode  �����ǰ������Ͳ��õ�Ԥ�ⷽʽΪIntra_16x16����ô���ֶ���Ч������0~3��ʾ��Intra_16x16�е�����ģʽ


//CodedBlockPatternLuma  �����ǰ������Ͳ��õ�Ԥ�ⷽʽΪIntra_16x16����ô���ֶ���Ч������ʾ��Luma����е�CBP��
//��h.264�﷨�ṹ������residual���֣�����֪����Ԥ��ģʽΪIntra_16x16ʱ�������Ҫ�ֿ�AC level��DC level�����ر��롣
//0����ʾ����ڵ�16��4x4���е�AC levelȫ����Ϊ0��15������ڵ�16��4x4����������һ�����AC level��ȫΪ0



//CodedBlockPatternChroma :�����ǰ������Ͳ��õ�Ԥ�ⷽʽΪIntra_16x16����ô���ֶ���Ч,����ʾ��Luma����е�CBP
//
//һ������ɫ�ȷ�����coded_block_pattern��Cb��Cr��CodedBlockPatternChroma��ͬ��
bool Macroblock::macroblock_layer(BitStream& bs, const SliceHeader& sHeader)
{
	isAe = sHeader.pps.entropy_coding_mode_flag;  //ae(v)��ʾCABAC����



	if (isAe) // ae(v) ��ʾCABAC����
	{
		//ret = cabac.CABAC_decode_mb_type(picture, bs, mb_type); //2 ue(v) | ae(v)
		//RETURN_IF_FAILED(ret != 0, ret);
	}
	else // ue(v) ��ʾCAVLC����
	{
		//��ǰ�����ʲô���ͣ�����ʲô���뷽ʽ mb_type������͵���ֵ��I slice����26����ֵ
		mb_type = bs.readUE(); //2 ue(v) | ae(v)
	}

	uint32_t	 slice_type = sHeader.slice_type;

	uint32_t	 fix_mb_type = mb_type;		//���������

	SLIECETYPE   fix_slice_type = (SLIECETYPE)(slice_type % 5);   //���������

	//����mb_type����Ϊp����ܰ�����i���
	fixed_mb_type(slice_type, fix_mb_type, fix_slice_type);



	//��ȡ��ǰ����Ԥ��ģʽ
	H264_MB_PART_PRED_MODE mode = MbPartPredMode(fix_mb_type, fix_slice_type, 0);
	uint32_t  numMbPart = NumMbPart(fix_mb_type, fix_slice_type);

	if (fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_I && fix_mb_type == 25)  //I_PCM ������Ԥ�⣬�任������
	{
		while (!byte_aligned(bs))
		{
			/*���ر���ģʽ��CABACʱ,
			  ��ʱҪ�������ֽڶ���, �����ݴ���һ���ֽڵĵ�һ�����ؿ�ʼ,
			  �����û���ֽڶ��뽫�������ɸ� pcm_alignment_zero_bit ��Ϊ��䡣*/
			pcm_alignment_zero_bit = bs.readBit(); //2 f(1)
		}

		for (size_t i = 0; i < 256; i++)
		{
			int32_t v = sHeader.sps.BitDepthY;
			/*pcm_sample_luma[i]��һ������ֵ����һ�� pcm_sample_luma[i]ֵ���������դɨ���е���������ֵ��
			  ���ص���Ŀͨ��������Щ����ÿһ������BitDepthY ��
			  �� profile_idc ������ 100, 110, 122 �� 144 ʱ�� pcm_sample_luma[i]���ܵ���0*/
			pcm_sample_luma[i] = bs.readMultiBit(v); //3 u(v)
		}

		//MbWidthC   MbHeightCɫ�����еĿ�Ⱥ͸߶�
		pcm_sample_chroma = new uint32_t[2 * sHeader.sps.MbWidthC * sHeader.sps.MbHeightC]();
		for (size_t i = 0; i < 2 * sHeader.sps.MbWidthC * sHeader.sps.MbHeightC; i++)
		{
			int32_t v = sHeader.sps.BitDepthC;
			/*pcm_sample_ chroma[i]��һ������ֵ��ɫ��
			��һ�� MbWidthC* MbHeightC pcm_sample_ chroma[i]ֵ���������դɨ���е�Cb����ֵ�������MbWidthC* MbHeightC
			pcm_sample_chroma[i]ֵ���������դɨ���� �� Cr ����ֵ�����ص���Ŀͨ��������Щ����ÿһ������ BitDepthC
			�� profile_idc ������ 100, 110, 122 �� 144 ʱ��pcm_sample_ chroma[i]���ܵ���0��
			*/
			pcm_sample_chroma[i] = bs.readMultiBit(v); //3 u(v)
		}
	}
	else {
		bool noSubMbPartSizeLessThan8x8Flag = true;

		if (!is_I_NxN(fix_mb_type, fix_slice_type) &&
			mode != H264_MB_PART_PRED_MODE::Intra_16x16 &&
			numMbPart == 4)
		{

			sub_mb_pred(fix_mb_type);
			for (int mbPartIdx = 0; mbPartIdx < 4; mbPartIdx++)
			{
				/*if (sub_mb_type[mbPartIdx] != B_Direct_8x8) {
					if (NumSubMbPart(sub_mb_type[mbPartIdx]) > 1)
						noSubMbPartSizeLessThan8x8Flag = 0
				}
				else if (!direct_8x8_inference_flag)
					noSubMbPartSizeLessThan8x8Flag = 0*/
			}
			//........
		}
		else
		{
			if (sHeader.pps.transform_8x8_mode_flag && is_I_NxN(fix_mb_type, fix_slice_type))
			{
				//ʹ��8x8�任����
				if (isAe)
				{

				}
				else
				{
					/*����1��ʾ���ڵ�ǰ��飬���������б任ϵ��������̺�ͼ�񹹽����̵ĵ��ã�
					  Ӧ�ڲ���� 8x8 ���ȥ��ЧӦ�˲�����֮ǰ��ɡ�transform_size_8x8_flag ���� 0 ��ʾ���ڵ�ǰ��飬
					  �������㵱�б任ϵ��������̺�ͼ�񹹽����̵ĵ�����ʣ���4x4���ȥ��ЧӦ�˲�����֮ǰ��ɡ�
					  ��� transform_size_8x8_flag �ڱ������в����ڣ���Ĭ����ֵΪ0��*/
					transform_size_8x8_flag = bs.readBit();
				}

				//........
			}

			mb_pred(bs, sHeader, fix_mb_type, mode, numMbPart);
		}

		if (mode != H264_MB_PART_PRED_MODE::Intra_16x16)
		{

			if (isAe)
			{
				//cabac
			}
			else
			{
				//��ʾ�������Ⱥ�ɫ��8x8����ܺ��з���ı任ϵ����ֵ����Ԥ��ģʽ������ Intra_16x16 �� ���
				coded_block_pattern = bs.readME(sHeader.sps.ChromaArrayType, mode);

			}
			//һ���������ȷ�����coded_block_pattern
			CodedBlockPatternLuma = coded_block_pattern % 16;

			/*CodedBlockPatternChroma
				����
				0		����ɫ�ȱ任ϵ����ֵ������ 0��
				1		һ������ɫ��   DC �任ϵ����ֵ��Ϊ0������ɫ��   AC �任ϵ����ֵ���� �� 0��
				2		�������ɫ��   DC �任ϵ����ֵ��Ϊ0��һ������ɫ��   AC �任ϵ���� ֵ��Ϊ 0��*/
			CodedBlockPatternChroma = coded_block_pattern / 16;

			if (
				CodedBlockPatternLuma > 0 &&
				sHeader.pps.transform_8x8_mode_flag &&
				!is_I_NxN(fix_mb_type, fix_slice_type) &&
				noSubMbPartSizeLessThan8x8Flag &&
				(!(fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_B && mb_type == 0) || sHeader.sps.direct_8x8_inference_flag)
				)
			{

				if (isAe)
				{
					//transform_size_8x8_flag
				}
				else
				{
					transform_size_8x8_flag = bs.readBit();
				}

			}
		}

		if (CodedBlockPatternLuma > 0 || CodedBlockPatternChroma > 0 || mode == H264_MB_PART_PRED_MODE::Intra_16x16)
		{
			if (isAe)
			{
				//mb_qp_delta
			}
			else
			{
				//�ܸı���� �� QPY ��ֵ ��mb_qp_delta ������ֵӦ�����ڨC( 26 + QpBdOffsetY / 2) �� +( 25 + QpBdOffsetY / 2 )��Χ�ڡ�
				//���κκ�飨���� P_Skip �� B_Skip ������ͣ��ж������� mb_qp_delta ʱ�� mb_qp_delta Ĭ��Ϊ0��
				mb_qp_delta = bs.readSE();
			}
			residual(bs, sHeader, 0, 15);
		}
	}


	return false;
}

//���Ԥ���﷨
bool Macroblock::mb_pred(BitStream& bs, const SliceHeader& sHeader, uint32_t mb_type, H264_MB_PART_PRED_MODE mode, uint32_t numMbPart)
{

	if (mode == H264_MB_PART_PRED_MODE::Intra_4x4 || mode == H264_MB_PART_PRED_MODE::Intra_8x8 || mode == H264_MB_PART_PRED_MODE::Intra_16x16)
	{
		if (mode == H264_MB_PART_PRED_MODE::Intra_4x4)
		{
			for (size_t luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++)
			{

				//��ʾ���Ϊ luma4x4BlkIdx = 0��15 ��4x4 ���ȿ��֡��Intra_4x4 Ԥ�⡣
				if (isAe) // ae(v) ��ʾCABAC����
				{

				}
				else
				{
					//������ʾ�ҵ�ǰ�õ�ģʽ��ǰ����ǲ���һ����
					prev_intra4x4_pred_mode_flag[luma4x4BlkIdx] = bs.readBit();
				}

				if (!prev_intra4x4_pred_mode_flag[luma4x4BlkIdx]) {
					if (isAe) // ae(v) ��ʾCABAC����
					{

					}
					else
					{
						//��9��Ԥ��ģʽ
						/*ģʽ0����ֱģʽ��������A~D���á�
						  ģʽ1��ˮƽģʽ��������I~L���á�
						  ģʽ2��DCģʽ��������A~D��I~L���á�
						  ģʽ3~8������ģʽ��������������A��L����ͨ��Ȩ�ز��ȵĹ�ʽ��Ȩ����ġ�*/
						rem_intra4x4_pred_mode[luma4x4BlkIdx] = bs.readMultiBit(3);
					}

				}
			}
		}


		if (mode == H264_MB_PART_PRED_MODE::Intra_8x8)
		{
			for (size_t luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++) {
				//��ʾ���Ϊ luma8x8BlkIdx = 0��3 ��8x8 ���ȿ��Intra_8x8 Ԥ��
				if (isAe) // ae(v) ��ʾCABAC����
				{

				}
				else
				{
					prev_intra8x8_pred_mode_flag[luma8x8BlkIdx] = bs.readBit();
				}

				if (!prev_intra8x8_pred_mode_flag[luma8x8BlkIdx]) {
					if (isAe) // ae(v) ��ʾCABAC����
					{

					}
					else
					{
						rem_intra8x8_pred_mode[luma8x8BlkIdx] = bs.readMultiBit(3);
					}

				}
			}
		}
		// YUV 4 : 2 : 0 || YUV 4 : 2 : 2��yuvһ�����
		if (sHeader.sps.ChromaArrayType == 1 || sHeader.sps.ChromaArrayType == 2)
		{
			//���������ɫ�ȵĿռ�Ԥ������ʹ��Intra_4x4 �� Intra_16x16 Ԥ��
			//intra_chroma_pred_mode ��ȡֵ��ΧΪ0 ��  3��
			//  0 = DC �ֱ���֮Ϊ��ֵģʽ����ֵģʽ�£�4 x 4 �ӿ��� 16 �����ض�����ͬ��ֵ��
			//         ���Ϸ����ĸ����� A B C D ������ĸ����� I J K L �ľ�ֵ��
			//	1 = ˮƽ��
			//	2 = ��ֱ��
			//	3 = ƽ���
			if (isAe) // ae(v) ��ʾCABAC����
			{

			}
			else
			{
				intra_chroma_pred_mode = bs.readUE();
			}
		}
	}
	else if (mode != H264_MB_PART_PRED_MODE::Direct)
	{
		for (size_t mbPartIdx = 0; mbPartIdx < numMbPart; mbPartIdx++)
		{
			//��ref_idx_l0[mbPartIdx] ����ʱ, ����ʾ�ο�ͼ���б����Ϊ0 �Ĳο�ͼ������Ԥ��

			/*if ((sHeader.num_ref_idx_l0_active_minus1 > 0  ||
				slice_data.mb_field_decoding_flag != field_pic_flag) && MbPartPredMode(mb_type, mbPartIdx) != Pred_L1)
			{

			}*/
		}
	}
	return false;
}
//��õ�ǰ������������õ�IntraԤ�ⷽʽ
H264_MB_PART_PRED_MODE Macroblock::MbPartPredMode(uint32_t mb_type, SLIECETYPE slice_type, uint32_t mbPartIdx)
{
	H264_MB_PART_PRED_MODE mode = H264_MB_PART_PRED_MODE::NA;

	if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_I)
	{
		if (mbPartIdx == 0)
		{
			//mb_type == I_NxN
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
				mode = mb_type_slices_I[mb_type].MbPartPredMode;  //na 
			}
			else {
				mode = mb_type_slices_I[mb_type + 1].MbPartPredMode;
			}
		}
		else
		{
			printError("i���mbPartIdx������0");
			exit(1);
		}

	}
	else if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_P || slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP)
	{

		if (mbPartIdx == 1)
		{
			mode = mb_type_sleces_sp_p[mb_type].MbPartPredMode1;
		}
		else {
			mode = mb_type_sleces_sp_p[mb_type].MbPartPredMode0;
		}
	}
	else if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		if (mbPartIdx)
		{
			mode = mb_type_sleces_b[mb_type].MbPartPredMode1;
		}
		else {
			mode = mb_type_sleces_b[mb_type].MbPartPredMode0;
		}
	}
	else if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		if (mb_type == 0)
		{
			mode = mb_type_sleces_si[mb_type].MbPartPredMode;
		}
		else
		{
			printError("SImbPartIdx������0");
			exit(1);
		}

	}
	return mode;
}

H264_MB_PART_PRED_MODE Macroblock::MbPartPredMode2(uint32_t mb_type, SLIECETYPE slice_type, uint32_t mbPartIdx)
{

	H264_MB_PART_PRED_MODE mode = H264_MB_PART_PRED_MODE::NA;
	return H264_MB_PART_PRED_MODE();
}

//��鱻�ָ�ɶ��ٲ���
uint32_t Macroblock::NumMbPart(uint32_t mb_type, SLIECETYPE slice_type)
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



//����slice_type��mb_type
int Macroblock::fixed_mb_type(uint32_t slice_type, uint32_t& fix_mb_type, SLIECETYPE& fix_slice_type)
{

	if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		if (mb_type >= 1 && mb_type <= 26)//˵��������i���
		{
			fix_mb_type = mb_type - 1;
			fix_slice_type = SLIECETYPE::H264_SLIECE_TYPE_I;  //˵��������I���
		}
	}
	else if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_SP ||
		(SLIECETYPE)(slice_type % 5) != SLIECETYPE::H264_SLIECE_TYPE_P)
	{
		if (mb_type >= 5 && mb_type <= 30)
		{
			fix_mb_type = mb_type - 5;
			fix_slice_type = SLIECETYPE::H264_SLIECE_TYPE_I;  //˵��������I���
		}
	}
	else if ((SLIECETYPE)(slice_type % 5) == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		if (mb_type >= 23 && mb_type <= 48)
		{
			fix_mb_type = mb_type - 23;
			fix_slice_type = SLIECETYPE::H264_SLIECE_TYPE_I;  //˵��������I���
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


//����в�����
bool Macroblock::residual(BitStream& bs, const SliceHeader& sHeader, int startIdx, int endIdx)
{

	bool isAe = sHeader.pps.entropy_coding_mode_flag;
	if (isAe)
	{
		//residual_block = residual_block_cabac
	}
	else
	{
		ResidualBlockCavlc residual_block;
	}

	residual_luma(bs, sHeader, i16x16DClevel, i16x16AClevel, level4x4, level8x8, startIdx, endIdx);
	return false;
}
int Macroblock::residual_luma(BitStream& bs, const SliceHeader& sHeader, int32_t i16x16DClevel[16], int32_t i16x16AClevel[16][16], int32_t level4x4[16][16], int32_t level8x8[4][64], int32_t startIdx, int32_t endIdx)
{
	return 0;
}
bool Macroblock::sub_mb_pred(uint32_t mb_type)
{
	return false;
}

Macroblock::~Macroblock()
{
}
