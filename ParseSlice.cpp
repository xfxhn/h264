#include "ParseSlice.h"




ParseSlice::ParseSlice(ParseNalu& nalu) :nalu(nalu)
{
	/*macroblock = nullptr;
	sHeader = nullptr;*/
	mbX = 0;
	mbY = 0;
	this->sHeader = nullptr;
	macroblock = nullptr;
	CurrMbAddr = 0;
}

bool ParseSlice::parse(BitStream& bs, const ParsePPS* ppsCache, const ParseSPS* spsCache)
{

	////SliceHeader sHeader(nalu);
	this->sHeader = new SliceHeader(nalu);
	sHeader->slice_header(bs, ppsCache, spsCache);


	////总共多少字节
	//size_t size = bs.size;
	////读取到第几bit位
	//size_t bitOffset = (8 - bs.bitsLeft);
	////读取到第几字节
	//uint8_t* data = bs.currentPtr;



	this->macroblock = new Macroblock * [sHeader->PicSizeInMbs];

	memset(macroblock, NULL, sizeof(Macroblock*) * sHeader->PicSizeInMbs);


	for (size_t i = 0; i < sHeader->PicSizeInMbs; i++)
	{
		macroblock[i] = new Macroblock(*this);
	}
	SliceData sData;

	sData.slice_data(bs, *this);


	return false;
}


ParseSlice::~ParseSlice()
{
	if (macroblock)
	{
		for (size_t i = 0; i < sHeader->PicSizeInMbs; i++)
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
	if (sHeader)
	{
		delete sHeader;
	}

}

void ParseSlice::transformDecode4x4LuamResidualProcess()
{

	if (macroblock[CurrMbAddr])
	{

	}


}
