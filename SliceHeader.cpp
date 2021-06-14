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
	//这个属性表示的是在这个 Slice 中第一个宏块的序号
	first_mb_in_slice = bs.readUE(); //2 ue(v)
	//slice的类型
	slice_type = bs.readUE() % 5; //2 ue(v)
	//依赖的pps id
	pic_parameter_set_id = bs.readUE(); //2 ue(v)

	pps = ppsCache[pic_parameter_set_id];
	sps = spsCache[pps.seq_parameter_set_id];


	if (sps.separate_colour_plane_flag)
	{
		//指定与当前切片RBSP关联的颜色平面。colour_plane_id的值应该在0到2的范围内，包括。
		//colour_plane_id等于0、1和2分别对应于Y、Cb和Cr平面。
		colour_plane_id = bs.readMultiBit(2); //2 u(2)
	}
	// 用作一个图像标识符 ，在比特流中应由 log2_max_frame_num_minus4 + 4 个比特表示
	frame_num = bs.readMultiBit(sps.log2_max_frame_num_minus4 + 4); //2 u(v)
	//m_picture_coded_type = H264_PICTURE_CODED_TYPE_FRAME;


	//frame_mbs_only_flag是否为帧编码
	if (!sps.frame_mbs_only_flag)
	{
		//当这个句法元素取值为1时属于场编码； 0时为非场编码。当field_pic_flag 不存在时，应推定其值为0。
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
	//表示用于条带中的所有宏块的 QPY  的初始值，该值在宏块层将被 mb_qp_delta 的值修改。该条带初始 QPY 量化参数按下面的公式计算：
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
	//QSY 的值用于解码 mb_type 等SI 的SI 条带的所有宏块以及预测模式为帧间的SP 条带的所有宏块。
	QSY = 26 + pps.pic_init_qs_minus26 + slice_qs_delta;
	//是否存在去块滤波器控制相关信息 =1存在响应去块滤波器 =0没有相应信息
	if (pps.deblocking_filter_control_present_flag)
	{
		disable_deblocking_filter_idc = bs.readUE(); //2 ue(v)

		if (disable_deblocking_filter_idc != 1)
		{
			slice_group_change_cycle = bs.readSE(); //2 se(v)
			slice_beta_offset_div2 = bs.readSE(); //2 se(v)
		}
	}





	//指出当切片组的大小对于每个图像来说都不同的时候,对切片组数目的运算的乘数.slice_group_change_rate_minus1取值范围为0到PicSizeInMapUnits-1:
	SliceGroupChangeRate = pps.slice_group_change_rate_minus1 + 1;


	if (pps.num_slice_groups_minus1 > 0 && pps.slice_group_map_type >= 3 && pps.slice_group_map_type <= 5)
	{
		int v = h264_log2(sps.PicSizeInMapUnits / SliceGroupChangeRate + 1); // Ceil( Log2( PicSizeInMapUnits ÷ SliceGroupChangeRate + 1 ) );
		/* 片组类型 slice_group_map_type 是3.4.5时，由该元素可以获取片组中映射单元的数目 */
		slice_group_change_cycle = bs.readMultiBit(v); //2 u(v)
	}



	//是否是帧场自适应编码
	MbaffFrameFlag = (sps.mb_adaptive_frame_field_flag && !field_pic_flag);

	//高有多少宏块
	PicHeightInMbs = sps.PicHeightInMapUnits;
	//总共有多少宏块
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
		//PicSizeInMapUnits 总共有多少宏块
		mapUnitToSliceGroupMap = new uint32_t[PicSizeInMbs]();

		if (mapUnitToSliceGroupMap == nullptr)
		{
			printError("分配失败 mapUnitToSliceGroupMap");
			return -1;
		}
		setMapUnitToSliceGroupMap();
	}



	//解码器在对每个片解码之前，首先需要基于当前有效图像参数集和需解码的片头，产生宏块片组映射图变量 MbToSliceGroupMap
	if (MbToSliceGroupMap == nullptr)
	{
		//在得到 mapUnitToSliceGroupMap 之后,要调用8.2.2.8中规定的过程，
		//将映射单元到条带组的映射mapUnitToSliceGroupMap, 转变为宏块到条带组的映射 MbToSliceGroupMap
		MbToSliceGroupMap = new uint32_t[PicSizeInMbs]();
		if (MbToSliceGroupMap == nullptr)
		{
			printError("分配失败 MbToSliceGroupMap");
			return -1;
		}
		//用于计算变量nextMbAddress
		setMbToSliceGroupMap();

	}

	//TU是transform unit的缩写，是变换量化的单位（是针对DCT变换的一个基本单元），

	//ScalingList 存储每个TU单元中每个点对应的量化、反量化系数
	setScallingList();
	return false;
}

bool SliceHeader::dec_ref_pic_marking(BitStream& bs)
{
	if (nalu.IdrPicFlag)
	{
		//帧间编码的时候作为参考帧的标记
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

bool SliceHeader::setMapUnitToSliceGroupMap()
{
	//所有宏块属于片组0
	if (pps.num_slice_groups_minus1 == 0)
	{
		for (size_t i = 0; i < sps.PicSizeInMapUnits; i++)
		{
			mapUnitToSliceGroupMap[i] = 0;
		}
	}


	//  FMO  纠错映射
	return true;
}
bool SliceHeader::setMbToSliceGroupMap()
{
	int32_t i = 0;

	for (i = 0; i < PicSizeInMbs; i++)
	{
		if (sps.frame_mbs_only_flag || field_pic_flag)//场编码或者帧编码
		{
			MbToSliceGroupMap[i] = mapUnitToSliceGroupMap[i];
		}
		else if (MbaffFrameFlag)//帧场自适应
		{
			MbToSliceGroupMap[i] = mapUnitToSliceGroupMap[i / 2];
		}
		else//既不是帧编码，也不是场编码，也不是帧场自适应
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
		//如果编码器未给出缩放矩阵值，则缩放矩阵值全部默认为16

		//4x4TU的量化矩阵 应用于4x4intraYUV、4x4inter YUV
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

		//pps里面的值，可能会覆盖之前sps得到的值
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