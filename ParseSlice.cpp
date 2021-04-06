#include "ParseSlice.h"




ParseSlice::ParseSlice(ParseNalu& nalu):nalu(nalu)
{

	//memset(this, 0, sizeof(ParseSlice));
	/*this->ppsGather = pps;
	this->spsGather = sps;*/
	
}




bool ParseSlice::slice_header(BitStream& bs, const ParsePPS ppsCache[256], const ParseSPS spsCache[32])
{


	//这个属性表示的是在这个 Slice 中第一个宏块的序号
	first_mb_in_slice = bs.readUE(); //2 ue(v)
	//slice的类型
	slice_type = bs.readUE(); //2 ue(v)
	//依赖的pps id
	pic_parameter_set_id = bs.readUE(); //2 ue(v)
	
	//bool flag = getppsAndSps();

	pps = ppsCache[pic_parameter_set_id];
	sps = spsCache[pps.seq_parameter_set_id];




	if (sps.separate_colour_plane_flag)
	{
		//处理的哪一个颜色的分量
		//则表示 UV 与 Y 分开编码。而对于分开编码的模式，我们采用和单色模式一样的规则。
		colour_plane_id = bs.readMultiBit(2); //2 u(2)
	}
	// 用作一个图像标识 符 ，在比特流中应由 log2_max_frame_num_minus4 + 4 个比特表示
	frame_num = bs.readMultiBit(sps.log2_max_frame_num_minus4 + 4); //2 u(v)
	//m_picture_coded_type = H264_PICTURE_CODED_TYPE_FRAME;
	//是否为帧编码
	if (!sps.frame_mbs_only_flag)
	{
		//等于 1 表示该条带是一个编码场的条带。
		//field_pic_flag 等于 0 表示该条带是一个编码帧的条带。当field_pic_flag 不存在时，应推定其值为0。
		field_pic_flag = bs.readBit(); //2 u(1)
		//m_picture_coded_type = H264_PICTURE_CODED_TYPE_TOP_FIELD;
		if (field_pic_flag)
		{
			//等于1表示该条带是一个编码底场的一部分。bottom_field_flag 等于 0 表示该图像是一个编码的顶场。
			//当该条带不存在此语法元素时，应推定其值为0
			bottom_field_flag = bs.readBit(); //2 u(1)
			if (bottom_field_flag)
			{
				//m_picture_coded_type = H264_PICTURE_CODED_TYPE_BOTTOM_FIELD;
			}
		}
	}
	//是否是立即刷新帧的标记
	if (nalu.IdrPicFlag)
	{
		idr_pic_id = bs.readUE();
	}

	/*如果 pic_order_cnt_type等于0，
		应该是按照解码顺序列表中第一个图像之后的所有其它图像，
		这些图像不能是由 8.2.5.2节所述frame_num间隙的解码过程中得到的非"不存在"图像，
		或者是解码顺序 中当前图像之前的图像或者是当前图像。当pic_order_cnt_type等于0，
		并且当前图像不是 8.2.5.2节规 定的frame_num间隙的解码过程中得到的"不存在"图像时，
		当前图像被包含在在解码参考图像标记过程 之前的listD中。*/
	if (sps.pic_order_cnt_type == 0)
	{
		/*表示一个编码帧的顶场或一个编码场的图像顺序数对 MaxPicOrderCntLsb  取模。 
			pic_order_cnt_lsb 语法元素的大小是log2_max_pic_order_cnt_lsb_minus4 + 4 个比特。
			pic_order_cnt_lsb 的值应该在 0 到 MaxPicOrderCntLsb – 1 的范围内（包括边界值）。*/
		pic_order_cnt_lsb = bs.readMultiBit(sps.log2_max_pic_order_cnt_lsb_minus4 + 4); //2 u(v)
		if (pps.bottom_field_pic_order_in_frame_present_flag && !field_pic_flag)
		{
			//表示一个编码帧的底场和顶场的图像顺序数之间的差
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
		/*printError("ref_pic_list_modification 错误");
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
	//熵编码使用的方法，不同的语法元素在不同模式选择的方式不同的
	//等于0，那么采用语法表中左边的描述符所指定的方法
	//等于1，就采用语法表中右边的描述符所指定的方法
	if (pps.entropy_coding_mode_flag && (SLIECETYPE)slice_type != SLIECETYPE::H264_SLIECE_TYPE_I && (SLIECETYPE)slice_type != SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		//表示用于决定关联变量的初始化过程中使用的初始化表格的序号。变量 cabac_init_idc 的值应 该在0 到  2 的范围内（包括0 和2）
		cabac_init_idc = bs.readUE(); //2 ue(v)
	}
	//表示用于条带中的所有宏块的 QPY  的初始值，该值在宏块层将被 mb_qp_delta 的值修改。该 条带初始  QPY 量化参数按下面的公式计算：
	//SliceQPY = 26 + pic_init_qp_minus26 + slice_qp_delta
	slice_qp_delta = bs.readSE();
	return false;
}

bool ParseSlice::dec_ref_pic_marking(BitStream& bs)
{

	if (nalu.IdrPicFlag)
	{
		//帧间编码的时候作为参考帧的标记
		no_output_of_prior_pics_flag = bs.readBit(); //2 | 5 u(1)
		long_term_reference_flag = bs.readBit(); //2 | 5 u(1)
	}
	return false;
}
bool ParseSlice::pred_weight_table(BitStream& bs)
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
//帧间参考作用的 参考帧列表
bool ParseSlice::ref_pic_list_modification(BitStream& bs)
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
					printError("i必须在0-31");
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
					printError("i必须在0-31");
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

ParseSlice::~ParseSlice()
{
}
