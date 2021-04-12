#pragma once
#include "Common.h"
#include "ParsePPS.h"
#include "ParseSPS.h"
#include "ParseNalu.h"

class SliceHeader
{
public:
	ParseNalu& nalu;
	ParsePPS pps;
	ParseSPS sps;


	uint32_t     first_mb_in_slice; // 2 ue(v)
	uint32_t     slice_type; // 2 ue(v)
	uint32_t     pic_parameter_set_id; // 2 ue(v)
	uint32_t     colour_plane_id; // 2 u(2)
	uint32_t     frame_num; // 2 u(v)
	bool     field_pic_flag; // 2 u(1)
	bool     bottom_field_flag; // 2 u(1)
	uint32_t     idr_pic_id; // 2 ue(v)
	uint32_t     pic_order_cnt_lsb; // 2 u(v)
	int32_t     delta_pic_order_cnt_bottom; // 2 se(v)
	int32_t     delta_pic_order_cnt[2]; // 2 se(v)
	uint32_t     redundant_pic_cnt; // 2 ue(v)
	bool     direct_spatial_mv_pred_flag; // 2 u(1)
	bool     num_ref_idx_active_override_flag; // 2 u(1)
	uint32_t     num_ref_idx_l0_active_minus1; // 2 ue(v) [0,31]
	uint32_t     num_ref_idx_l1_active_minus1; // 2 ue(v) [0,31]
	uint32_t     cabac_init_idc; // 2 ue(v)
	int32_t     slice_qp_delta; // 2 se(v)
	uint32_t     sp_for_switch_flag; // 2 u(1)
	int32_t     slice_qs_delta; // 2 se(v)
	uint32_t     disable_deblocking_filter_idc; // 2 ue(v) 0：开启环路滤波，滤波可跨越slice边界。1：关闭环路滤波。2：开启环路滤波，只针对同一个slice滤波。
	int32_t     slice_alpha_c0_offset_div2; // 2 se(v)
	int32_t     slice_beta_offset_div2; // 2 se(v)
	uint32_t     slice_group_change_cycle; // 2 u(v)






	 // dec_ref_pic_marking
	bool     no_output_of_prior_pics_flag; // 2 | 5 u(1)
	bool     long_term_reference_flag; // 2 | 5 u(1)
	bool     adaptive_ref_pic_marking_mode_flag; // 2 | 5 u(1)
	//DEC_REF_PIC_MARKING    m_dec_ref_pic_marking[32];
	//int32_t     dec_ref_pic_marking_count; //m_dec_ref_pic_marking[]数组大小




	// ref_pic_list_modification//帧间参考作用的 参考帧列表
	bool     ref_pic_list_modification_flag_l0; // 2 u(1)
	uint32_t     modification_of_pic_nums_idc[2][32]; // 2 ue(v)
	uint32_t     abs_diff_pic_num_minus1[2][32]; // 2 ue(v)
	uint32_t     long_term_pic_num[2][32]; // 2 ue(v)
	bool     ref_pic_list_modification_flag_l1; // 2 u(1)
	uint32_t     ref_pic_list_modification_count_l0; //modification_of_pic_nums_idc[0]数组大小
	uint32_t     ref_pic_list_modification_count_l1; //modification_of_pic_nums_idc[1]数组大小



	// pred_weight_table
	uint32_t     luma_log2_weight_denom; // 2 ue(v)
	uint32_t     chroma_log2_weight_denom; // 2 ue(v)
	bool     luma_weight_l0_flag; // 2 u(1)
	int32_t     luma_weight_l0[32]; // 2 se(v)
	int32_t     luma_offset_l0[32]; // 2 se(v)
	uint32_t     chroma_weight_l0_flag; // 2 ue(v)
	int32_t     chroma_weight_l0[32][2]; // 2 se(v)
	int32_t     chroma_offset_l0[32][2]; // 2 se(v)
	bool     luma_weight_l1_flag; // 2 u(1)
	int32_t     luma_weight_l1[32]; // 2 se(v)
	int32_t     luma_offset_l1[32]; // 2 se(v)
	bool     chroma_weight_l1_flag; // 2 u(1)
	int32_t     chroma_weight_l1[32][2]; // 2 se(v)
	int32_t     chroma_offset_l1[32][2]; // 2 se(v)

	//表示帧场自适应
	bool			MbaffFrameFlag;   //MbaffFrameFlag = ( mb_adaptive_frame_field_flag && !field_pic_flag );
	int32_t			SliceGroupChangeRate;
	int32_t         SliceQPY; //SliceQPY = 26 + pic_init_qp_minus26 + slice_qp_delta;
	int32_t         QPY_prev; //QPY,PREV 是当前 slice 中前一宏块的量化参数 QPY 的值，在每个 slice 的开始处，对于 slice 中的第一个宏块，QPY,PREV 应该被初始化成等式 7-16 中的 SliceQPY 值。


	uint32_t  PicHeightInMbs;
	uint32_t  PicSizeInMbs;
	uint32_t* mapUnitToSliceGroupMap; //MbToSliceGroupMap[PicSizeInMapUnits];
	uint32_t* MbToSliceGroupMap; //MbToSliceGroupMap[PicSizeInMbs];
public:


	SliceHeader(ParseNalu& nalu);
	~SliceHeader();


	bool slice_header(BitStream& bs, const ParsePPS ppsCache[256], const ParseSPS spsCache[32]);


	bool dec_ref_pic_marking(BitStream& bs);
	bool pred_weight_table(BitStream& bs);
	bool ref_pic_list_modification(BitStream& bs);
private:
	bool setMapUnitToSliceGroupMap();
	bool setMbToSliceGroupMap();

};

