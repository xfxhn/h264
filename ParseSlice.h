#pragma once
#include "Common.h"
#include "ParsePPS.h"
#include "ParseSPS.h"
#include "ParseNalu.h"

class ParseSlice
{
public:
    ParseNalu &nalu;
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
    uint32_t     disable_deblocking_filter_idc; // 2 ue(v) 0��������·�˲����˲��ɿ�Խslice�߽硣1���رջ�·�˲���2��������·�˲���ֻ���ͬһ��slice�˲���
    int32_t     slice_alpha_c0_offset_div2; // 2 se(v)
    int32_t     slice_beta_offset_div2; // 2 se(v)
    uint32_t     slice_group_change_cycle; // 2 u(v)
public:
    //ParseSlice(ParsePPS* pps, ParseSPS* sps, uint32_t naluType);
    ParseSlice(ParseNalu& nalu);

    //bool getppsAndSps();
    //ParseSPS* getsps(vector<ParseSPS*> sps);

	bool slice_header(BitStream& bs, const ParsePPS ppsCache[256], const ParseSPS spsCache[32]);
    bool dec_ref_pic_marking(BitStream& bs);
	~ParseSlice();
};

