#pragma once
#include "Common.h"
#include "ParseSPS.h"



//图像参数集
class ParsePPS
{
public:
	ParseSPS     sps;


	uint32_t     pic_parameter_set_id; // 1 ue(v)
	uint32_t     seq_parameter_set_id; // 1 ue(v)
	bool     entropy_coding_mode_flag; // 1 u(1)
	bool     bottom_field_pic_order_in_frame_present_flag; // 1 u(1)
	uint32_t     num_slice_groups_minus1; // 1 ue(v)
	uint32_t     slice_group_map_type; // 1 ue(v)    [A.2.1 Baseline profile] num_slice_groups_minus1 in the range of 0 to 7, inclusive.
	uint32_t     run_length_minus1[8]; //[ iGroup ] 1 ue(v)
	uint32_t     top_left[8]; //[ iGroup ] 1 ue(v)
	uint32_t     bottom_right[8]; //[ iGroup ] 1 ue(v)
	bool     slice_group_change_direction_flag; // 1 u(1)
	//用以指明变量 SliceGroupChangeRAte 
	uint32_t     slice_group_change_rate_minus1; // 1 ue(v)
	uint32_t     pic_size_in_map_units_minus1; // 1 ue(v)
	uint32_t* slice_group_id; //[ i ] 1 u(v)
	uint32_t     num_ref_idx_l0_default_active_minus1; // 1 ue(v)
	uint32_t     num_ref_idx_l1_default_active_minus1; // 1 ue(v)
	bool     weighted_pred_flag; // 1 u(1)
	uint32_t     weighted_bipred_idc; // 1 u(2)
	int     pic_init_qp_minus26; // /* relative to 26 */ 1 se(v)
	int     pic_init_qs_minus26; // /* relative to 26 */ 1 se(v)
	int     chroma_qp_index_offset; // 1 se(v) 
	//slice header中是否存在去块滤波器控制相关信息 =1存在响应去块滤波器 =0没有相应信息
	bool     deblocking_filter_control_present_flag; // 1 u(1)
	bool     constrained_intra_pred_flag; // 1 u(1)
	bool     redundant_pic_cnt_present_flag; // 1 u(1)
	bool     transform_8x8_mode_flag; // 1 u(1)
	bool     pic_scaling_matrix_present_flag; // 1 u(1)
	int      pic_scaling_list_present_flag[12]; //[ i ] 1 u(1)
	int		 second_chroma_qp_index_offset; // 1 se(v) //-12 到 +12

	int32_t     ScalingList4x4[6][16];
	int32_t     ScalingList8x8[6][64];
	int32_t     UseDefaultScalingMatrix4x4Flag[6];
	int32_t     UseDefaultScalingMatrix8x8Flag[6];
public:
	ParsePPS();

	bool pic_parameter_set_rbsp(BitStream& bs, const ParseSPS spsCache[32]);
	~ParsePPS();
};

