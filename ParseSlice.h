#pragma once
#include "Common.h"
#include "ParsePPS.h"
#include "ParseSPS.h"

class ParseSlice
{
public:
    uint32_t naluType;
    ParsePPS* pps;
    ParseSPS* sps;

    uint32_t     first_mb_in_slice; // 2 ue(v)
    uint32_t     slice_type; // 2 ue(v)
    uint32_t     pic_parameter_set_id; // 2 ue(v)
    uint32_t     colour_plane_id; // 2 u(2)
    uint32_t     frame_num; // 2 u(v)
    uint32_t     field_pic_flag; // 2 u(1)
    uint32_t     bottom_field_flag; // 2 u(1)
    uint32_t     idr_pic_id; // 2 ue(v)
    uint32_t     pic_order_cnt_lsb; // 2 u(v)
    int32_t     delta_pic_order_cnt_bottom; // 2 se(v)
    int32_t     delta_pic_order_cnt[2]; // 2 se(v)
    uint32_t     redundant_pic_cnt; // 2 ue(v)
    uint32_t     direct_spatial_mv_pred_flag; // 2 u(1)
    uint32_t     num_ref_idx_active_override_flag; // 2 u(1)
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
public:
    //ParseSlice(ParsePPS* pps, ParseSPS* sps, uint32_t naluType);
    ParseSlice(uint32_t naluType);

    ParsePPS* getpps(vector<ParsePPS*> pps);
    ParseSPS* getsps(vector<ParseSPS*> sps);

	bool slice_header(BitStream& bs, vector<ParsePPS*> pps, vector<ParseSPS*> sps);
	~ParseSlice();
};

