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

	int Intra4x4PredMode = macroblock[CurrMbAddr]->Intra4x4PredMode[luma4x4BlkIdx];
	cout << "Intra4x4PredMode" << Intra4x4PredMode << endl;



}
//Intra4x4PredMode���Ƶ�����
void ParseSlice::getIntra4x4PredMode(size_t luma4x4BlkIdx, bool isLuam)
{

	//���㵱ǰ���ȿ����Ͻ�����������뵱ǰ������Ͻ�������������λ��
	int x = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 0);
	int y = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 1);


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
		//���������
		luma4x4BlkIdxA = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
	}

	getMbAddrNAndLuma4x4BlkIdxN(luma4x4BlkIdx, isLuam, mbAddrB, x + 0, y + (-1), maxW, maxH, xW, yW);
	if (mbAddrB != NA)
	{
		//�ϲ�������
		luma4x4BlkIdxB = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
	}


	bool dcPredModePredictedFlag = false;
	/*�� P �� B Ƭ�У�֡�ڱ���ĺ����ڽ��������ǲ��õ�֡����롣
		�����䷨Ԫ�ص��� 1 ʱ����ʾ֡�ڱ���ĺ�鲻����֡�����ĺ���������Ϊ�Լ���Ԥ�⣬
		��֡�ڱ���ĺ��ֻ�����ڽ�֡�ڱ���ĺ���������Ϊ�Լ���Ԥ�⣻�����䷨Ԫ�ص��� 0 ʱ����ʾ�������������ơ�*/
	if (mbAddrA == NA ||
		mbAddrB == NA ||
		(mbAddrA != NA && isInterframe(macroblock[mbAddrA]->mode) && sHeader->pps.constrained_intra_pred_flag) ||
		(mbAddrB != NA && isInterframe(macroblock[mbAddrB]->mode)) && sHeader->pps.constrained_intra_pred_flag)
	{
		dcPredModePredictedFlag = true;
	}

	int intraMxMPredModeA = NA;
	int intraMxMPredModeB = NA;


	if (dcPredModePredictedFlag ||
		(mbAddrA != NA && macroblock[mbAddrA]->mode != H264_MB_PART_PRED_MODE::Intra_4x4 && macroblock[mbAddrA]->mode != H264_MB_PART_PRED_MODE::Intra_8x8)
		)
	{
		intraMxMPredModeA = Intra_4x4_DC;
	}
	else
	{
		//ѡ��ǰ���4*4�������4*4�ӿ��Ԥ��ģʽ
		if (macroblock[mbAddrA]->mode == H264_MB_PART_PRED_MODE::Intra_4x4)
		{
			intraMxMPredModeA = macroblock[mbAddrA]->Intra4x4PredMode[luma4x4BlkIdxA];
		}
		else  //8*8
		{
			intraMxMPredModeA = macroblock[mbAddrA]->Intra4x4PredMode[luma4x4BlkIdxA >> 2];
		}
	}

	if (dcPredModePredictedFlag ||
		(mbAddrB != NA && macroblock[mbAddrB]->mode != H264_MB_PART_PRED_MODE::Intra_4x4 && macroblock[mbAddrB]->mode != H264_MB_PART_PRED_MODE::Intra_8x8)
		)
	{
		intraMxMPredModeB = Intra_4x4_DC;
	}
	else
	{
		//ѡ��ǰ���4*4���ϲ��� 4*4�ӿ��Ԥ��ģʽ
		if (macroblock[mbAddrB]->mode == H264_MB_PART_PRED_MODE::Intra_4x4)
		{
			intraMxMPredModeB = macroblock[mbAddrB]->Intra4x4PredMode[luma4x4BlkIdxB];
		}
		else //8*8
		{
			intraMxMPredModeB = macroblock[mbAddrB]->Intra4x4PredMode[luma4x4BlkIdxB >> 2];
		}
	}

	//�������Ϸ����ڿ��Ԥ��ģʽ��ѡȡ��С��һ����ΪԤ�ȶ���ģʽ
	int predIntra4x4PredMode = min(intraMxMPredModeA, intraMxMPredModeB);

	//������ָ����Ҫ��Ҫ�����Ԥ��ֵ������ã���ô���Ԥ��ֵ���ǵ�ǰ���֡��Ԥ��ģʽ������ʹӺ�����ȡ��Ԥ��ģʽ�м��㡣
	if (macroblock[CurrMbAddr]->prev_intra4x4_pred_mode_flag[luma4x4BlkIdx])
	{
		//�ж������ж�ȡ�ı�־λprev_intra4x4_pred_mode_flag������ñ�־λΪ1����Ԥ�ȶ���ģʽ���ǵ�ǰ���Ԥ��ģʽ
		macroblock[CurrMbAddr]->Intra4x4PredMode[luma4x4BlkIdx] = predIntra4x4PredMode;
	}
	else
	{
		if (macroblock[CurrMbAddr]->rem_intra4x4_pred_mode[luma4x4BlkIdx] < predIntra4x4PredMode)
		{
			macroblock[CurrMbAddr]->Intra4x4PredMode[luma4x4BlkIdx] = macroblock[CurrMbAddr]->rem_intra4x4_pred_mode[luma4x4BlkIdx];
		}
		else
		{
			macroblock[CurrMbAddr]->Intra4x4PredMode[luma4x4BlkIdx] = macroblock[CurrMbAddr]->rem_intra4x4_pred_mode[luma4x4BlkIdx] + 1;
		}
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
		scaling(true, false);
		for (size_t luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++)
		{
			int c[4][4] = { 0 };
			int r[4][4] = { 0 };

			//��ɨ�����  �ڽ�����������Ҫ�����һά����ת���ɶ�ά���������������
			inverseScanner4x4Process(macroblock[CurrMbAddr]->level4x4[luma4x4BlkIdx], c);
			//����4*4�в������Լ��任���̣�c Ϊ���룬r Ϊ���������ǲв�����ֵ
			//scalingTransformProcess(c, r, true, false);

			//���㵱ǰ���ȿ����Ͻ�����������뵱ǰ������Ͻ�������������λ��
			int x = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 0);
			int y = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 1);


			//4*4Ԥ�����
			Intra_4x4_prediction(luma4x4BlkIdx, true);


			//��·�˲�������
		}


	}


}

