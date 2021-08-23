#pragma once
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




enum class NaluType :int
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
//slice type
enum class SLIECETYPE :int
{
	H264_SLIECE_TYPE_P = 0,
	H264_SLIECE_TYPE_B = 1,
	H264_SLIECE_TYPE_I = 2,
	H264_SLIECE_TYPE_SP = 3,
	H264_SLIECE_TYPE_SI = 4,
	H264_SLIECE_TYPE_P2 = 5,
	H264_SLIECE_TYPE_B2 = 6,
	H264_SLIECE_TYPE_I2 = 7,
	H264_SLIECE_TYPE_SP2 = 8,
	H264_SLIECE_TYPE_SI2 = 9,
};


enum class H264_MB_TYPE :int
{
	NA = -1,
	//表7 - 11－I 条带的宏块类型
	I_NxN,         //    0
	I_16x16_0_0_0, //    1
	I_16x16_1_0_0, //    2
	I_16x16_2_0_0, //    3
	I_16x16_3_0_0, //    4
	I_16x16_0_1_0, //    5
	I_16x16_1_1_0, //    6
	I_16x16_2_1_0, //    7
	I_16x16_3_1_0, //    8
	I_16x16_0_2_0, //    9
	I_16x16_1_2_0, //    10
	I_16x16_2_2_0, //    11
	I_16x16_3_2_0, //    12
	I_16x16_0_0_1, //    13
	I_16x16_1_0_1, //    14
	I_16x16_2_0_1, //    15
	I_16x16_3_0_1, //    16
	I_16x16_0_1_1, //    17
	I_16x16_1_1_1, //    18
	I_16x16_2_1_1, //    19
	I_16x16_3_1_1, //    20
	I_16x16_0_2_1, //    21
	I_16x16_1_2_1, //    22
	I_16x16_2_2_1, //    23
	I_16x16_3_2_1, //    24
	I_PCM,		   //    25

	//表7-13－P和SP条带的值为0到4的宏块类型
	P_L0_16x16,
	P_L0_L0_16x8,
	P_L0_L0_8x16,
	P_8x8,
	P_8x8ref0,
	P_Skip,

	//表7 - 12－SI条带的值为0的宏块类型
	SI,

	//表7-14－B条带中值为0到22的宏块类型
	B_Direct_16x16, //  0
	B_L0_16x16, //      1
	B_L1_16x16, //      2
	B_Bi_16x16, //      3
	B_L0_L0_16x8, //    4
	B_L0_L0_8x16, //    5
	B_L1_L1_16x8, //    6
	B_L1_L1_8x16, //    7
	B_L0_L1_16x8, //    8
	B_L0_L1_8x16, //    9
	B_L1_L0_16x8, //    10
	B_L1_L0_8x16, //    11
	B_L0_Bi_16x8, //    12
	B_L0_Bi_8x16, //    13
	B_L1_Bi_16x8, //    14
	B_L1_Bi_8x16, //    15
	B_Bi_L0_16x8, //    16
	B_Bi_L0_8x16, //    17
	B_Bi_L1_16x8, //    18
	B_Bi_L1_8x16, //    19
	B_Bi_Bi_16x8, //    20
	B_Bi_Bi_8x16, //    21
	B_8x8, //           22
	B_Skip, //          -1  P_Skip表示该宏块在码流中没有更多的数据了

	//Sub-macroblock types in P macroblocks
	P_L0_8x8, //    0
	P_L0_8x4, //    1
	P_L0_4x8, //    2
	P_L0_4x4, //    3

	//Sub-macroblock types in B macroblocks
	B_Direct_8x8, //    0
	B_L0_8x8, //    1
	B_L1_8x8, //    2
	B_Bi_8x8, //    3
	B_L0_8x4, //    4
	B_L0_4x8, //    5
	B_L1_8x4, //    6
	B_L1_4x8, //    7
	B_Bi_8x4, //    8
	B_Bi_4x8, //    9
	B_L0_4x4, //    10
	B_L1_4x4, //    11
	B_Bi_4x4, //    12
};


//表明当前宏块类型所采用的Intra预测方式
enum class H264_MB_PART_PRED_MODE :int
{
	Intra_4x4,
	Intra_8x8,
	Intra_16x16,
	Pred_L0,
	Pred_L1,
	Direct,
	BiPred,
	NA
};


enum class RESIDUAL_LEVEL
{
	LumaLevel4x4,
	LumaLevel8x8,
	ChromaDCLevel,
	ChromaACLevel,
	ChromaACLevelCb,
	ChromaACLevelCr,
	Intra16x16DCLevel,
	Intra16x16ACLevel,
	CbIntra16x16DCLevel,
	CbIntra16x16ACLevel,
	CbLevel4x4,
	CrLevel4x4,
	CbLevel8x8,
	CrLevel8x8,
	CrIntra16x16DCLevel,
	CrIntra16x16ACLevel
};


//enum class PICTURE_MARKING :int
//{
//	ENDWHILE, //结束循环，退出标记（marding）操作。
//	SHOR_TERM_UNUSED_FOR_REFERENCE,//将一个短期参考图像标记为非参考图像，也 即将一个短期参考图像移出参考帧队列
//	LONG_TERM_UNUSED_FOR_REFERENCE,//将一个长期参考图像标记为非参考图像，也 即将一个长期参考图像移出参考帧队列
//	SHOR_TERM_TRANS_LONG_TERM,  //将一个短期参考图像转为长期参考图像。
//	MAX_LONG_TERM_SIZE, //指明长期参考帧的最大数目。
//	CLEAR_TERM,  //清空参考帧队列，将所有参考图像移出参考 帧队列，并禁用长期参考机制
//	CURRENT_TRANS_LONG_TERM //将当前图像存为一个长期参考帧。
//};
enum class PICTURE_MARKING
{
	UNKOWN,
	USED_FOR_REFERENCE,   //用过的
	SHORT_TERM_REFERENCE, //短期参考帧
	LONG_TERM_REFERENCE,  //长期参考帧
	UNUSED_FOR_REFERENCE, //没有使用过的参考帧
};

//Intra4x4PredMode 9种4*4帧内预测模式
constexpr int Intra_4x4_Vertical = 0;
constexpr int Intra_4x4_Horizontal = 1;
constexpr int Intra_4x4_DC = 2;
constexpr int Intra_4x4_Diagonal_Down_Left = 3;
constexpr int Intra_4x4_Diagonal_Down_Right = 4;
constexpr int Intra_4x4_Vertical_Right = 5;
constexpr int Intra_4x4_Horizontal_Down = 6;
constexpr int Intra_4x4_Vertical_Left = 7;
constexpr int Intra_4x4_Horizontal_Up = 8;

//enum class Intra4x4PredMode :int
//{
//	Intra_4x4_Vertical,
//	Intra_4x4_Horizontal,
//	Intra_4x4_DC,
//	Intra_4x4_Diagonal_Down_Left,
//	Intra_4x4_Diagonal_Down_Right,
//	Intra_4x4_Vertical_Right,
//	Intra_4x4_Horizontal_Down,
//	Intra_4x4_Vertical_Left,
//	Intra_4x4_Horizontal_Up,
//	NA = NA
//};