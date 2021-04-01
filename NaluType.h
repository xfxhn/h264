#pragma once
#include <iostream>
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