#pragma once
#include "Common.h"
#include "ParsePPS.h"
#include "ParseSPS.h"
#include "Macroblock.h"

class ParseSlice;
class SliceHeader;

class Picture
{

public:
	ParsePPS pps;
	ParseSPS sps;
	Macroblock** macroblock;

	//����POC��Ҫ�õ���
	int			FrameNumOffset;
	uint16_t	pic_order_cnt_lsb;
	uint16_t	frame_num;
	bool		field_pic_flag;

	int         PicOrderCntMsb;
	int         PicOrderCntLsb;
	int         TopFieldOrderCnt;
	int         BottomFieldOrderCnt;


public:
	int					PicOrderCnt;

	uint16_t			MaxFrameNum;
	uint16_t			FrameNum;				//���ڲο�֡
	uint16_t			FrameNumWrap;			//���ڲο�֡
	int			PicNum;  //ʹ��ͼ����� PicNum ���һ�����ڲο�ͼ��
	int			LongTermPicNum;//ʹ�ó���ͼ����� LongTermPicNum ��� һ�����ڲο�ͼ��
	int			MaxLongTermFrameIdx; //���ڲο�֡�������Ŀ
	int			LongTermFrameIdx;    //���ڲο�֡
	PICTURE_MARKING  reference_marked_type; //�ο�֡���״̬
	bool		memory_management_control_operation_5_flag; //����ͼ����Ϊ�������ο�



	Picture(ParseSlice* slice, const SliceHeader& sHeader);
};

