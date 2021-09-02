#pragma once
#include "Common.h"
#include "ParsePPS.h"
#include "ParseSPS.h"
#include "ParseNalu.h"


struct DEC_REF_PIC_MARKING
{
	uint8_t     memory_management_control_operation; //在自适应标记（marking）模式中指明本次操作的具体内容 表7-24
	uint8_t     difference_of_pic_nums_minus1;
	uint8_t     long_term_pic_num;
	uint8_t     long_term_frame_idx;
	uint8_t     max_long_term_frame_idx_plus1;
};

class SliceHeader
{
public:

	ParseNalu nalu;
	ParsePPS pps;
	ParseSPS sps;

	uint32_t    slice_type; // 2 ue(v)


	uint32_t     first_mb_in_slice; // 2 ue(v)

	uint32_t     pic_parameter_set_id; // 2 ue(v)
	uint32_t     colour_plane_id; // 2 u(2)
	uint16_t     frame_num; // 2 u(v)
	bool		field_pic_flag; // 2 u(1)
	bool		bottom_field_flag; // 2 u(1)
	uint16_t     idr_pic_id; // 0 - 65535
	uint16_t     pic_order_cnt_lsb; // 2 - 2^12-1 
	int32_t     delta_pic_order_cnt_bottom; // 2 se(v)
	int32_t     delta_pic_order_cnt[2]; // 2 se(v)
	uint32_t     redundant_pic_cnt; // 2 ue(v)
	bool		direct_spatial_mv_pred_flag; // 2 u(1)
	bool		num_ref_idx_active_override_flag; // 2 u(1)
	uint8_t     num_ref_idx_l0_active_minus1; // 2 ue(v) [0,31]
	uint8_t     num_ref_idx_l1_active_minus1; // 2 ue(v) [0,31]
	uint32_t     cabac_init_idc; // 2 ue(v)
	int32_t     slice_qp_delta; // 2 se(v)
	uint32_t     sp_for_switch_flag; // 2 u(1)
	int32_t     slice_qs_delta; // 2 se(v)
	bool     disable_deblocking_filter_idc; // 2 ue(v) 0：开启环路滤波，滤波可跨越slice边界。1：关闭环路滤波。2：开启环路滤波，只针对同一个slice滤波。
	int8_t     slice_alpha_c0_offset_div2; // 2 se(v)
	int8_t     slice_beta_offset_div2; // 2 se(v)
	uint32_t     slice_group_change_cycle; // 2 u(v)



	int		QSY;


	bool     no_output_of_prior_pics_flag; // 2 | 5 u(1)
	bool     long_term_reference_flag; // 2 | 5 u(1)
	bool     adaptive_ref_pic_marking_mode_flag; // 2 | 5 u(1)
	DEC_REF_PIC_MARKING    dec_ref_pic_markings[32];
	size_t				   dec_ref_pic_markings_size;




	// ref_pic_list_modification//帧间参考作用的 参考帧列表
	bool     ref_pic_list_modification_flag_l0; // 2 u(1)
	uint8_t     modification_of_pic_nums_idc[2][32]; // 2 ue(v)
	uint16_t     abs_diff_pic_num_minus1[2][32]; // 0 到 MaxPicNum – 1
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
	bool		MbaffFrameFlag;   //MbaffFrameFlag = ( mb_adaptive_frame_field_flag && !field_pic_flag );
	int			SliceGroupChangeRate;
	int			SliceQPY; //SliceQPY = 26 + pic_init_qp_minus26 + slice_qp_delta;
	int         QPY_prev; //QPY,PREV 是当前 slice 中前一宏块的量化参数 QPY 的值，在每个 slice 的开始处，对于 slice 中的第一个宏块，QPY,PREV 应该被初始化成等式 7-16 中的 SliceQPY 值。
	int8_t      FilterOffsetA;
	int8_t      FilterOffsetB;

	uint32_t  PicHeightInMbs;
	uint32_t  PicSizeInMbs;
	int16_t         MaxPicNum; //MaxPicNum = (field_pic_flag == 0) ? MaxFrameNum : (2 * MaxFrameNum);
	int16_t         CurrPicNum; //CurrPicNum = (field_pic_flag == 0) ? frame_num : (2 * frame_num + 1);

	/*图像中每个“mapUnit(映射单元)”属于哪个组号，需要一个数组来记录，将这个数据定义为mapUnitToSliceGroupMap[]，
	数组中的第i个元素表示以光栅顺序扫描时，第i个“mapUnit(映射单元)”对应哪个组（组号）。*/
	uint32_t* mapUnitToSliceGroupMap; //MbToSliceGroupMap[PicSizeInMapUnits];
	//当我们得到mapUnitToSliceGroupMap[]后，就可以根据宏块与“mapUnit(映射单元)”的对应关系
	uint32_t* MbToSliceGroupMap; //MbToSliceGroupMap[PicSizeInMbs];

	int ScalingList4x4[6][16];
	int ScalingList8x8[6][64];


public:


	SliceHeader(const ParseNalu& nalu);

	~SliceHeader();

	SliceHeader& operator= (const SliceHeader& src);


	bool isFinishPicture();

	bool slice_header(BitStream& bs, const ParsePPS ppsCache[256], const ParseSPS spsCache[32]);


	bool dec_ref_pic_marking(BitStream& bs);
	bool pred_weight_table(BitStream& bs);
	bool ref_pic_list_modification(BitStream& bs);
private:
	bool setMapUnitToSliceGroupMap();
	bool setMbToSliceGroupMap();

	void setScallingList();

};

