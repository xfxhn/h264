#include "ParsePPS.h"



/// <summary>
/// 关于片组的概念：一个片组由若干个宏块组成，可以按照某种规则将一个图像中的某些宏块（这些宏块可以在图像中的任意位置，不是必须相邻）划分成一个组，就是一个片组。
/// 这也就是说，图像由若干“片组”组成，片组由若干“片”组成。而我们之前一直说的“图像由若干‘片’组成”，其实是针对不使用FMO的情况，
/// 当不使用FMO时，一个图像中默认只有一个片组，该片组包含整个图像的所有宏块（一般都是只有一个片组）。
/// </summary>
ParsePPS::ParsePPS()
{
	pic_parameter_set_id = 0;
	seq_parameter_set_id = 0;
	entropy_coding_mode_flag = 0;
	bottom_field_pic_order_in_frame_present_flag = 0;
	num_slice_groups_minus1 = 0;
	slice_group_map_type = 0;
	memset(run_length_minus1, 0, sizeof(int32_t) * 8);
	memset(top_left, 0, sizeof(int32_t) * 8);
	memset(bottom_right, 0, sizeof(int32_t) * 8);
	slice_group_change_direction_flag = 0;
	slice_group_change_rate_minus1 = 0;
	pic_size_in_map_units_minus1 = 0;
	slice_group_id = NULL;
	num_ref_idx_l0_default_active_minus1 = 0;
	num_ref_idx_l1_default_active_minus1 = 0;
	weighted_pred_flag = 0;
	weighted_bipred_idc = 0;
	pic_init_qp_minus26 = 0;
	pic_init_qs_minus26 = 0;
	chroma_qp_index_offset = 0;
	deblocking_filter_control_present_flag = 0;
	constrained_intra_pred_flag = 0;
	redundant_pic_cnt_present_flag = false;
	transform_8x8_mode_flag = 0;
	pic_scaling_matrix_present_flag = false;
	memset(pic_scaling_list_present_flag, 0, sizeof(int32_t) * 12);
	second_chroma_qp_index_offset = 0;
	memset(ScalingList4x4, 0, sizeof(int32_t) * 6 * 16);
	memset(ScalingList8x8, 0, sizeof(int32_t) * 6 * 64);
}


