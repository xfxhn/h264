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

	uint32_t PicSizeInMbs;
	uint32_t PicWidthInSamplesL;
	uint32_t PicHeightInSamplesL;
	uint32_t PicWidthInSamplesC;
	uint32_t PicHeightInSamplesC;
	uint8_t  slice_type;

	Macroblock** macroblock;
	uint8_t** lumaData;
	uint8_t** chromaCbData;
	uint8_t** chromaCrData;

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
	bool		memory_management_control_operation_6_flag;



	Picture(ParseSlice* slice, const SliceHeader& sHeader);
	~Picture();
};

