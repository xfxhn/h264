#include "Picture.h"
#include "ParseSlice.h"


Picture::Picture(ParseSlice* slice)
{

	PicOrderCntMsb = slice->PicOrderCntMsb;
	PicOrderCntLsb = slice->PicOrderCntLsb;
	TopFieldOrderCnt = slice->TopFieldOrderCnt;
	BottomFieldOrderCnt = slice->BottomFieldOrderCnt;

	FrameNumWrap = 0;
}
