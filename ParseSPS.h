#pragma once
#include "Common.h"


#define    H264_MAX_SPS_COUNT    32    // 7.4.2.1.1: seq_parameter_set_id shall be in the range of 0 to 31, inclusive.
#define    H264_MAX_OFFSET_REF_FRAME_COUNT    256    // 7.4.2.1.1: num_ref_frames_in_pic_order_cnt_cycle的范围是0到255(包括0和255)。

#define    MONOCHROME               0    //黑白图像
#define    CHROMA_FORMAT_IDC_420    1
#define    CHROMA_FORMAT_IDC_422    2
#define    CHROMA_FORMAT_IDC_444    3




struct CHROMA_FORMAT_IDC_T
{
	uint32_t    chroma_format_idc;
	bool		separate_colour_plane_flag;//是分开编码还是一起编码（yuv）
	int32_t     SubWidthC;
	int32_t     SubHeightC;
};



class ParseSPS
{
public:
	uint32_t profile_idc; // 0 u(8)
	uint32_t constraint_set0_flag; // 0 u(1)
	uint32_t constraint_set1_flag; // 0 u(1)
	uint32_t constraint_set2_flag; // 0 u(1)
	uint32_t constraint_set3_flag; // 0 u(1)
	uint32_t constraint_set4_flag; // 0 u(1)
	uint32_t constraint_set5_flag; // 0 u(1)
	uint32_t reserved_zero_2bits; // /* equal to 0 */ 0 u(2)
	uint32_t level_idc; // 0 u(8)
	uint32_t seq_parameter_set_id; // 0 ue(v)
	uint8_t chroma_format_idc;   // 0 ue(v)
	bool separate_colour_plane_flag;  // 0 u(1)
	uint8_t bit_depth_luma_minus8;   // 0 ue(v)当bit_depth_luma_minus8不存在时，它将被推断为等于0。bit_depth_luma_minus8的取值范围为0到6(包括6)。
	uint8_t bit_depth_chroma_minus8;   // 0 ue(v)
	bool qpprime_y_zero_transform_bypass_flag;  // 0 u(1)
	bool seq_scaling_matrix_present_flag;  // 0 u(1)
	bool  seq_scaling_list_present_flag[12]; //seq_scaling_list_present_flag[ i ] 0 u(1)
	uint8_t log2_max_frame_num_minus4;  // 0-12
	uint16_t MaxFrameNum;
	uint32_t pic_order_cnt_type;  // 0 ue(v)
	uint32_t log2_max_pic_order_cnt_lsb_minus4;  // 0 ue(v)
	uint32_t  MaxPicOrderCntLsb;
	bool delta_pic_order_always_zero_flag;  // 0 ue(v)
	int  offset_for_non_ref_pic;  //-2^31-1 - 2^31-1，有符号4字节整数
	int32_t  offset_for_top_to_bottom_field;   // 0 se(v)
	uint8_t num_ref_frames_in_pic_order_cnt_cycle; // 0-255
	int offset_for_ref_frame[H264_MAX_OFFSET_REF_FRAME_COUNT]; //-2^31-1 - 2^31-1，有符号4字节整数
	int ExpectedDeltaPerPicOrderCntCycle; //offset_for_ref_frame[ num_ref_frames_in_pic_order_cnt_cycle ] 0 se(v)

	uint8_t     max_num_ref_frames; // 0 ue(v)
	bool     gaps_in_frame_num_value_allowed_flag; // 0 u(1)
	uint32_t     pic_width_in_mbs_minus1; // 0 ue(v)
	uint32_t     pic_height_in_map_units_minus1; // 0 ue(v)
	bool     frame_mbs_only_flag; // 0 u(1)
	bool     mb_adaptive_frame_field_flag; // 0 u(1)
	bool     direct_8x8_inference_flag; // 0 u(1)
	bool     frame_cropping_flag; // 0 u(1)
	uint32_t     frame_crop_left_offset; // 0 ue(v)
	uint32_t     frame_crop_right_offset; // 0 ue(v)
	uint32_t     frame_crop_top_offset; // 0 ue(v)
	uint32_t     frame_crop_bottom_offset; // 0 ue(v)


	bool     vui_parameters_present_flag; // 0 u(1)







	//暂时不处理
	int32_t     ScalingList4x4[6][16];
	int32_t     ScalingList8x8[6][64];


	bool     UseDefaultScalingMatrix4x4Flag[6];
	bool     UseDefaultScalingMatrix8x8Flag[6];



	int32_t     PicWidthInMbs; // PicWidthInMbs = pic_width_in_mbs_minus1 + 1;


	uint32_t  PicWidthInSamplesL;
	uint32_t  PicWidthInSamplesC;
	uint32_t  PicHeightInSamplesC;
	uint32_t  PicHeightInSamplesL;


	int32_t     FrameHeightInMbs; //FrameHeightInMbs = ( 2 - frame_mbs_only_flag ) * PicHeightInMapUnits;
	int32_t     PicHeightInMapUnits; //PicHeightInMapUnits = pic_height_in_map_units_minus1 + 1;
	int32_t     PicSizeInMapUnits; //PicSizeInMapUnits = PicWidthInMbs * PicHeightInMapUnits;
	uint8_t     ChromaArrayType;


	uint32_t   SubWidthC;
	uint32_t   SubHeightC;
	uint8_t   BitDepthY;
	uint8_t   QpBdOffsetY;
	uint8_t   BitDepthC;
	uint8_t   QpBdOffsetC;
	uint32_t   MbWidthC;
	uint32_t   MbHeightC;

public:
	ParseSPS();

	bool seq_parameter_set_data(BitStream& bs);

	~ParseSPS();

private:

};
