#include "Picture.h"
#include "ParseSlice.h"
#include "SliceHeader.h"


Picture::Picture(ParseSlice* slice, const SliceHeader& sHeader)
{

	sps = sHeader.sps;
	pps = sHeader.pps;
	PicSizeInMbs = sHeader.PicSizeInMbs;

	macroblock = new Macroblock * [PicSizeInMbs];
	for (size_t i = 0; i < PicSizeInMbs; i++)
	{
		macroblock[i] = new Macroblock;
		memcpy(macroblock[i], slice->macroblock[i], sizeof(Macroblock));
	}

	PicWidthInSamplesL = slice->PicWidthInSamplesL;
	PicHeightInSamplesL = slice->PicHeightInSamplesL;

	lumaData = new uint8_t * [PicWidthInSamplesL];
	for (size_t i = 0; i < PicWidthInSamplesL; i++)
	{
		lumaData[i] = new uint8_t[PicHeightInSamplesL]();
		memcpy(lumaData[i], slice->lumaData[i], sizeof(uint8_t) * PicHeightInSamplesL);
	}

	PicWidthInSamplesC = slice->PicWidthInSamplesC;
	PicHeightInSamplesC = slice->PicHeightInSamplesC;
	chromaCbData = new uint8_t * [PicWidthInSamplesC];
	chromaCrData = new uint8_t * [PicWidthInSamplesC];

	for (size_t i = 0; i < PicWidthInSamplesC; i++)
	{
		chromaCbData[i] = new uint8_t[PicHeightInSamplesC]();
		memcpy(chromaCbData[i], slice->chromaCbData[i], sizeof(uint8_t) * PicHeightInSamplesC);

		chromaCrData[i] = new uint8_t[PicHeightInSamplesC]();
		memcpy(chromaCrData[i], slice->chromaCrData[i], sizeof(uint8_t) * PicHeightInSamplesC);
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

Picture::~Picture()
{
	if (macroblock)
	{
		for (size_t i = 0; i < PicSizeInMbs; i++)
		{
			if (macroblock[i])
			{
				delete macroblock[i];
				macroblock[i] = nullptr;
			}

		}
		delete[] macroblock;
		macroblock = nullptr;
	}

	if (lumaData)
	{
		for (size_t i = 0; i < PicWidthInSamplesL; i++)
		{
			if (lumaData[i])
			{
				delete[] lumaData[i];
				lumaData[i] = nullptr;
			}
		}
		delete[] lumaData;
		lumaData = nullptr;
	}

	if (chromaCbData && chromaCrData)
	{
		for (size_t i = 0; i < PicWidthInSamplesC; i++)
		{
			if (chromaCbData[i])
			{
				delete[] chromaCbData[i];
				chromaCbData[i] = nullptr;
			}

			if (chromaCrData[i])
			{
				delete[] chromaCrData[i];
				chromaCrData[i] = nullptr;
			}
		}
		delete[] chromaCbData;
		chromaCbData = nullptr;

		delete[] chromaCrData;
		chromaCrData = nullptr;
	}



}
