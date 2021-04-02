#include "ParseSlice.h"




ParseSlice::ParseSlice(ParseNalu& nalu):nalu(nalu)
{

	//memset(this, 0, sizeof(ParseSlice));
	/*this->ppsGather = pps;
	this->spsGather = sps;*/
	
}




bool ParseSlice::slice_header(BitStream& bs, const ParsePPS ppsCache[256], const ParseSPS spsCache[32])
{


	//������Ա�ʾ��������� Slice �е�һ���������
	first_mb_in_slice = bs.readUE(); //2 ue(v)
	//slice������
	slice_type = bs.readUE(); //2 ue(v)
	//������pps id
	pic_parameter_set_id = bs.readUE(); //2 ue(v)
	
	//bool flag = getppsAndSps();

	pps = ppsCache[pic_parameter_set_id];
	sps = spsCache[pps.seq_parameter_set_id];




	if (sps.separate_colour_plane_flag)
	{
		//�������һ����ɫ�ķ���
		//���ʾ UV �� Y �ֿ����롣�����ڷֿ������ģʽ�����ǲ��ú͵�ɫģʽһ���Ĺ���
		colour_plane_id = bs.readMultiBit(2); //2 u(2)
	}
	// ����һ��ͼ���ʶ �� ���ڱ�������Ӧ�� log2_max_frame_num_minus4 + 4 �����ر�ʾ
	frame_num = bs.readMultiBit(sps.log2_max_frame_num_minus4 + 4); //2 u(v)
	//m_picture_coded_type = H264_PICTURE_CODED_TYPE_FRAME;
	//�Ƿ�Ϊ֡����
	if (!sps.frame_mbs_only_flag)
	{
		//���� 1 ��ʾ��������һ�����볡��������
		//field_pic_flag ���� 0 ��ʾ��������һ������֡����������field_pic_flag ������ʱ��Ӧ�ƶ���ֵΪ0��
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
		printError("ref_pic_list_modification ����");
		return -1;
		//ret = ref_pic_list_modification(bs);
		//RETURN_IF_FAILED(ret != 0, ret);
	}

	/*if ((pps.weighted_pred_flag && ((SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_P || (SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP))
		|| (pps.weighted_bipred_idc == 1 && (SLIECETYPE)slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
		)
	{
		ret = pred_weight_table(bs);
		RETURN_IF_FAILED(ret != 0, ret);
	}*/

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
	//��ʾ���������е����к��� QPY  �ĳ�ʼֵ����ֵ�ں��㽫�� mb_qp_delta ��ֵ�޸ġ��� ������ʼ  QPY ��������������Ĺ�ʽ���㣺
	//SliceQPY = 26 + pic_init_qp_minus26 + slice_qp_delta
	slice_qp_delta = bs.readSE();
	return false;
}

bool ParseSlice::dec_ref_pic_marking(BitStream& bs)
{


	return false;
}

ParseSlice::~ParseSlice()
{
}
