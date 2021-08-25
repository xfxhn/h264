#include "Picture.h"
#include "ParseSlice.h"


Picture::Picture(ParseSlice* slice)
{
	reference_marked_type = PICTURE_MARKING::UNKOWN;

	PicOrderCntMsb = slice->PicOrderCntMsb;
	PicOrderCntLsb = slice->PicOrderCntLsb;
	TopFieldOrderCnt = slice->TopFieldOrderCnt;
	BottomFieldOrderCnt = slice->BottomFieldOrderCnt;


	MaxLongTermFrameIdx = NA;

	FrameNumWrap = 0;
	PicNum = 0;
	LongTermPicNum = 0;
	LongTermFrameIdx = 0;
	memory_management_control_operation_5_flag = false;
}
