#pragma once

class ParseSlice;
class Picture
{

public:
	int         PicOrderCntMsb;
	int         PicOrderCntLsb;
	int         TopFieldOrderCnt;
	int         BottomFieldOrderCnt;

	int FrameNumWrap;
	int PicNum;
	int LongTermPicNum;
	int LongTermFrameIdx;
	PICTURE_MARKING  reference_marked_type; //I,P作为参考帧的mark状态
public:
	ParseSlice* picturePreviousRef;  //前一个已经解码完成的帧


	Picture(ParseSlice* slice);
};