void ParseSlice::inverseScanner4x4Process(int value[16], int c[4][4])
{

	//����һ������ɨ�裬Ӧ�����ڳ������ʱ��Ż��õ�
	//zig-zag

	c[0][0] = value[0];
	c[0][1] = value[1];
	c[1][0] = value[2];
	c[2][0] = value[3];

	c[1][1] = value[4];
	c[0][2] = value[5];
	c[0][3] = value[6];
	c[1][2] = value[7];

	c[2][1] = value[8];
	c[3][0] = value[9];
	c[3][1] = value[10];
	c[2][2] = value[11];

	c[1][3] = value[12];
	c[2][3] = value[13];
	c[3][2] = value[14];
	c[3][3] = value[15];

}

void ParseSlice::inverseScanner8x8Process(int values[64], int c[8][8])
{

	//8x8 zig-zag scan
	c[0][0] = values[0];
	c[0][1] = values[1];
	c[1][0] = values[2];
	c[2][0] = values[3];
	c[1][1] = values[4];
	c[0][2] = values[5];
	c[0][3] = values[6];
	c[1][2] = values[7];
	c[2][1] = values[8];
	c[3][0] = values[9];
	c[4][0] = values[10];
	c[3][1] = values[11];
	c[2][2] = values[12];
	c[1][3] = values[13];
	c[0][4] = values[14];
	c[0][5] = values[15];

	c[1][4] = values[16];
	c[2][3] = values[17];
	c[3][2] = values[18];
	c[4][1] = values[19];
	c[5][0] = values[20];
	c[6][0] = values[21];
	c[5][1] = values[22];
	c[4][2] = values[23];
	c[3][3] = values[24];
	c[2][4] = values[25];
	c[1][5] = values[26];
	c[0][6] = values[27];
	c[0][7] = values[28];
	c[1][6] = values[29];
	c[2][5] = values[30];
	c[3][4] = values[31];

	c[4][3] = values[32];
	c[5][2] = values[33];
	c[6][1] = values[34];
	c[7][0] = values[35];
	c[7][1] = values[36];
	c[6][2] = values[37];
	c[5][3] = values[38];
	c[4][4] = values[39];
	c[3][5] = values[40];
	c[2][6] = values[41];
	c[1][7] = values[42];
	c[2][7] = values[43];
	c[3][6] = values[44];
	c[4][5] = values[45];
	c[5][4] = values[46];
	c[6][3] = values[47];

	c[7][2] = values[48];
	c[7][3] = values[49];
	c[6][4] = values[50];
	c[5][5] = values[51];
	c[4][6] = values[52];
	c[3][7] = values[53];
	c[4][7] = values[54];
	c[5][6] = values[55];
	c[6][5] = values[56];
	c[7][4] = values[57];
	c[7][5] = values[58];
	c[6][6] = values[59];
	c[5][7] = values[60];
	c[6][7] = values[61];
	c[7][6] = values[62];
	c[7][7] = values[63];
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




//���������о���Ȩ�⸴�ӶȺ�ѹ��Ч��֮���֣�HEVC�ı任��Ӧ����������������
//1.�任���е�ϵ��������8bit��ʾ����������λ����2.�任�˵ĵ�һ����������Ԫ����һ����������Ӧ��������64��
//������������ǰ�ᣬHEVC�ж�DCT�任�˽��������ţ���ÿ���任���е�ϵ������ 6 + log2(N) / 2 bit��ȡ����
//����ٶԸ���ϵ������һ���ĵ���֮�󣨵�����ԭ�����ڣ�ʹ�õ�����ı任�������ϲ����ᵽ��ԭ��4 - 6������ԭ��1 - 3֮���ҵ�һ����õ�ƽ��㣩���õ���ͬ�ߴ��ı任�ˡ�
//��������������������������������
//ԭ�����ӣ�https ://blog.csdn.net/ftlisdcr/article/details/54345151
//���ź������Ƶ�����
void ParseSlice::scaling(bool isLuam, bool isChromaCb)
{


	//����mbIsInterFlag��������������
	bool mbIsInterFlag = false;
	//֡��
	if (isInterframe(macroblock[CurrMbAddr]->mode))
	{
		mbIsInterFlag = true;
	}
	//ͨ��������ʽ�õ�����iYCbCr
	int iYCbCr = 0;
	//separate_colour_plane_flag=1 �ֿ�����
	if (sHeader->sps.separate_colour_plane_flag)
	{
		//colour_plane_id����0��1��2�ֱ��Ӧ��Y��Cb��Crƽ�档
		iYCbCr = sHeader->colour_plane_id;
	}
	else
	{
		if (isLuam)
		{
			iYCbCr = 0;
		}
		else if (isChromaCb)
		{
			iYCbCr = 1;
		}
		else
		{
			iYCbCr = 2;
		}
	}

	int weightScale4x4[4][4] = { 0 };
	//ȡ��һ��scalinglist,��BP/MP/EP�У�weightScale4x4��ȫΪ16��4x4����
	//�Ƿ���֡�仹��֡�ڣ�ȡ��Ӧ�������ž���  <3=֡�ڣ�>3<=6��Ӧ��֡��   mbIsInterFlag ��3:0
	inverseScanner4x4Process(sHeader->ScalingList4x4[iYCbCr + ((mbIsInterFlag) ? 3 : 0)], weightScale4x4);

	//LevelScale4x4( m, i, j ) = weightScale4x4( i, j ) * normAdjust4x4( m, i, j )

	int v4x4[6][3] =
	{
		{10, 16, 13},
		{11, 18, 14},
		{13, 20, 16},
		{14, 23, 18},
		{16, 25, 20},
		{18, 29, 23},
	};

	//normAdjust4x4
	for (size_t m = 0; m < 6; m++)
	{
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				if (i % 2 == 0 && j % 2 == 0)
				{
					LevelScale4x4[m][i][j] = weightScale4x4[i][j] * v4x4[m][0];
				}
				else if (i % 2 == 1 && j % 2 == 1)
				{
					LevelScale4x4[m][i][j] = weightScale4x4[i][j] * v4x4[m][1];
				}
				else
				{
					LevelScale4x4[m][i][j] = weightScale4x4[i][j] * v4x4[m][2];
				}
			}
		}
	}


	int weightScale8x8[8][8] = { 0 };
	//�Ƿ���֡�仹��֡�ڣ�ȡ��Ӧ�������ž���  0=֡�ڣ�1=��Ӧ��֡��   mbIsInterFlag
	inverseScanner8x8Process(sHeader->ScalingList8x8[2 * iYCbCr + mbIsInterFlag], weightScale8x8);


	int v8x8[6][6] =
	{
		{20, 18, 32, 19, 25, 24},
		{22, 19, 35, 21, 28, 26},
		{26, 23, 42, 24, 33, 31},
		{28, 25, 45, 26, 35, 33},
		{32, 28, 51, 30, 40, 38},
		{36, 32, 58, 34, 46, 43},
	};

	//LevelScale8x8( m, i, j ) = weightScale8x8( i, j ) * normAdjust8x8( m, i, j )

	for (size_t m = 0; m < 6; m++)
	{
		for (size_t i = 0; i < 8; i++)
		{
			for (size_t j = 0; j < 8; j++)
			{
				if (i % 4 && j % 4 == 0)
				{
					LevelScale8x8[m][i][j] = weightScale8x8[i][j] * v8x8[m][0];
				}
				else if (i % 2 == 1 && j % 2 == 1)
				{
					LevelScale8x8[m][i][j] = weightScale8x8[i][j] * v8x8[m][1];
				}
				else if (i % 4 == 2 && j % 4 == 2)
				{
					LevelScale8x8[m][i][j] = weightScale8x8[i][j] * v8x8[m][2];
				}
				else if ((i % 4 == 0 && j % 2 == 1) || (i % 2 == 1 && j % 4 == 0))
				{
					LevelScale8x8[m][i][j] = weightScale8x8[i][j] * v8x8[m][3];
				}
				else if ((i % 4 == 0 && j % 4 == 2) || (i % 4 == 2 && j % 4 == 0))
				{
					LevelScale8x8[m][i][j] = weightScale8x8[i][j] * v8x8[m][4];
				}
				else
				{
					LevelScale8x8[m][i][j] = weightScale8x8[i][j] * v8x8[m][5];
				}
			}
		}
	}
}
