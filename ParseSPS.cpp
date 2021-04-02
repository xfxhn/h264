#include "ParseSPS.h"
//在 SPS 和 PPS 的语法表格中，会出现以下几种描述子：
//
//u(n) : 无符号整数，n bit 长度
//ue(v) : 无符号指数哥伦布熵编码
//se(v) : 有符号指数哥伦布熵编码
ParseSPS::ParseSPS()
{
    profile_idc = 0;
    constraint_set0_flag = 0;
    constraint_set1_flag = 0;
    constraint_set2_flag = 0;
    constraint_set3_flag = 0;
    constraint_set4_flag = 0;
    constraint_set5_flag = 0;
    reserved_zero_2bits = 0;
    level_idc = 0;
    seq_parameter_set_id = 0;
    chroma_format_idc = 0;
    separate_colour_plane_flag = 0;
    bit_depth_luma_minus8 = 0;
    bit_depth_chroma_minus8 = 0;
    qpprime_y_zero_transform_bypass_flag = 0;
    seq_scaling_matrix_present_flag = 0;
    memset(seq_scaling_list_present_flag, 0, sizeof(int32_t) * 12);
    log2_max_frame_num_minus4 = 0;
    pic_order_cnt_type = 0;
    log2_max_pic_order_cnt_lsb_minus4 = 0;
    delta_pic_order_always_zero_flag = 0;
    offset_for_non_ref_pic = 0;
    offset_for_top_to_bottom_field = 0;
    num_ref_frames_in_pic_order_cnt_cycle = 0;

    memset(offset_for_ref_frame, 0, sizeof(int32_t) * H264_MAX_OFFSET_REF_FRAME_COUNT);
    max_num_ref_frames = 0;
    gaps_in_frame_num_value_allowed_flag = 0;
    pic_width_in_mbs_minus1 = 0;
    pic_height_in_map_units_minus1 = 0;
    frame_mbs_only_flag = 0;
    mb_adaptive_frame_field_flag = 0;
    direct_8x8_inference_flag = 0;
    frame_cropping_flag = 0;
    frame_crop_left_offset = 0;
    frame_crop_right_offset = 0;
    frame_crop_top_offset = 0;
    frame_crop_bottom_offset = 0;
    vui_parameters_present_flag = 0;


    memset(ScalingList4x4, 0, sizeof(int32_t) * 6 * 16);
    memset(ScalingList8x8, 0, sizeof(int32_t) * 6 * 64);
    PicWidthInMbs = 0;
    PicHeightInMapUnits = 0;
    PicSizeInMapUnits = 0;
    ChromaArrayType = 0;
}



