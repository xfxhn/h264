#include "SliceHeader.h"

SliceHeader::SliceHeader(ParseNalu& nalu) :nalu(nalu)
{
	first_mb_in_slice = 0;
	slice_type = 0;
	pic_parameter_set_id = 0;
	colour_plane_id = 0;
	frame_num = 0;
	field_pic_flag = false;
	bottom_field_flag = 0;
	idr_pic_id = 0;
	pic_order_cnt_lsb = 0;
	delta_pic_order_cnt_bottom = 0;
	delta_pic_order_cnt[0] = 0;
	delta_pic_order_cnt[1] = 0;
	redundant_pic_cnt = 0;
	direct_spatial_mv_pred_flag = 0;
	num_ref_idx_active_override_flag = 0;
	num_ref_idx_l0_active_minus1 = 0;
	num_ref_idx_l1_active_minus1 = 0;
	cabac_init_idc = 0;
	slice_qp_delta = 0;
	sp_for_switch_flag = 0;
	slice_qs_delta = 0;
	disable_deblocking_filter_idc = 0;
	slice_group_change_cycle = 0;
	slice_beta_offset_div2 = 0;
	slice_group_change_cycle = 0;

	ref_pic_list_modification_flag_l0 = 0;
	memset(modification_of_pic_nums_idc, 0, sizeof(int32_t) * 32 * 2);
	memset(abs_diff_pic_num_minus1, 0, sizeof(int32_t) * 32 * 2);
	memset(long_term_pic_num, 0, sizeof(int32_t) * 32 * 2);
	ref_pic_list_modification_flag_l1 = 0;
	ref_pic_list_modification_count_l0 = 0;
	ref_pic_list_modification_count_l1 = 0;

	luma_log2_weight_denom = 0;
	chroma_log2_weight_denom = 0;
	luma_weight_l0_flag = 0;
	memset(luma_weight_l0, 0, sizeof(int32_t) * 32);
	memset(luma_offset_l0, 0, sizeof(int32_t) * 32);
	chroma_weight_l0_flag = 0;
	memset(chroma_weight_l0, 0, sizeof(int32_t) * 32 * 2);
	memset(chroma_offset_l0, 0, sizeof(int32_t) * 32 * 2);
	luma_weight_l1_flag = 0;
	memset(luma_weight_l1, 0, sizeof(int32_t) * 32);
	memset(luma_offset_l1, 0, sizeof(int32_t) * 32);
	chroma_weight_l1_flag = 0;
	memset(chroma_weight_l1, 0, sizeof(int32_t) * 32 * 2);
	memset(chroma_offset_l1, 0, sizeof(int32_t) * 32 * 2);

	no_output_of_prior_pics_flag = 0;
	long_term_reference_flag = 0;
	adaptive_ref_pic_marking_mode_flag = 0;



	MbaffFrameFlag = false;
	SliceGroupChangeRate = 0;
	SliceQPY = 0;
	QPY_prev = 0;

	PicHeightInMbs = 0;
	PicSizeInMbs = 0;
	mapUnitToSliceGroupMap = nullptr;
	MbToSliceGroupMap = nullptr;
	/*memset(m_dec_ref_pic_marking, 0, sizeof(DEC_REF_PIC_MARKING) * 32);
	dec_ref_pic_marking_count = 0;

	slice_id = 0;
	syntax_element_categories = 0;
	slice_type_fixed = 0;
	mb_cnt = 0;
	SliceQPY = 0;
	MbaffFrameFlag = 0;
	PicHeightInMbs = 0;
	PicHeightInSamplesL = 0;
	PicHeightInSamplesC = 0;
	PicSizeInMbs = 0;
	MaxPicNum = 0;
	CurrPicNum = 0;
	SliceGroupChangeRate = 0;
	MapUnitsInSliceGroup0 = 0;
	QSY = 0;
	picNumL0Pred = 0;
	picNumL1Pred = 0;
	refIdxL0 = 0;
	refIdxL1 = 0;

	mapUnitToSliceGroupMap = NULL;
	MbToSliceGroupMap = NULL;
	PrevRefFrameNum = 0;
	UnusedShortTermFrameNum = 0;
	FilterOffsetA = 0;
	FilterOffsetB = 0;

	m_picture_coded_type = H264_PICTURE_CODED_TYPE_UNKNOWN;
	m_is_malloc_mem_self = 0;*/

	memset(ScalingList4x4, 0, sizeof(int) * 6 * 16);
	memset(ScalingList8x8, 0, sizeof(int) * 6 * 64);
}



