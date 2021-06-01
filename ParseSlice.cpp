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

void ParseSlice::Intra_4x4_prediction(size_t luma4x4BlkIdx, bool isLuam)
{
	//9��֡��4x4Ԥ��ģʽ
	getIntra4x4PredMode(luma4x4BlkIdx, isLuam);


}
//Intra4x4PredMode���Ƶ�����
void ParseSlice::getIntra4x4PredMode(size_t luma4x4BlkIdx, bool isLuam)
{

	//���㵱ǰ���ȿ����Ͻ�����������뵱ǰ������Ͻ�������������λ��
	int x = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(luma4x4BlkIdx / 4, 4, 8, 8, 0);
	int y = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(luma4x4BlkIdx / 4, 4, 8, 8, 1);


	int maxW = 0;
	int maxH = 0;
	if (isLuam)
	{
		maxW = maxH = 16;
	}
	else
	{
		//ɫ�ȿ�߶ȺͿ��
		maxH = sHeader->sps.MbHeightC;
		maxW = sHeader->sps.MbWidthC;
	}


	

	//������֡������Ӧ
	//����CurrMbAddr����ڰ�����xN��yN�������ں��ĵ�ַ�����������״̬
	int mbAddrA = NA;
	int mbAddrB = NA;

	//λ��4��4��luma4x4BlkIdx�����ϲ��4��4���ȿ���������������״̬
	int luma4x4BlkIdxA = NA;
	int luma4x4BlkIdxB = NA;
	//��ʾ����mbAddrN���Ͻǵ���ԣ���������ڵ�ǰ������Ͻǣ�λ�ã�xN��yN����
	int xW = NA;
	int yW = NA;

	//����λ�õĲ��ֵ ��6-2�� xD, yD ��
	//����λ�ã� xN, yN)
	getMbAddrNAndLuma4x4BlkIdxN(luma4x4BlkIdx, isLuam, mbAddrA, x + (-1), y + 0, maxW, maxH, xW, yW);

	if (mbAddrA != NA)
	{
		luma4x4BlkIdxA = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
	}
	getMbAddrNAndLuma4x4BlkIdxN(luma4x4BlkIdx, isLuam, mbAddrB, x + 0, y + (-1), maxW, maxH, xW, yW);
	if (mbAddrB != NA)
	{
		luma4x4BlkIdxB = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
	}


	bool dcPredModePredictedFlag = false;
	if (true)
	{

	}
}
//mbAddrN ��luma4x4BlkIdxN��N ����A or B���Ƶ�����
void ParseSlice::getMbAddrNAndLuma4x4BlkIdxN(
	size_t luma4x4BlkIdx, bool isLuam, int& mbAddrN, const int xN, const int yN, const int maxW, const int maxH, int& xW, int& yW
)
{
	if (xN < 0 && yN < 0)
	{
		//6.4.5 �ڹ涨�Ĺ��̵�����Ϊ mbAddrD = CurrMbAddr �C PicWidthInMbs �C 1�����Ϊ mbAddrD �Ƿ���á�
		//�� �⣬��CurrMbAddr % PicWidthInMbs ����0 ʱmbAddrD ������ʶΪ�����á�
		int mbAddrD = CurrMbAddr - sHeader->sps.PicWidthInMbs - 1;
		//������
		if (isMbUsable(mbAddrD, CurrMbAddr) || CurrMbAddr % sHeader->sps.PicWidthInMbs == 0)
		{

		}
		else
		{
			mbAddrN = mbAddrD;
		}
	}
	else if (xN < 0 && (yN >= 0 && yN <= maxH - 1))
	{
		int mbAddrA = CurrMbAddr - 1;
		if (isMbUsable(mbAddrA, CurrMbAddr) || CurrMbAddr % sHeader->sps.PicWidthInMbs == 0)
		{

		}
		else
		{
			mbAddrN = mbAddrA;
		}
	}
	else if ((xN >= 0 && xN <= maxW - 1) && yN < 0)
	{
		int mbAddrB = CurrMbAddr - sHeader->sps.PicWidthInMbs;
		if (isMbUsable(mbAddrB, CurrMbAddr))
		{

		}
		else
		{
			mbAddrN = mbAddrB;
		}
	}
	else if (xN > maxW - 1 && yN < 0)
	{
		int mbAddrC = CurrMbAddr - sHeader->sps.PicWidthInMbs + 1;
		if (isMbUsable(mbAddrC, CurrMbAddr) || (CurrMbAddr + 1) % sHeader->sps.PicWidthInMbs == 0)
		{

		}
		else
		{
			mbAddrN = mbAddrC;
		}
	}
	else if ((xN >= 0 && xN <= maxW - 1) && (yN >= 0 && yN <= maxH - 1))
	{

		mbAddrN = CurrMbAddr;
	}

	//��ʾ����mbAddrN���Ͻǵ���ԣ���������ڵ�ǰ������Ͻǣ�λ�ã�xN��yN����
	xW = (xN + maxW) % maxW;
	yW = (yN + maxH) % maxH;


}
void ParseSlice::transformDecode4x4LuamResidualProcess()
{

	if (macroblock[CurrMbAddr]->mode != H264_MB_PART_PRED_MODE::Intra_16x16)
	{
		//scaling();
		for (size_t luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++)
		{
			//int c[4][4] = { 0 };
			//int r[4][4] = { 0 };

			//��ɨ�����
			//inverseScannerProcess(macroblock[CurrMbAddr]->level4x4[i], c);
			//����4*4�в������Լ��任���̣�c Ϊ���룬r Ϊ���������ǲв�����ֵ
			//scalingTransformProcess(c, r, true, false);

			//���㵱ǰ���ȿ����Ͻ�����������뵱ǰ������Ͻ�������������λ��
			int x = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(luma4x4BlkIdx / 4, 4, 8, 8, 0);
			int y = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(luma4x4BlkIdx / 4, 4, 8, 8, 1);


			//4*4Ԥ�����
			Intra_4x4_prediction(luma4x4BlkIdx, true);


			//��·�˲�������
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
void ParseSlice::scalingTransformProcess(int c[4][4], int r[4][4], bool isLuam, bool isChromaCb)
{

	int bitDepth = 0;
	getChromaQuantisationParameters(isChromaCb);
	if (isLuam)
	{
		//������ȣ�ƫ��
		bitDepth = sHeader->sps.BitDepthY;
	}
	else
	{
		bitDepth = sHeader->sps.BitDepthC;
	}

	bool sMbFlag = false;
	//���mb_type=si������Ϊsp������֡��Ԥ��ģʽ
	if (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI || (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP && isInterMode(macroblock[CurrMbAddr]->mode)))
	{
		sMbFlag = 1;
	}

	//����qP���Ƶ�����

	int qP = 0;
	if (isLuam && !sMbFlag)
	{
		qP = macroblock[CurrMbAddr]->QP1Y;
	}
	else if (isLuam && sMbFlag)
	{
		////QSY ��ֵ���ڽ��� mb_type ��SI ��SI ���������к���Լ�Ԥ��ģʽΪ֡���SP ���������к�顣
		qP = sHeader->QSY;
	}
	else if (!isLuam && !sMbFlag)
	{
		qP = macroblock[CurrMbAddr]->QP1C;
	}
	else
	{
		qP = macroblock[CurrMbAddr]->QSC;
	}



	//���Ź���
	if (sHeader->sps.qpprime_y_zero_transform_bypass_flag && macroblock[CurrMbAddr]->QP1Y == 0)
	{
		memcpy(r, c, sizeof(int) * 16);
	}
	else
	{
		//8.5.12.1 Scaling process for residual 4x4 blocks
		int d[4][4] = { 0 };

		//for (size_t i = 0; i <= 3; i++)
		//{
		//	for (size_t j = 0; j <= 3; j++)
		//	{
		//		if (i == 0 && j == 0
		//			&& ((isLuam && macroblock[CurrMbAddr]->mode == H264_MB_PART_PRED_MODE::Intra_16x16) || !isLuam)
		//			)
		//		{
		//			d[0][0] = c[0][0];
		//		}
		//		else
		//		{
		//			if (qP >= 24)
		//			{
		//				d[i][j] = (c[i][j] * LevelScale4x4[qP % 6][i][j]) << (qP / 6 - 4);
		//			}
		//			else //if (qP < 24)
		//			{
		//				d[i][j] = (c[i][j] * LevelScale4x4[qP % 6][i][j] + h264_power2(3 - qP / 6)) >> (4 - qP / 6);
		//			}
		//		}
		//	}
		//}
	}


}
//ɫ���������������Ź��ܵ��Ƶ�����
void ParseSlice::getChromaQuantisationParameters(bool isChromaCb)
{
	//CbCr
	int qPOffset = 0;


	if (isChromaCb)
	{
		//����ɫ������������ƫ����ֵ
		qPOffset = sHeader->pps.chroma_qp_index_offset;
	}
	else
	{
		qPOffset = sHeader->pps.second_chroma_qp_index_offset;
	}
	//ÿ��ɫ�ȷ�����qPI ֵͨ��������ʽ���  //QpBdOffsetC ɫ��ƫ��
	int qPI = Clip3(-(int)sHeader->sps.QpBdOffsetC, 51, macroblock[CurrMbAddr]->QPY + qPOffset);


	int QPC = 0;
	if (qPI < 30)
	{
		QPC = qPI;
	}
	else
	{
		//int qPIs[] = { 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };
		int QPCs[] = { 29, 30, 31, 32, 32, 33, 34, 34, 35, 35, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 39 };

		QPC = QPCs[qPI - 30];
	}


	int QP1C = QPC + sHeader->sps.QpBdOffsetC;

	macroblock[CurrMbAddr]->QPC = QPC;
	macroblock[CurrMbAddr]->QP1C = QP1C;



	if ((SLIECETYPE)sHeader->slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP || (SLIECETYPE)sHeader->slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		//��QSY ���� QPY��QSC ���� QPC
		macroblock[CurrMbAddr]->QSY = macroblock[CurrMbAddr]->QPY;
		macroblock[CurrMbAddr]->QSC = macroblock[CurrMbAddr]->QPC;
	}
	//if (isChromaCb)
	//{
	//	macroblock[CurrMbAddr]->QPCb = QPC;
	//	macroblock[CurrMbAddr]->QP1Cb = QP1C;
	//}
	//else //the chroma component is the Cr component
	//{
	//	macroblock[CurrMbAddr]->QPCr = QPC;
	//	macroblock[CurrMbAddr]->QP1Cr = QP1C;
	//}

}





//���ź������Ƶ�����
void ParseSlice::scaling()
{


	//����mbIsInterFlag��������������
	bool mbIsInterFlag = false;
	//֡��
	if (isInterframe(macroblock[CurrMbAddr]->mode))
	{
		mbIsInterFlag = 1;
	}
	//ͨ��������ʽ�õ�����iYCbCr
	int iYCbCr = 0;
	//separate_colour_plane_flag=1 �ֿ�����
	if (sHeader->sps.separate_colour_plane_flag)
	{
		iYCbCr = sHeader->colour_plane_id;
	}
	else
	{

	}
}
