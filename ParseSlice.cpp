#include "ParseSlice.h"




ParseSlice::ParseSlice(ParseNalu& nalu) :nalu(nalu)
{
	mbNum = 0;
	macroblock = nullptr;
}

bool ParseSlice::parse(BitStream& bs, const ParsePPS* ppsCache, const ParseSPS* spsCache)
{

	SliceHeader sHeader(nalu);

	bool ret = sHeader.slice_header(bs, ppsCache, spsCache);
	//�ܹ������ֽ�
	size_t size = bs.size;
	//��ȡ���ڼ�bitλ
	size_t bitOffset = (8 - bs.bitsLeft);
	//��ȡ���ڼ��ֽ�
	uint8_t* data = bs.currentPtr;



	SliceData sData;

	sData.slice_data(bs, sHeader);

	/*mbNum = sHeader.sps.pic_width_in_mbs_minus1 * sHeader.sps.pic_height_in_map_units_minus1;

	macroblock = new Macroblock * [mbNum];

	memset(macroblock, NULL, sizeof(Macroblock*)* mbNum);

	for (size_t i = 0; i < mbNum; i++)
	{
		macroblock[i] = new Macroblock(data, bitOffset, sHeader.pps);
		break;
	}*/

	return false;
}


ParseSlice::~ParseSlice()
{
	if (macroblock)
	{

		for (size_t i = 0; i < mbNum; i++)
		{
			if (macroblock[i])
			{
				delete macroblock[i];
				macroblock[i] = nullptr;
			}
		}
		delete macroblock;
		macroblock = nullptr;
	}
}