bool ParsePPS::pic_parameter_set_rbsp(BitStream& bs, const ParseSPS spsCache[32])
{
	int32_t iGroup = 0;
	pic_parameter_set_id = bs.readUE(); //1 ue(v)
	//sps ID通过这个id可以找到所引用的sps结构
	seq_parameter_set_id = bs.readUE(); //1 ue(v)

	sps = spsCache[seq_parameter_set_id];
	//熵编码使用的方法，不同的语法元素在不同模式选择的方式不同的
	//等于0，那么采用语法表中左边的描述符所指定的方法
	//等于1，就采用语法表中右边的描述符所指定的方法
	entropy_coding_mode_flag = bs.readBit(); //1 u(1)
	//标识位，用于表示另外条带头中的两个语法元素delta_pic_order_cnt_bottom和delta_pic_order_cn是否存在的标识。
	//这两个语法元素表示了某一帧的底场的POC的计算方法。
	bottom_field_pic_order_in_frame_present_flag = bs.readBit(); //1 u(1)


	//slice groups数量减去1，为0的是时候这个slice属于所有slice group，大于0被分割成多个slice group
	//num_slice_groups_minus1等于0，图像只有一个片组时，则不启用FMO；其他情况下，一个图像中有多个片组，这时都使用FMO。
	//slice group表示一帧中红快的组成方式
	//加1表示一个图像中的条带组数。当 num_slice_groups_minus1 等于 0 时，图像中 所有的条带属于同一个条带组。
	num_slice_groups_minus1 = bs.readUE(); //1 ue(v)

	//图像参数集应包括取值范围0-7的num_slice_groups_minus1参数，包括0与7；
	//片组模式
	if (num_slice_groups_minus1 > 0)
	{
		//MFO前面的slice内部宏块总是连续的。FMO技术通过映射表（macroblock to slice group map）决定具体每个宏块属于哪个slice groups，突破了宏块连续的限制。

		/*slice groups又可以按扫描顺序分成多个slice，当不使用FMO时，就相当于整个图像是一个slice groups的情形。由于各slice groups的解码也是独立的。
		当码流传输错误的时候，解码端可以根据邻近正确解码的其他slice groups宏块，进行错误隐藏。*/

		/*slice_group_map_type 等于 0 表示隔行扫描的条带组。
		  slice_group_map_type 等于 1 表示一种分散的条带组映射。
		  slice_group_map_type 等于 2 表示一个或多个前景条带组和一个残余条带组。
		  slice_group_map_type 的值等于 3、4 和 5 表示变换的条带组。当 num_slice_groups_minus1 不等于 1 时， slice_group_map_type 不应等于3、4 或  5。
		  slice_group_map_type 等于 6 表示对每个条带组映射单元清楚地分配一个条带组。*/

		  //slice_group_map_type可以指定使用哪个规则为当前图像划分片组
		  //如果这六种默认的规则都不能满足要求，还可以定制自己的映射规则。
		slice_group_map_type = bs.readUE(); //1 ue(v)
		if (slice_group_map_type == 0)
		{
			for (iGroup = 0; iGroup <= num_slice_groups_minus1; iGroup++)
			{
				//用来指定条带组映射单元的光栅扫描顺序中分配给第 i 个条带组的连续条带组映射单 元的数目。
				//run_length_minus1[ i ] 的取值范围应该在0 到  PicSizeInMapUnits – 1 内（包括边界值）。
				run_length_minus1[iGroup] = bs.readUE(); //1 ue(v)
			}
		}
		else if (slice_group_map_type == 2)
		{
			for (iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++)
			{
				top_left[iGroup] = bs.readUE(); //1 ue(v)
				bottom_right[iGroup] = bs.readUE(); //1 ue(v)
			}
		}
		else if (slice_group_map_type == 3 || slice_group_map_type == 4 || slice_group_map_type == 5)
		{
			slice_group_change_direction_flag = bs.readBit(); //1 u(1)
			//用以指明变量 SliceGroupChangeRAte 
			slice_group_change_rate_minus1 = bs.readUE(); //1 ue(v)
		}
		else if (slice_group_map_type == 6)
		{
			pic_size_in_map_units_minus1 = bs.readUE(); //1 ue(v)
			//slice_group_id = (int32_t*)malloc(pic_size_in_map_units_minus1 + 1); // PicSizeInMapUnits − 1 = PicWidthInMbs * PicHeightInMapUnits - 1;
			slice_group_id = new uint32_t[pic_size_in_map_units_minus1 + 1];


			/*slice_group_id[i] 表示光栅扫描顺序中的第 i 个条带组映射单元的一个条带组。
			  slice_group_id[i] 语法元素的大小是 Ceil(Log2(num_slice_groups_minus1 + 1)) 比特。
			  slice_group_id[i] 的 值 应 该 在 0 到 num_slice_groups_minus1 范围内（包括边界值）。*/

			int v = h264_log2(num_slice_groups_minus1 + 1); //Ceil( Log2( num_slice_groups_minus1 + 1 ) );
			for (size_t i = 0; i <= pic_size_in_map_units_minus1; i++)
			{
				slice_group_id[i] = bs.readMultiBit(v); //1 u(v)    v = Ceil( Log2( num_slice_groups_minus1 + 1 ) ) bits.
			}
		}
	}

	//表示当Slice Header中的num_ref_idx_active_override_flag标识位为0时，
	//P/SP/B slice的语法元素num_ref_idx_l0_active_minus1和num_ref_idx_l1_active_minus1的默认值。
	num_ref_idx_l0_default_active_minus1 = bs.readUE(); //1 ue(v)
	num_ref_idx_l1_default_active_minus1 = bs.readUE(); //1 ue(v)

	//是否开启加权预测
	weighted_pred_flag = bs.readBit(); //1 u(1)
	//加权预测的索引
	weighted_bipred_idc = bs.readMultiBit(2); //1 u(2)

   /* 表示每个条带的SliceQPY 初始值减 26。
	  当解码非0 值的slice_qp_delta 时，该初始值在 条带层被修正，
	  并且在宏块层解码非 0 值的 mb_qp_delta 时进一步被修正。pic_init_qp_minus26 的值应该在－ (26 + QpBdOffsetY ) 到 +25 之间（包括边界值）。*/
	pic_init_qp_minus26 = bs.readSE(); // /* relative to 26 */ 1 se(v)
	//表示在 SP 或 SI 条带中的所有宏块的 SliceQSY初始值减26
	pic_init_qs_minus26 = bs.readSE(); // /* relative to 26 */ 1 se(v)
	//计算色度量化参数的偏移量值  范围：-22 ---  22
	chroma_qp_index_offset = bs.readSE(); //1 se(v)
	//slice header中是否存在去块滤波器控制相关信息 =1存在响应去块滤波器 =0没有相应信息
	deblocking_filter_control_present_flag = bs.readBit(); //1 u(1)
	//帧内预测参考数据来源 =1某一个宏块进行帧内预测的时候只能采用 

	/*在 P 和 B 片中，帧内编码的宏块的邻近宏块可能是采用的帧间编码。
		当本句法元素等于 1 时，表示帧内编码的宏块不能用帧间编码的宏块的像素作为自己的预测，
		即帧内编码的宏块只能用邻近帧内编码的宏块的像素作为自己的预测；而本句法元素等于 0 时，表示不存在这种限制。*/
	constrained_intra_pred_flag = bs.readBit(); //1 u(1)
	//等于0表示redundant_pic_cnt语法元素不会在条带头、图像参数集中指明（直接或与相应的数据分割块A关联）的数据分割块 B 和数据分割块 C 中出现。
	//redundant_pic_cnt_present_flag 等 于 1 表示 redundant_pic_cnt语法元素将出现在条带头、
	//图像参数集中指明（直接或与相应的数据分割块 A 关联的数据分割块 B 和数据分割块 C 中。
	redundant_pic_cnt_present_flag = bs.readBit(); //1 u(1)

	//当second_chroma_qp_index_offset 不存在时，默认其值等于 chroma_qp_index_offset
	second_chroma_qp_index_offset = chroma_qp_index_offset;
	if (bs.more_rbsp_data())
	{
		//等于1表示8x8变换解码过程可能正在使用（参见 8.5 节）。transform_8x8_mode_flag 等于 0 表示未使用 8x8 变换解码过程。
		//当 transform_8x8_mode_flag 不存在时，默认其值 为0。
		transform_8x8_mode_flag = bs.readBit();


		//等于 1 表示存在用来修改在序列参数集中指定的缩放比例列表的参数。 
		//pic_scaling_matrix_present_flag 等于 0 表示用于该图像中的缩放比例列表应等于那些由序列参数集规定的。
		//当 pic_scaling_matrix_present_flag 不存在时，默认其值为0。
		pic_scaling_matrix_present_flag = bs.readBit();

		if (pic_scaling_matrix_present_flag)
		{
			for (size_t i = 0; i < 6 + ((sps.chroma_format_idc != 3) ? 2 : 6) * transform_8x8_mode_flag; i++)
			{

				/*等于1表示存在缩放比例列表的语法结构并用于指定序号为i的缩放比例列 表。
				pic_scaling_list_present_flag[i]等于0 表示在图像参数集中不存在缩放比例列表 i 的语法结构，
				并且根据 seq_scaling_matrix_present_flag 的值，应用下列条款*/
				pic_scaling_list_present_flag[i] = bs.readBit(); //1 u(1)


				if (pic_scaling_list_present_flag[i] == 1)
				{
					if (i < 6)
					{
						bool ret = scaling_list(bs, ScalingList4x4[i], 16, UseDefaultScalingMatrix4x4Flag[i]);
						//RETURN_IF_FAILED(ret != 0, ret);
					}
					else
					{
						bool ret = scaling_list(bs, ScalingList8x8[i - 6], 64, UseDefaultScalingMatrix8x8Flag[i - 6]);
						//RETURN_IF_FAILED(ret != 0, ret);
					}
				}
			}
			//表示为在 QPC 值的表格中寻找 Cr 色度分量而应加到参数 QPY 和 QSY 上的偏移。second_chroma_qp_index_offset 的值应在-12 到 +12 范围内（包括边界值）
			second_chroma_qp_index_offset = bs.readSE();
		}
	}

	return false;
}

ParsePPS::~ParsePPS()
{
	if (slice_group_id)
	{
		/*my_free(slice_group_id);*/
		delete[] slice_group_id;
		slice_group_id = NULL;
	}
}
