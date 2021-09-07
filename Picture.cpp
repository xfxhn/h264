#include "Picture.h"
#include "ParseSlice.h"
#include "SliceHeader.h"


Picture::Picture(ParseSlice* slice, const SliceHeader& sHeader)
{

	sps = sHeader.sps;
	pps = sHeader.pps;

	macroblock = new Macroblock * [sHeader.PicSizeInMbs];

	for (size_t i = 0; i < sHeader.PicSizeInMbs; i++)
	{
		macroblock[i] = new Macroblock;
		memcpy(macroblock[i], slice->macroblock[i], sizeof(Macroblock));
	}


	pic_order_cnt_lsb = sHeader.pic_order_cnt_lsb;
	frame_num = sHeader.frame_num;
	field_pic_flag = sHeader.field_pic_flag;
	FrameNumOffset = slice->FrameNumOffset;



	PicOrderCntMsb = slice->PicOrderCntMsb;
	PicOrderCntLsb = slice->PicOrderCntLsb;
	TopFieldOrderCnt = slice->TopFieldOrderCnt;
	BottomFieldOrderCnt = slice->BottomFieldOrderCnt;




	reference_marked_type = PICTURE_MARKING::UNKOWN;


	PicOrderCnt = slice->PicOrderCnt;
	//存储每个相应的frame_num
	FrameNum = sHeader.frame_num;
	MaxFrameNum = sHeader.sps.MaxFrameNum;

	MaxLongTermFrameIdx = NA;
	FrameNumWrap = 0;
	PicNum = 0;
	LongTermPicNum = 0;
	LongTermFrameIdx = 0;
	memory_management_control_operation_5_flag = false;
}
