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
	PICTURE_MARKING  reference_marked_type; //I,P��Ϊ�ο�֡��mark״̬
public:
	ParseSlice* picturePreviousRef;  //ǰһ���Ѿ�������ɵ�֡


	Picture(ParseSlice* slice);
};