bool SliceHeader::slice_header(BitStream& bs, const ParsePPS ppsCache[256], const ParseSPS spsCache[32])
{
	//������Ա�ʾ��������� Slice �е�һ���������
	first_mb_in_slice = bs.readUE(); //2 ue(v)
	//slice������
	slice_type = bs.readUE() % 5; //2 ue(v)
	//������pps id
	pic_parameter_set_id = bs.readUE(); //2 ue(v)

	pps = ppsCache[pic_parameter_set_id];
	sps = spsCache[pps.seq_parameter_set_id];


	if (sps.separate_colour_plane_flag)
	{
		//ָ���뵱ǰ��ƬRBSP��������ɫƽ�档colour_plane_id��ֵӦ����0��2�ķ�Χ�ڣ�������
		//colour_plane_id����0��1��2�ֱ��Ӧ��Y��Cb��Crƽ�档
		colour_plane_id = bs.readMultiBit(2); //2 u(2)
	}
	// ����һ��ͼ���ʶ�� ���ڱ�������Ӧ�� log2_max_frame_num_minus4 + 4 �����ر�ʾ
	frame_num = bs.readMultiBit(sps.log2_max_frame_num_minus4 + 4); //2 u(v)
	//m_picture_coded_type = H264_PICTURE_CODED_TYPE_FRAME;


	//frame_mbs_only_flag�Ƿ�Ϊ֡����
	if (!sps.frame_mbs_only_flag)
	{
		//������䷨Ԫ��ȡֵΪ1ʱ���ڳ����룻 0ʱΪ�ǳ����롣��field_pic_flag ������ʱ��Ӧ�ƶ���ֵΪ0��
		field_pic_flag = bs.readBit(); //2 u(1)
		//m_picture_coded_type = H264_PICTURE_CODED_TYPE_TOP_FIELD;
		if (field_pic_flag)
		{
			//����1��ʾ��������һ������׳���һ���֡�bottom_field_flag ���� 0 ��ʾ��ͼ����һ������Ķ�����
			//�������������ڴ��﷨Ԫ��ʱ��Ӧ�ƶ���ֵΪ0
			bottom_field_flag = bs.readBit(); //2 u(1)
			if (bottom_field_flag)
			{
				//m_picture_coded_type = H264_PICTURE_CODED_TYPE_BOTTOM_FIELD;
			}
		}
	}
	//�Ƿ�������ˢ��֡�ı��
	if (nalu.IdrPicFlag)
	{
		idr_pic_id = bs.readUE();
	}

	/*��� pic_order_cnt_type����0��
		Ӧ���ǰ��ս���˳���б��е�һ��ͼ��֮�����������ͼ��
		��Щͼ�������� 8.2.5.2������frame_num��϶�Ľ�������еõ��ķ�"������"ͼ��
		�����ǽ���˳�� �е�ǰͼ��֮ǰ��ͼ������ǵ�ǰͼ�񡣵�pic_order_cnt_type����0��
		���ҵ�ǰͼ���� 8.2.5.2�ڹ� ����frame_num��϶�Ľ�������еõ���"������"ͼ��ʱ��
		��ǰͼ�񱻰������ڽ���ο�ͼ���ǹ��� ֮ǰ��listD�С�*/
	if (sps.pic_order_cnt_type == 0)
	{
		/*��ʾһ������֡�Ķ�����һ�����볡��ͼ��˳������ MaxPicOrderCntLsb  ȡģ��
			pic_order_cnt_lsb �﷨Ԫ�صĴ�С��log2_max_pic_order_cnt_lsb_minus4 + 4 �����ء�
			pic_order_cnt_lsb ��ֵӦ���� 0 �� MaxPicOrderCntLsb �C 1 �ķ�Χ�ڣ������߽�ֵ����*/
		pic_order_cnt_lsb = bs.readMultiBit(sps.log2_max_pic_order_cnt_lsb_minus4 + 4); //2 u(v)
		if (pps.bottom_field_pic_order_in_frame_present_flag && !field_pic_flag)
		{
			//��ʾһ������֡�ĵ׳��Ͷ�����ͼ��˳����֮��Ĳ�
			delta_pic_order_cnt_bottom = bs.readSE(); //2 se(v)
		}
	}


	if (sps.pic_order_cnt_type == 1 && !sps.delta_pic_order_always_zero_flag)
	{
		delta_pic_order_cnt[0] = bs.readSE(); //2 se(v)

		if (pps.bottom_field_pic_order_in_frame_present_flag && !field_pic_flag)
		{
			delta_pic_order_cnt[1] = bs.readSE(); //2 se(v)
		}
	}


	if (pps.redundant_pic_cnt_present_flag)
	{
		redundant_pic_cnt = bs.readUE(); //2 ue(v)
	}


	if ((SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		direct_spatial_mv_pred_flag = bs.readBit(); //2 u(1)
	}

	if ((SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_P || (SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP || (SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		num_ref_idx_active_override_flag = bs.readBit(); //2 u(1)

		num_ref_idx_l0_active_minus1 = pps.num_ref_idx_l0_default_active_minus1;
		num_ref_idx_l1_active_minus1 = pps.num_ref_idx_l1_default_active_minus1;

		if (num_ref_idx_active_override_flag)
		{
			num_ref_idx_l0_active_minus1 = bs.readUE(); //2 ue(v)
			if ((SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
			{
				num_ref_idx_l1_active_minus1 = bs.readUE(); //2 ue(v)
			}
		}

		if (num_ref_idx_l0_active_minus1 >= 32)
		{
			printf("num_ref_idx_l0_active_minus1=%d, which should be in [0,31].\n", num_ref_idx_l0_active_minus1);
			return -1;
		}

		if (num_ref_idx_l1_active_minus1 >= 32)
		{
			printf("num_ref_idx_l1_active_minus1=%d, which should be in [0,31].\n", num_ref_idx_l1_active_minus1);
			return -1;
		}
	}

	if (nalu.nal_unit_type == 20 || nalu.nal_unit_type == 21)
	{
		//ret = ref_pic_list_mvc_modification(bs); // /* specified in Annex H */ 2
		printf("Unsupported m_nal_unit.nal_unit_type == 20 || m_nal_unit.nal_unit_type == 21;\n");
		return -1;
	}
	else
	{

		bool ret = ref_pic_list_modification(bs);
		/*printError("ref_pic_list_modification ����");
		return -1;*/
		//RETURN_IF_FAILED(ret != 0, ret);
	}

	if ((pps.weighted_pred_flag && ((SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_P || (SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP))
		|| (pps.weighted_bipred_idc == 1 && (SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
		)
	{
		bool ret = pred_weight_table(bs);
		//RETURN_IF_FAILED(ret != 0, ret);
	}

	if (nalu.nal_ref_idc != 0)
	{
		bool ret = dec_ref_pic_marking(bs);
		//RETURN_IF_FAILED(ret != 0, ret);
	}
	//�ر���ʹ�õķ�������ͬ���﷨Ԫ���ڲ�ͬģʽѡ��ķ�ʽ��ͬ��
	//����0����ô�����﷨������ߵ���������ָ���ķ���
	//����1���Ͳ����﷨�����ұߵ���������ָ���ķ���
	if (pps.entropy_coding_mode_flag && (SLIECETYPE)slice_type != SLIECETYPE::H264_SLIECE_TYPE_I && (SLIECETYPE)slice_type != SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		//��ʾ���ھ������������ĳ�ʼ��������ʹ�õĳ�ʼ��������š����� cabac_init_idc ��ֵӦ ����0 ��  2 �ķ�Χ�ڣ�����0 ��2��
		cabac_init_idc = bs.readUE(); //2 ue(v)
	}
	//��ʾ���������е����к��� QPY  �ĳ�ʼֵ����ֵ�ں��㽫�� mb_qp_delta ��ֵ�޸ġ���������ʼ QPY ��������������Ĺ�ʽ���㣺
	//SliceQPY = 26 + pic_init_qp_minus26 + slice_qp_delta

	slice_qp_delta = bs.readSE();

	SliceQPY = 26 + pps.pic_init_qp_minus26 + slice_qp_delta;
	QPY_prev = SliceQPY;



	if ((SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP || (SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		if ((SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP)
		{
			sp_for_switch_flag = bs.readBit(); //2 u(1)
		}
		slice_qs_delta = bs.readSE(); //2 se(v)
	}
	//QSY ��ֵ���ڽ��� mb_type ��SI ��SI ���������к���Լ�Ԥ��ģʽΪ֡���SP ���������к�顣
	QSY = 26 + pps.pic_init_qs_minus26 + slice_qs_delta;
	//�Ƿ����ȥ���˲������������Ϣ =1������Ӧȥ���˲��� =0û����Ӧ��Ϣ
	if (pps.deblocking_filter_control_present_flag)
	{
		disable_deblocking_filter_idc = bs.readUE(); //2 ue(v)

		if (disable_deblocking_filter_idc != 1)
		{
			slice_group_change_cycle = bs.readSE(); //2 se(v)
			slice_beta_offset_div2 = bs.readSE(); //2 se(v)
		}
	}





	//ָ������Ƭ��Ĵ�С����ÿ��ͼ����˵����ͬ��ʱ��,����Ƭ����Ŀ������ĳ���.slice_group_change_rate_minus1ȡֵ��ΧΪ0��PicSizeInMapUnits-1:
	SliceGroupChangeRate = pps.slice_group_change_rate_minus1 + 1;


	if (pps.num_slice_groups_minus1 > 0 && pps.slice_group_map_type >= 3 && pps.slice_group_map_type <= 5)
	{
		int v = h264_log2(sps.PicSizeInMapUnits / SliceGroupChangeRate + 1); // Ceil( Log2( PicSizeInMapUnits �� SliceGroupChangeRate + 1 ) );
		/* Ƭ������ slice_group_map_type ��3.4.5ʱ���ɸ�Ԫ�ؿ��Ի�ȡƬ����ӳ�䵥Ԫ����Ŀ */
		slice_group_change_cycle = bs.readMultiBit(v); //2 u(v)
	}



	//�Ƿ���֡������Ӧ����
	MbaffFrameFlag = (sps.mb_adaptive_frame_field_flag && !field_pic_flag);

	//���ж��ٺ��
	PicHeightInMbs = sps.PicHeightInMapUnits;
	//�ܹ��ж��ٺ��
	PicSizeInMbs = sps.PicSizeInMapUnits;
	/*
	PicHeightInSamplesL = PicHeightInMbs * 16;
	PicHeightInSamplesC = PicHeightInMbs * m_sps.MbHeightC;

	MaxPicNum = (field_pic_flag == 0) ? m_sps.MaxFrameNum : (2 * m_sps.MaxFrameNum);
	CurrPicNum = (field_pic_flag == 0) ? frame_num : (2 * frame_num + 1);
	MapUnitsInSliceGroup0 = MIN(slice_group_change_cycle * SliceGroupChangeRate, m_sps.PicSizeInMapUnits);


	FilterOffsetA = slice_alpha_c0_offset_div2 << 1;
	FilterOffsetB = slice_beta_offset_div2 << 1;*/


	if (mapUnitToSliceGroupMap == nullptr)
	{
		//PicSizeInMapUnits �ܹ��ж��ٺ��
		mapUnitToSliceGroupMap = new uint32_t[PicSizeInMbs]();

		if (mapUnitToSliceGroupMap == nullptr)
		{
			printError("����ʧ�� mapUnitToSliceGroupMap");
			return -1;
		}
		setMapUnitToSliceGroupMap();
	}



	//�������ڶ�ÿ��Ƭ����֮ǰ��������Ҫ���ڵ�ǰ��Чͼ���������������Ƭͷ���������Ƭ��ӳ��ͼ���� MbToSliceGroupMap
	if (MbToSliceGroupMap == nullptr)
	{
		//�ڵõ� mapUnitToSliceGroupMap ֮��,Ҫ����8.2.2.8�й涨�Ĺ��̣�
		//��ӳ�䵥Ԫ���������ӳ��mapUnitToSliceGroupMap, ת��Ϊ��鵽�������ӳ�� MbToSliceGroupMap
		MbToSliceGroupMap = new uint32_t[PicSizeInMbs]();
		if (MbToSliceGroupMap == nullptr)
		{
			printError("����ʧ�� MbToSliceGroupMap");
			return -1;
		}
		//���ڼ������nextMbAddress
		setMbToSliceGroupMap();

	}

	//TU��transform unit����д���Ǳ任�����ĵ�λ�������DCT�任��һ��������Ԫ����

	//ScalingList �洢ÿ��TU��Ԫ��ÿ�����Ӧ��������������ϵ��
	setScallingList();
	return false;
}

bool SliceHeader::dec_ref_pic_marking(BitStream& bs)
{
	if (nalu.IdrPicFlag)
	{
		//֡������ʱ����Ϊ�ο�֡�ı��
		no_output_of_prior_pics_flag = bs.readBit(); //2 | 5 u(1)
		long_term_reference_flag = bs.readBit(); //2 | 5 u(1)
	}
	return false;
}

bool SliceHeader::pred_weight_table(BitStream& bs)
{
	int ret = 0;
	int32_t i = 0;
	int32_t j = 0;


	luma_log2_weight_denom = bs.readUE(); //2 ue(v)
	if (sps.ChromaArrayType != 0)
	{
		chroma_log2_weight_denom = bs.readUE(); //2 ue(v)
	}

	for (i = 0; i <= num_ref_idx_l0_active_minus1; i++)
	{
		luma_weight_l0[i] = 1 << luma_log2_weight_denom; //When luma_weight_l0_flag is equal to 0
		luma_offset_l0[i] = 0; //When luma_weight_l0_flag is equal to 0

		luma_weight_l0_flag = bs.readBit(); //2 u(1)
		if (luma_weight_l0_flag)
		{
			luma_weight_l0[i] = bs.readSE(); //2 se(v)
			luma_offset_l0[i] = bs.readSE(); //2 se(v)
		}

		if (sps.ChromaArrayType != 0)
		{
			chroma_weight_l0[i][0] = 1 << chroma_log2_weight_denom; //When chroma_weight_l0_flag is equal to 0
			chroma_weight_l0[i][1] = 1 << chroma_log2_weight_denom; //When chroma_weight_l0_flag is equal to 0
			chroma_offset_l0[i][0] = 0;
			chroma_offset_l0[i][1] = 0;

			chroma_weight_l0_flag = bs.readBit(); //2 u(1)
			if (chroma_weight_l0_flag)
			{
				for (j = 0; j < 2; j++)
				{
					chroma_weight_l0[i][j] = bs.readSE(); //2 se(v)
					chroma_offset_l0[i][j] = bs.readSE(); //2 se(v)
				}
			}
		}
	}

	if (slice_type % 5 == 1)
	{
		for (i = 0; i <= num_ref_idx_l1_active_minus1; i++)
		{
			luma_weight_l1[i] = 1 << luma_log2_weight_denom; //When luma_weight_l1_flag is equal to 0
			luma_offset_l1[i] = 0; //When luma_weight_l1_flag is equal to 0

			luma_weight_l1_flag = bs.readBit(); //2 u(1)
			if (luma_weight_l1_flag)
			{
				luma_weight_l1[i] = bs.readSE(); //2 se(v)
				luma_offset_l1[i] = bs.readSE(); //2 se(v)
			}

			if (sps.ChromaArrayType != 0)
			{
				chroma_weight_l1[i][0] = 1 << chroma_log2_weight_denom; //When chroma_weight_l1_flag is equal to 0
				chroma_weight_l1[i][1] = 1 << chroma_log2_weight_denom; //When chroma_weight_l1_flag is equal to 0
				chroma_offset_l1[i][0] = 0;
				chroma_offset_l1[i][1] = 0;

				chroma_weight_l1_flag = bs.readBit(); //2 u(1)
				if (chroma_weight_l1_flag)
				{
					for (j = 0; j < 2; j++)
					{
						chroma_weight_l1[i][j] = bs.readSE(); //2 se(v)
						chroma_offset_l1[i][j] = bs.readSE(); //2 se(v)
					}
				}
			}
		}
	}
	return false;
}

bool SliceHeader::ref_pic_list_modification(BitStream& bs)
{
	if (slice_type % 5 != 2 && slice_type % 5 != 4) //H264_SLIECE_TYPE_I = 2; H264_SLIECE_TYPE_SI = 4;
	{
		ref_pic_list_modification_flag_l0 = bs.readBit(); //2 u(1)
		if (ref_pic_list_modification_flag_l0)
		{
			int32_t i = 0;
			do
			{
				if (i >= 32)
				{
					//LOG_ERROR("modification_of_pic_nums_idc[0][i]: i=%d, must be in [0,31]\n", i);
					printError("i������0-31");
					return -1;
				}

				modification_of_pic_nums_idc[0][i] = bs.readUE(); //2 ue(v)
				if (modification_of_pic_nums_idc[0][i] == 0 || modification_of_pic_nums_idc[0][i] == 1)
				{
					abs_diff_pic_num_minus1[0][i] = bs.readUE(); //2 ue(v)
				}
				else if (modification_of_pic_nums_idc[0][i] == 2)
				{
					long_term_pic_num[0][i] = bs.readUE(); //2 ue(v)
				}
				i++;
			} while (modification_of_pic_nums_idc[0][i - 1] != 3);
			ref_pic_list_modification_count_l0 = i;
		}
	}

	if (slice_type % 5 == 1) //H264_SLIECE_TYPE_B = 1;
	{
		ref_pic_list_modification_flag_l1 = bs.readBit(); //2 u(1)
		if (ref_pic_list_modification_flag_l1)
		{
			int32_t i = 0;
			do
			{
				if (i >= 32)
				{
					printError("i������0-31");
					return -1;
				}

				modification_of_pic_nums_idc[1][i] = bs.readUE(); //2 ue(v)
				if (modification_of_pic_nums_idc[1][i] == 0 || modification_of_pic_nums_idc[1][i] == 1)
				{
					abs_diff_pic_num_minus1[1][i] = bs.readUE(); //2 ue(v)
				}
				else if (modification_of_pic_nums_idc[1][i] == 2)
				{
					long_term_pic_num[1][i] = bs.readUE(); //2 ue(v)
				}
				i++;
			} while (modification_of_pic_nums_idc[1][i - 1] != 3);
			ref_pic_list_modification_count_l1 = i;
		}
	}
	return false;
}

bool SliceHeader::setMapUnitToSliceGroupMap()
{
	//���к������Ƭ��0
	if (pps.num_slice_groups_minus1 == 0)
	{
		for (size_t i = 0; i < sps.PicSizeInMapUnits; i++)
		{
			mapUnitToSliceGroupMap[i] = 0;
		}
	}


	//  FMO  ����ӳ��
	return true;
}
bool SliceHeader::setMbToSliceGroupMap()
{
	int32_t i = 0;

	for (i = 0; i < PicSizeInMbs; i++)
	{
		if (sps.frame_mbs_only_flag || field_pic_flag)//���������֡����
		{
			MbToSliceGroupMap[i] = mapUnitToSliceGroupMap[i];
		}
		else if (MbaffFrameFlag)//֡������Ӧ
		{
			MbToSliceGroupMap[i] = mapUnitToSliceGroupMap[i / 2];
		}
		else//�Ȳ���֡���룬Ҳ���ǳ����룬Ҳ����֡������Ӧ
		{
			MbToSliceGroupMap[i] = mapUnitToSliceGroupMap[(i / (2 * sps.PicWidthInMbs)) * sps.PicWidthInMbs + (i % sps.PicWidthInMbs)];
		}
	}
	return true;
}
void SliceHeader::setScallingList()
{
	if (!sps.seq_scaling_matrix_present_flag && !pps.pic_scaling_matrix_present_flag)
	{
		constexpr int Flat_4x4_16[16] = { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 };
		constexpr int Flat_8x8_16[64] = {
				16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
				16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
				16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
				16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		};
		//���������δ�������ž���ֵ�������ž���ֵȫ��Ĭ��Ϊ16

		//4x4TU���������� Ӧ����4x4intraYUV��4x4inter YUV
		for (size_t i = 0; i < 12; i++)
		{
			if (i < 6)
			{
				memcpy(ScalingList4x4[i], Flat_4x4_16, sizeof(int) * 16);
			}
			else
			{
				memcpy(ScalingList8x8[i - 6], Flat_8x8_16, sizeof(int) * 64);
			}
		}
	}
	else
	{
		if (sps.seq_scaling_matrix_present_flag)
		{
			memcpy(ScalingList4x4, sps.ScalingList4x4, sizeof(int) * 6 * 16);
			memcpy(ScalingList8x8, sps.ScalingList8x8, sizeof(int) * 6 * 64);
		}

		//pps�����ֵ�����ܻḲ��֮ǰsps�õ���ֵ
		if (pps.pic_scaling_matrix_present_flag)
		{
			memcpy(ScalingList4x4, pps.ScalingList4x4, sizeof(int) * 6 * 16);
			memcpy(ScalingList8x8, pps.ScalingList8x8, sizeof(int) * 6 * 64);
		}
	}

}
SliceHeader::~SliceHeader()
{


	if (mapUnitToSliceGroupMap != nullptr)
	{
		delete[] mapUnitToSliceGroupMap;
		mapUnitToSliceGroupMap = nullptr;
	}

	if (MbToSliceGroupMap != nullptr)
	{
		delete[] MbToSliceGroupMap;
		MbToSliceGroupMap = nullptr;
	}

}