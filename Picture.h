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

	//解码POC需要用到的
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
	uint16_t			FrameNum;				//短期参考帧
	uint16_t			FrameNumWrap;			//短期参考帧
	int			PicNum;  //使用图像序号 PicNum 标记一个短期参考图像
	int			LongTermPicNum;//使用长期图像序号 LongTermPicNum 标记 一个长期参考图像
	int			MaxLongTermFrameIdx; //长期参考帧的最大数目
	int			LongTermFrameIdx;    //长期参考帧
	PICTURE_MARKING  reference_marked_type; //参考帧标记状态
	bool		memory_management_control_operation_5_flag; //所有图像标记为不用作参考



	Picture(ParseSlice* slice, const SliceHeader& sHeader);
};

