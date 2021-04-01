#pragma once
#include "Common.h"




//Í¼Ïñ²ÎÊý¼¯
class ParsePPS
{
public:
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
    uint32_t     slice_group_change_rate_minus1; // 1 ue(v)
    uint32_t     pic_size_in_map_units_minus1; // 1 ue(v)
    uint32_t*    slice_group_id; //[ i ] 1 u(v)
    uint32_t     num_ref_idx_l0_default_active_minus1; // 1 ue(v)
    uint32_t     num_ref_idx_l1_default_active_minus1; // 1 ue(v)
    bool     weighted_pred_flag; // 1 u(1)
    uint32_t     weighted_bipred_idc; // 1 u(2)
    int32_t     pic_init_qp_minus26; // /* relative to 26 */ 1 se(v)
    int32_t     pic_init_qs_minus26; // /* relative to 26 */ 1 se(v)
    int32_t     chroma_qp_index_offset; // 1 se(v)
    bool     deblocking_filter_control_present_flag; // 1 u(1)
    bool     constrained_intra_pred_flag; // 1 u(1)
    bool     redundant_pic_cnt_present_flag; // 1 u(1)
    bool     transform_8x8_mode_flag; // 1 u(1)
    bool     pic_scaling_matrix_present_flag; // 1 u(1)
    int32_t    pic_scaling_list_present_flag[12]; //[ i ] 1 u(1)
    int32_t    second_chroma_qp_index_offset; // 1 se(v)

    int32_t     ScalingList4x4[6][16];
    int32_t     ScalingList8x8[6][64];
    int32_t     UseDefaultScalingMatrix4x4Flag[6];
    int32_t     UseDefaultScalingMatrix8x8Flag[6];
public:
    ParsePPS();

    bool pic_parameter_set_rbsp(BitStream& bs);
    ~ParsePPS();
};

