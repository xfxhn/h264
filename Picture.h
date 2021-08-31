#pragma once
#include "NaluType.h"
class ParseSlice;
class SliceHeader;

class Picture
{

public:
	//����POC��Ҫ�õ���
	int			FrameNumOffset;
	int			pic_order_cnt_lsb;
	int			frame_num;

	int         PicOrderCntMsb;
	int         PicOrderCntLsb;
	int         TopFieldOrderCnt;
	int         BottomFieldOrderCnt;


	//PICTURE_MARKING  reference_marked_type; //I,P��Ϊ�ο�֡��mark״̬
public:
	int			FrameNumWrap;
	int			PicNum;  //ʹ��ͼ����� PicNum ���һ�����ڲο�ͼ��
	int			LongTermPicNum;//ʹ�ó���ͼ����� LongTermPicNum ��� һ�����ڲο�ͼ��
	int			MaxLongTermFrameIdx; //���ڲο�֡�������Ŀ
	int			LongTermFrameIdx;    //���ڲο�֡
	PICTURE_MARKING  reference_marked_type; //�ο�֡���״̬
	bool		memory_management_control_operation_5_flag; //����ͼ����Ϊ�������ο�



	Picture(ParseSlice* slice, const SliceHeader& sHeader);
};

