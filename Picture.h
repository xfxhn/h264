#pragma once
#include "NaluType.h"
class ParseSlice;
class Picture
{

public:
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
public:
	ParseSlice* picturePreviousRef;  //ǰһ���Ѿ�������ɵ�֡


	Picture(ParseSlice* slice);
};