bool ParseSPS::seq_parameter_set_data(BitStream& bs)
{

    int ret = 0;
    //编码等级
    /*
    66	Baseline
    77	Main
    88	Extended
    100	High(FRExt)
    110	High 10 (FRExt)
    122	High 4:2 : 2 (FRExt)
    144	High 4 : 4 : 4 (FRExt)*/
    profile_idc = bs.readMultiBit(8);
    constraint_set0_flag = bs.readMultiBit(1); //constraint_set0_flag 0 u(1)
    constraint_set1_flag = bs.readMultiBit(1); //constraint_set1_flag 0 u(1)
    constraint_set2_flag = bs.readMultiBit(1); //constraint_set2_flag 0 u(1)
    constraint_set3_flag = bs.readMultiBit(1); //constraint_set3_flag 0 u(1)
    constraint_set4_flag = bs.readMultiBit(1); //constraint_set4_flag 0 u(1)
    constraint_set5_flag = bs.readMultiBit(1); //constraint_set5_flag 0 u(1)
    reserved_zero_2bits = bs.readMultiBit(2); //reserved_zero_2bits /* equal to 0 */ 0 u(2)
    level_idc = bs.readMultiBit(8); //level_idc 0 u(8)
    //要解码一个 Slice，需要有 SPS 和 PPS
    //但是码流中会有很多个SPS和PPS
    //如何确定依赖的是哪个SPS和PPS呢？
    //那就给 SPS 和 PPS 设置一个 ID 就行了
    seq_parameter_set_id = bs.readUE();

    //chroma_format_idc = 0	单色
    //chroma_format_idc = 1	YUV 4 : 2 : 0
    //chroma_format_idc = 2	YUV 4 : 2 : 2
    //chroma_format_idc = 3	YUV 4 : 4 : 4
    //当chroma_format_idc不存在时，它将被推断为等于1(4:2:0色度格式)。
    //码流是什么格式
    chroma_format_idc = 1;
    //只有当 profile_idc 等于这些值的时候，chroma_format_idc 才会被显式记录，
    //那么如果 profile_idc 不是这些值，chroma_format_idc 就会取默认值。
    if (profile_idc == 100 //A.2.4 High profile //Only I, P, and B slice types may be present.
        || profile_idc == 110 //A.2.5(A.2.8) High 10 (Intra) profile //Only I, P, and B slice types may be present.
        || profile_idc == 122 //A.2.6(A.2.9) High 4:2:2 (Intra) profile //Only I, P, and B slice types may be present.
        || profile_idc == 244 //A.2.7(A.2.10) High 4:4:4 Predictive/Intra profile //Only I, P, B slice types may be present.
        || profile_idc == 44 //A.2.11 CAVLC 4:4:4 Intra profile
        || profile_idc == 83 //G.10.1.2.1 Scalable Constrained High profile (SVC) //Only I, P, EI, and EP slices shall be present.
        || profile_idc == 86 //Scalable High Intra profile (SVC)
        || profile_idc == 118 //Stereo High profile (MVC)
        || profile_idc == 128 //Multiview High profile (MVC)
        || profile_idc == 138 //Multiview Depth High profile (MVCD)
        || profile_idc == 139 //
        || profile_idc == 134 //
        || profile_idc == 135 //
        )
    {
        chroma_format_idc = bs.readUE();
        //当在码流中读取到 chroma_format_idc 之后separate_colour_plane_flag
        //当separate_colour_plane_flag不存在时，它将被推断为等于0。
        separate_colour_plane_flag = 0;
        //这个语法元素在 chroma_format_idc 等于 3，也就是 YUV 444 模式的时候才有
        if (chroma_format_idc==3)
        {
            /*当图像是 YUV 444 的时候，YUV 三个分量的比重是相同的，那么就有两种编码方式了。
            第一种，就是和其他格式一样，让 UV 分量依附在 Y 分量上；第二种，就是把 UV 和 Y 分开，独立出来。
            separate_colour_plane_flag 这个值默认是 0，表示 UV 依附于 Y，和 Y 一起编码，
            如果 separate_colour_plane_flag 变成 1，则表示 UV 与 Y 分开编码。
            而对于分开编码的模式，我们采用和单色模式一样的规则。*/
            separate_colour_plane_flag = bs.readBit();
        }
        //指定了亮度矩阵的比特深度以及亮度量化参数范围偏移量；此处取0，表示每个亮度像素用8为表示，QpBdOffset参数为0；
        bit_depth_luma_minus8 = bs.readUE();
        //与bit_depth_luma_minus8类似，只不过是针对色度的
        bit_depth_chroma_minus8 = bs.readUE();
        qpprime_y_zero_transform_bypass_flag = bs.readBit();
        //缩放标志位
        seq_scaling_matrix_present_flag = bs.readBit();

        if (this->seq_scaling_matrix_present_flag)
        {
            for (size_t i = 0; i < ((this->chroma_format_idc != 3) ? 8 : 12); i++)
            {
                this->seq_scaling_list_present_flag[i] = bs.readBit(); //seq_scaling_list_present_flag[ i ] 0 u(1)
                if (this->seq_scaling_list_present_flag[i] == 1)
                {
                    if (i < 6)
                    {
                        ret = scaling_list(bs, ScalingList4x4[i], 16, UseDefaultScalingMatrix4x4Flag[i]);
                        //RETURN_IF_FAILED(ret != 0, ret);
                    }
                    else
                    {
                        ret = scaling_list(bs, ScalingList8x8[i - 6], 64, UseDefaultScalingMatrix8x8Flag[i - 6]);
                       // RETURN_IF_FAILED(ret != 0, ret);
                    }
                }
            }
        }
        
    }

    //这个句法元素主要是为读取另一个句法元素 frame_num  服务的，frame_num  是最重要的句法元素之一，它标识所属图像的解码顺序 。
    //这个句法元素同时也指明了frame_num 的所能达到的最大值:MaxFrameNum = 2*exp( log2_max_frame_num_minus4 + 4 )
    //最大帧率
    log2_max_frame_num_minus4 = bs.readUE();
    //指明了 poc  (picture  order  count)  的编码方法，poc 标识图像的播放顺序。
    //由poc 可以由 frame-num 通过映射关系计算得来，也可以索性由编码器显式地传送。
    //是指解码图像顺序的计数方法（如  8.2.1 节所述）。pic_order_cnt_type 的取值范围是0 到 2（包括0 和2）。
    pic_order_cnt_type = bs.readUE();

    if (this->pic_order_cnt_type == 0)
    {
        this->log2_max_pic_order_cnt_lsb_minus4 = bs.readUE(); //log2_max_pic_order_cnt_lsb_minus4 0 ue(v)
    }
    else if (this->pic_order_cnt_type == 1)
    {
        this->delta_pic_order_always_zero_flag = bs.readBit(); //delta_pic_order_always_zero_flag 0 u(1)
        this->offset_for_non_ref_pic = bs.readSE(); //offset_for_non_ref_pic 0 se(v)
        this->offset_for_top_to_bottom_field = bs.readSE(); //offset_for_top_to_bottom_field 0 se(v)
        this->num_ref_frames_in_pic_order_cnt_cycle = bs.readUE(); //num_ref_frames_in_pic_order_cnt_cycle 0 ue(v)

        for (size_t i = 0; i < (int32_t)this->num_ref_frames_in_pic_order_cnt_cycle; i++)
        {
            this->offset_for_ref_frame[i] = bs.readSE(); //offset_for_ref_frame[ i ] 0 se(v)
        }
    }
    //最大允许多少个参考帧
    max_num_ref_frames = bs.readUE();
    //是否允许出现不连续的情况,跳帧
    gaps_in_frame_num_value_allowed_flag = bs.readBit();
    //以红快为单位的宽度
    pic_width_in_mbs_minus1 = bs.readUE();
    //这里要考虑帧编码和场编码的问题，帧编码是红快高度
    pic_height_in_map_units_minus1 = bs.readUE();
    //是否为帧编码
    frame_mbs_only_flag = bs.readBit();


    if (!frame_mbs_only_flag)
    {
        //采用了场编码的情况下
        //是否采用红快级别帧场自适应的问题 ，是否可以在帧编码和场编码相互切换
        mb_adaptive_frame_field_flag = bs.readBit();
    }
    
    //推导亮度矢量的方法

    direct_8x8_inference_flag = bs.readBit();
    //图片进行裁剪的偏移量 如果为0那么就不需要裁剪
    frame_cropping_flag = bs.readBit();

    if (frame_cropping_flag)
    {
        frame_crop_left_offset = bs.readUE(); //frame_crop_left_offset 0 ue(v)
        frame_crop_right_offset = bs.readUE(); //frame_crop_right_offset 0 ue(v)
        frame_crop_top_offset = bs.readUE(); //frame_crop_top_offset 0 ue(v)
        frame_crop_bottom_offset = bs.readUE(); //frame_crop_bottom_offset 0 ue(v)
    }
    vui_parameters_present_flag = bs.readBit();

    if (vui_parameters_present_flag)
    {
        //视频渲染的一些辅助信息
        /*ret = m_vui.vui_parameters(bs);
        RETURN_IF_FAILED(ret != 0, ret);*/
    }



    
    if (separate_colour_plane_flag == 0) {
        ChromaArrayType = chroma_format_idc;
    }
    else {
        ChromaArrayType = 0;
    }
    getWidthAndHeight();



    return true;


}

pair<uint32_t, uint32_t> ParseSPS::getWidthAndHeight()
{
    //frame_mbs_only_flag 是否为帧编码
    //ChromaArrayType=0,表示只有 Y 分量或者表示 YUV 444 的独立模式
    //frame_cropping_flag图片进行裁剪的偏移量 如果为0那么就不需要裁剪
    uint32_t width = (pic_width_in_mbs_minus1 + 1) * 16;

    uint32_t height = (2 - frame_mbs_only_flag) * (pic_height_in_map_units_minus1 + 1) * 16;

   /* if (frame_cropping_flag)
    {
        int crop_unit_x = 0;
        int crop_unit_y = 0;

        if (ChromaArrayType == 0) {
            crop_unit_x = 1;
            crop_unit_y = 2 - frame_mbs_only_flag;
        }
    }
    cout << width << endl;
    cout << height << endl;
    cout << "----" << endl;*/
    return pair<uint32_t, uint32_t>(width, height);
}

ParseSPS::~ParseSPS()
{
}
