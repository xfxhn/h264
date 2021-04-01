#pragma once
#include <iostream>
//对于IDR帧来说，在IDR帧之后的所有帧都不能引用任何IDR帧之前的帧的内容，
//与此相反，对于普通的I - 帧来说，位于其之后的B - 和P - 帧可以引用位于普通I - 帧之前的I - 帧。
//从随机存取的视频流中，播放器永远可以从一个IDR帧播放，因为在它之后没有任何帧引用之前的帧。
//但是，不能在一个没有IDR帧的视频中从任意点开始播放，因为后面的帧总是会引用前面的帧。

//nal_unit_type	NALU 类型
//0	未定义
//1	这个 nalu 被称之为 Slice，这里就是除了 IDR Slice 以外     
//2	非 IDR SLICE，采用 A 类数据划分片段	slice_data_partition_a_layer_rbsp()
//3	非 IDR SLICE，采用 B 类数据划分片段	slice_data_partition_b_layer_rbsp()
//4	非 IDR SLICE，采用 C 类数据划分片段	slice_data_partition_c_layer_rbsp()

//5	IDR SLICE	nalu 被称之为 IDR 帧，也叫做关键帧   即时解码刷新 I和IDR帧都是使用帧内预测的。
//5 它们都是同一个东西而已,在编码和解码中为了方便，要首个I帧和其他I帧区别开，所以才把第一个首个I帧叫IDR,IDR帧的作用是立刻刷新,使错误不致传播

//6	补充增强信息 SEI	sei_rbsp()
//7	序列参数集 SPS	seq_parameter_set_rbsp()
//8	图像参数集 PPS	pic_parameter_set_rbsp()
//9	分隔符	access_unit_delimiter_rbsp()
//10	序列结束符	end_of_seq_rbsp()
//11	码流结束符	end_of_stream_rbsp()
//12	填充数据	filler_data_rbsp()
//13	序列参数扩展集	seq_parameter_set_extension_rbsp()
//14~18	保留
//19	未分割的辅助编码图像的编码条带	slice_layer_without_partitioning_rbsp()
//20~23	保留
//24~31	未指定
enum class NaluType:int
{
    H264_NAL_UNSPECIFIED = 0,
    H264_NAL_SLICE = 1,
    H264_NAL_DPA = 2,
    H264_NAL_DPB = 3,
    H264_NAL_DPC = 4,
    H264_NAL_IDR_SLICE = 5,
    H264_NAL_SEI = 6,
    H264_NAL_SPS = 7,
    H264_NAL_PPS = 8,
    H264_NAL_AUD = 9,
    H264_NAL_END_SEQUENCE = 10,
    H264_NAL_END_STREAM = 11,
    H264_NAL_FILLER_DATA = 12,
    H264_NAL_SPS_EXT = 13,
    H264_NAL_PREFIX = 14,
    H264_NAL_SUB_SPS = 15,
    H264_NAL_DPS = 16,
    H264_NAL_RESERVED17 = 17,
    H264_NAL_RESERVED18 = 18,
    H264_NAL_AUXILIARY_SLICE = 19,
    H264_NAL_EXTEN_SLICE = 20,
    H264_NAL_DEPTH_EXTEN_SLICE = 21,
    H264_NAL_RESERVED22 = 22,
    H264_NAL_RESERVED23 = 23,
    H264_NAL_UNSPECIFIED24 = 24,
    H264_NAL_UNSPECIFIED25 = 25,
    H264_NAL_UNSPECIFIED26 = 26,
    H264_NAL_UNSPECIFIED27 = 27,
    H264_NAL_UNSPECIFIED28 = 28,
    H264_NAL_UNSPECIFIED29 = 29,
    H264_NAL_UNSPECIFIED30 = 30,
    H264_NAL_UNSPECIFIED31 = 31,
};