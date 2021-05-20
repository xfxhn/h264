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


	////�ܹ������ֽ�
	//size_t size = bs.size;
	////��ȡ���ڼ�bitλ
	//size_t bitOffset = (8 - bs.bitsLeft);
	////��ȡ���ڼ��ֽ�
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

	if (macroblock[CurrMbAddr]->mode != H264_MB_PART_PRED_MODE::Intra_16x16)
	{

		for (size_t i = 0; i < 16; i++)
		{
			int c[4][4] = { 0 };
			int r[4][4] = { 0 };

			//����任�� �з����λ�õķ�ֵ ���� һ��4x4 ���ž���
			inverseScannerProcess(macroblock[CurrMbAddr]->level4x4[i], c);
			//����4*4�в������Լ��任���̣�c Ϊ���룬r Ϊ���������ǲв�����ֵ
			scalingTransformProcess(c, r, true);
		}


	}


}

void ParseSlice::inverseScannerProcess(int level4x4Luam[16], int c[4][4])
{

	//����һ������ɨ�裬Ӧ�����ڳ������ʱ��Ż��õ�
	//zig-zag

	c[0][0] = level4x4Luam[0];
	c[0][1] = level4x4Luam[1];
	c[1][0] = level4x4Luam[2];
	c[2][0] = level4x4Luam[3];

	c[1][1] = level4x4Luam[4];
	c[0][2] = level4x4Luam[5];
	c[0][3] = level4x4Luam[6];
	c[1][2] = level4x4Luam[7];

	c[2][1] = level4x4Luam[8];
	c[3][0] = level4x4Luam[9];
	c[3][1] = level4x4Luam[10];
	c[2][2] = level4x4Luam[11];

	c[1][3] = level4x4Luam[12];
	c[2][3] = level4x4Luam[13];
	c[3][2] = level4x4Luam[14];
	c[3][3] = level4x4Luam[15];

}

//���ڲв�4x4������źͱ任����
void ParseSlice::scalingTransformProcess(int c[4][4], int r[4][4], bool isLuam)
{

	int bitDepth = 0;

	if (isLuam)
	{
		//������ȣ�ƫ��
		bitDepth = sHeader->sps.BitDepthY;
	}
	else
	{
		bitDepth = sHeader->sps.BitDepthC;
	}

	int sMbFlag = 0;
	//���mb_type=si������Ϊsp������֡��Ԥ��ģʽ
	if (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI || (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP && isInterMode(macroblock[CurrMbAddr]->mode)))
	{
		sMbFlag = 1;
	}
	else
	{
		sMbFlag = 0;
	}

}
