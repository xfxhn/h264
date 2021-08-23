#pragma once
//����IDR֡��˵����IDR֮֡�������֡�����������κ�IDR֮֡ǰ��֡�����ݣ�
//����෴��������ͨ��I - ֡��˵��λ����֮���B - ��P - ֡��������λ����ͨI - ֮֡ǰ��I - ֡��
//�������ȡ����Ƶ���У���������Զ���Դ�һ��IDR֡���ţ���Ϊ����֮��û���κ�֡����֮ǰ��֡��
//���ǣ�������һ��û��IDR֡����Ƶ�д�����㿪ʼ���ţ���Ϊ�����֡���ǻ�����ǰ���֡��

//nal_unit_type	NALU ����
//0	δ����
//1	��� nalu ����֮Ϊ Slice��������ǳ��� IDR Slice ����     
//2	�� IDR SLICE������ A �����ݻ���Ƭ��	slice_data_partition_a_layer_rbsp()
//3	�� IDR SLICE������ B �����ݻ���Ƭ��	slice_data_partition_b_layer_rbsp()
//4	�� IDR SLICE������ C �����ݻ���Ƭ��	slice_data_partition_c_layer_rbsp()

//5	IDR SLICE	nalu ����֮Ϊ IDR ֡��Ҳ�����ؼ�֡   ��ʱ����ˢ�� I��IDR֡����ʹ��֡��Ԥ��ġ�
//5 ���Ƕ���ͬһ����������,�ڱ���ͽ�����Ϊ�˷��㣬Ҫ�׸�I֡������I֡���𿪣����ԲŰѵ�һ���׸�I֡��IDR,IDR֡������������ˢ��,ʹ�����´���

//6	������ǿ��Ϣ SEI	sei_rbsp()
//7	���в����� SPS	seq_parameter_set_rbsp()
//8	ͼ������� PPS	pic_parameter_set_rbsp()
//9	�ָ���	access_unit_delimiter_rbsp()
//10	���н�����	end_of_seq_rbsp()
//11	����������	end_of_stream_rbsp()
//12	�������	filler_data_rbsp()
//13	���в�����չ��	seq_parameter_set_extension_rbsp()
//14~18	����
//19	δ�ָ�ĸ�������ͼ��ı�������	slice_layer_without_partitioning_rbsp()
//20~23	����
//24~31	δָ��




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
	//��7 - 11��I �����ĺ������
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

	//��7-13��P��SP������ֵΪ0��4�ĺ������
	P_L0_16x16,
	P_L0_L0_16x8,
	P_L0_L0_8x16,
	P_8x8,
	P_8x8ref0,
	P_Skip,

	//��7 - 12��SI������ֵΪ0�ĺ������
	SI,

	//��7-14��B������ֵΪ0��22�ĺ������
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
	B_Skip, //          -1  P_Skip��ʾ�ú����������û�и����������

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


//������ǰ������������õ�IntraԤ�ⷽʽ
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
//	ENDWHILE, //����ѭ�����˳���ǣ�marding��������
//	SHOR_TERM_UNUSED_FOR_REFERENCE,//��һ�����ڲο�ͼ����Ϊ�ǲο�ͼ��Ҳ ����һ�����ڲο�ͼ���Ƴ��ο�֡����
//	LONG_TERM_UNUSED_FOR_REFERENCE,//��һ�����ڲο�ͼ����Ϊ�ǲο�ͼ��Ҳ ����һ�����ڲο�ͼ���Ƴ��ο�֡����
//	SHOR_TERM_TRANS_LONG_TERM,  //��һ�����ڲο�ͼ��תΪ���ڲο�ͼ��
//	MAX_LONG_TERM_SIZE, //ָ�����ڲο�֡�������Ŀ��
//	CLEAR_TERM,  //��ղο�֡���У������вο�ͼ���Ƴ��ο� ֡���У������ó��ڲο�����
//	CURRENT_TRANS_LONG_TERM //����ǰͼ���Ϊһ�����ڲο�֡��
//};
enum class PICTURE_MARKING
{
	UNKOWN,
	USED_FOR_REFERENCE,   //�ù���
	SHORT_TERM_REFERENCE, //���ڲο�֡
	LONG_TERM_REFERENCE,  //���ڲο�֡
	UNUSED_FOR_REFERENCE, //û��ʹ�ù��Ĳο�֡
};

//Intra4x4PredMode 9��4*4֡��Ԥ��ģʽ
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