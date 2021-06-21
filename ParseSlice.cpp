#include "ParseSlice.h"




ParseSlice::ParseSlice(ParseNalu& nalu) :nalu(nalu)
{
	/*macroblock = nullptr;
	sHeader = nullptr;*/
	mbX = 0;
	mbY = 0;
	this->sHeader = nullptr;
	macroblock = nullptr;
	lumaData = nullptr;

	CurrMbAddr = 0;
}

bool ParseSlice::parse(BitStream& bs, const ParsePPS* ppsCache, const ParseSPS* spsCache)
{

	////SliceHeader sHeader(nalu);
	this->sHeader = new SliceHeader(nalu);
	sHeader->slice_header(bs, ppsCache, spsCache);

	lumaData = new uint8_t[sHeader->sps.PicWidthInSamplesL * sHeader->sps.PicHeightInSamplesL];



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
		sHeader = nullptr;
	}

	if (lumaData)
	{
		delete lumaData;
		lumaData = nullptr;
	}

}

void ParseSlice::Intra_4x4_prediction(size_t luma4x4BlkIdx, bool isLuam)
{
	//9种帧内4x4预测模式
	getIntra4x4PredMode(luma4x4BlkIdx, isLuam);

	int Intra4x4PredMode = macroblock[CurrMbAddr]->Intra4x4PredMode[luma4x4BlkIdx];



	//相对4*4块左上角像素块的位置，13个参考像素x和y的坐标
	//参考文档https://blog.csdn.net/shaqoneal/article/details/78820128


	int referenceCoordinateX[13] = { -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7 };
	int referenceCoordinateY[13] = { -1,  0,  1,  2,  3, -1, -1, -1, -1, -1, -1, -1, -1 };

	/*int referenceCoordinateX[13] = { -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7 };
	int referenceCoordinateY[13] = { 3,  2,  1,  0, -1,   1,  -1, -1, -1, -1, -1, -1, -1 };*/

	//获取宏块所在相对左上角像素点的x坐标和y坐标
	int xO = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 0);
	int yO = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 1);
	//存储相对4*4子块左上角像素点13个参考样点值，坐标对应值
	//这里取最小45是为了让对应的坐标取到相对应的值，而不会重复
	int samples[45];
	memset(samples, -1, sizeof(int) * 45);
#define P(x, y)  samples[((y) + 1) * 9 + ((x) + 1)]

	for (size_t i = 0; i < 13; i++)
	{
		int x = referenceCoordinateX[i];
		int y = referenceCoordinateY[i];
		int xN = xO + x;
		int yN = yO + y;

		int maxW = 0;
		int maxH = 0;
		if (isLuam)
		{
			maxW = maxH = 16;
		}
		else
		{
			//色度块高度和宽度
			maxH = sHeader->sps.MbHeightC;
			maxW = sHeader->sps.MbWidthC;
		}

		int mbAddrN = NA;
		int xW = NA;
		int yW = NA;
		int luma4x4BlkIdxN = 0;
		getMbAddrNAndLuma4x4BlkIdxN(isLuam, mbAddrN, xN, yN, maxW, maxH, xW, yW);

		if (mbAddrN != NA)
		{
			//参考像素所处宏块的子块索引
			luma4x4BlkIdxN = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
		}

		if (mbAddrN == NA
			|| (isInterframe(macroblock[mbAddrN]->mode) && sHeader->pps.constrained_intra_pred_flag)
			|| (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI && sHeader->pps.constrained_intra_pred_flag)
			|| (x > 3) && (luma4x4BlkIdx == 3 || luma4x4BlkIdx == 11)
			)
		{
			P(x, y) = -1;
		}
		else
		{
			int xM = InverseRasterScan(mbAddrN, 16, 16, sHeader->sps.PicWidthInSamplesL, 0);
			int yM = InverseRasterScan(mbAddrN, 16, 16, sHeader->sps.PicWidthInSamplesL, 1);
		}
	}

	//如果p(4,-1)、p(5,-1)、p(6,-1)、p(7,-1)都不可获得，那么p(4,-1)、p(5,-1)、p(6,-1)、p(7,-1)这4个无效点都将替换为p(3,-1)的值参与预测运算。
	if (P(4, -1) < 0 && P(5, -1) < 0 && P(6, -1) < 0 && P(7, -1) < 0 && P(3, -1) >= 0)
	{
		P(4, -1) = P(3, -1);
		P(5, -1) = P(3, -1);
		P(6, -1) = P(3, -1);
		P(7, -1) = P(3, -1);
	}




	if (Intra4x4PredMode == Intra_4x4_Vertical)//垂直
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0)
		{
			for (size_t y = 0; y < 4; y++)
			{
				for (size_t x = 0; x < 4; x++)
				{
					//pred4x4L[ x, y ] = p[ x, −1 ], with x, y = 0..3
					macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = P(x, -1);
				}
			}

		}

	}
	else if (Intra4x4PredMode == Intra_4x4_Horizontal)//水平
	{
		if (P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0)
		{
			for (size_t y = 0; y < 4; y++)
			{
				for (size_t x = 0; x < 4; x++)
				{
					//pred4x4L[ x, y ] = p[ −1, y ], with x,y = 0..3
					macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = P(-1, y);
				}
			}
		}

	}
	else if (Intra4x4PredMode == Intra_4x4_DC)//均值
	{
		//四舍五入求平均数
		int val = 0;
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0)
		{
			val = (P(0, -1) + P(1, -1) + P(2, -1) + P(3, -1) + P(-1, 0) + P(-1, 1) + P(-1, 2) + P(-1, 3) + 4) >> 3;
		}
		else if ((P(0, -1) < 0 || P(1, -1) < 0 || P(2, -1) < 0 || P(3, -1) < 0) && (P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0))
		{
			val = (P(-1, 0) + P(-1, 1) + P(-1, 2) + P(-1, 3) + 2) >> 2;
		}
		else if ((P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0) && (P(-1, 0) < 0 || P(-1, 1) < 0 || P(-1, 2) < 0 || P(-1, 3) < 0))
		{
			val = (P(0, -1) + P(1, -1) + P(2, -1) + P(3, -1) + 2) >> 2;
		}
		else
		{
			//h264一个亮度像素都是8个bit
			val = 1 << (sHeader->sps.BitDepthY - 1); //128 在h264
		}


		for (size_t x = 0; x < 4; x++)
		{
			for (size_t y = 0; y < 4; y++)
			{
				macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = val;
			}
		}

	}
	else if (Intra4x4PredMode == Intra_4x4_Diagonal_Down_Left)
	{
		//加权预测
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0)
		{
			for (size_t y = 0; y < 4; y++)
			{
				for (size_t x = 0; x < 4; x++)
				{
					if (x == 3 && y == 3)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(6, -1) + 3 * P(7, -1) + 2) >> 2;
					}
					else
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(x + y, -1) + 2 * P(x + y + 1, -1) + P(x + y + 2, -1) + 2) >> 2;
					}
				}
			}
		}
	}
	else if (Intra4x4PredMode == Intra_4x4_Diagonal_Down_Right)
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(-1, -1) >= 0 && P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0)
		{
			for (size_t y = 0; y <= 3; y++)
			{
				for (size_t x = 0; x <= 3; x++)
				{
					if (x > y)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(x - y - 2, -1) + 2 * P(x - y - 1, -1) + P(x - y, -1) + 2) >> 2; //pred4x4L[y * 4 + x]
					}
					else if (x < y)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(-1, y - x - 2) + 2 * P(-1, y - x - 1) + P(-1, y - x) + 2) >> 2; //pred4x4L[y * 4 + x]
					}
					else //if (x == y)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(0, -1) + 2 * P(-1, -1) + P(-1, 0) + 2) >> 2; //pred4x4L[y * 4 + x]
					}
				}
			}
		}
	}
	else if (Intra4x4PredMode == Intra_4x4_Vertical_Right)
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(-1, -1) >= 0 && P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0)
		{
			for (size_t y = 0; y <= 3; y++)
			{
				for (size_t x = 0; x <= 3; x++)
				{
					int zVR = 2 * x - y;

					if (zVR == 0 || zVR == 2 || zVR == 4 || zVR == 6)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(x - (y >> 1) - 1, -1) + P(x - (y >> 1), -1) + 1) >> 1;
					}
					else if (zVR == 1 || zVR == 3 || zVR == 5)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(x - (y >> 1) - 2, -1) + 2 * P(x - (y >> 1) - 1, -1) + P(x - (y >> 1), -1) + 2) >> 2;
					}
					else if (zVR == -1)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(-1, 0) + 2 * P(-1, -1) + P(0, -1) + 2) >> 2;
					}
					else //if (zVR == -2 || zVR == -3)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(-1, y - 1) + 2 * P(-1, y - 2) + P(-1, y - 3) + 2) >> 2;
					}
				}
			}
		}
	}
	else if (Intra4x4PredMode == Intra_4x4_Horizontal_Down)
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(-1, -1) >= 0 && P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0)
		{
			for (size_t y = 0; y <= 3; y++)
			{
				for (size_t x = 0; x <= 3; x++)
				{
					int zHD = 2 * y - x;

					if (zHD == 0 || zHD == 2 || zHD == 4 || zHD == 6)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(-1, y - (x >> 1) - 1) + P(-1, y - (x >> 1)) + 1) >> 1;
					}
					else if (zHD == 1 || zHD == 3 || zHD == 5)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(-1, y - (x >> 1) - 2) + 2 * P(-1, y - (x >> 1) - 1) + P(-1, y - (x >> 1)) + 2) >> 2;
					}
					else if (zHD == -1)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(-1, 0) + 2 * P(-1, -1) + P(0, -1) + 2) >> 2;
					}
					else //if (zHD == -2 || zHD == -3)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(x - 1, -1) + 2 * P(x - 2, -1) + P(x - 3, -1) + 2) >> 2;
					}
				}
			}
		}
	}
	else if (Intra4x4PredMode == Intra_4x4_Vertical_Left)
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0)
		{
			for (size_t y = 0; y <= 3; y++)
			{
				for (size_t x = 0; x <= 3; x++)
				{
					if (y == 0 || y == 2)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(x + (y >> 1), -1) + P(x + (y >> 1) + 1, -1) + 1) >> 1;
					}
					else //if (y == 1 || y == 3)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(x + (y >> 1), -1) + 2 * P(x + (y >> 1) + 1, -1) + P(x + (y >> 1) + 2, -1) + 2) >> 2;
					}
				}
			}
		}
	}
	else if (Intra4x4PredMode == Intra_4x4_Horizontal_Up)
	{
		if (P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0)
		{
			for (size_t y = 0; y <= 3; y++)
			{
				for (size_t x = 0; x <= 3; x++)
				{
					int zHU = x + 2 * y;

					if (zHU == 0 || zHU == 2 || zHU == 4)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(-1, y + (x >> 1)) + P(-1, y + (x >> 1) + 1) + 1) >> 1;
					}
					else if (zHU == 1 || zHU == 3)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(-1, y + (x >> 1)) + 2 * P(-1, y + (x >> 1) + 1) + P(-1, y + (x >> 1) + 2) + 2) >> 2;
					}
					else if (zHU == 5)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = (P(-1, 2) + 3 * P(-1, 3) + 2) >> 2;
					}
					else //if (zHU > 5)
					{
						macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y] = P(-1, 3);
					}
				}
			}
		}
	}

#undef P;

}
//Intra4x4PredMode的推导过程
void ParseSlice::getIntra4x4PredMode(size_t luma4x4BlkIdx, bool isLuam)
{

	//计算当前亮度块左上角亮度样点距离当前宏块左上角亮度样点的相对位置
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
		//色度块高度和宽度
		maxH = sHeader->sps.MbHeightC;
		maxW = sHeader->sps.MbWidthC;
	}

	//不考虑帧场自适应
	//等于CurrMbAddr或等于包含（xN，yN）的相邻宏块的地址，及其可用性状态
	int mbAddrA = NA;
	int mbAddrB = NA;

	//位于4×4块luma4x4BlkIdx左侧和上侧的4×4亮度块的索引及其可用性状态
	int luma4x4BlkIdxA = NA;
	int luma4x4BlkIdxB = NA;
	//表示与宏块mbAddrN左上角的相对（不是相对于当前宏块左上角）位置（xN，yN）。
	int xW = NA;
	int yW = NA;

	//亮度位置的差分值 表6-2（ xD, yD ）
	//亮度位置（ xN, yN)
	getMbAddrNAndLuma4x4BlkIdxN(isLuam, mbAddrA, x + (-1), y + 0, maxW, maxH, xW, yW);

	if (mbAddrA != NA)
	{
		//左侧宏块子块索引
		luma4x4BlkIdxA = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
	}

	getMbAddrNAndLuma4x4BlkIdxN(isLuam, mbAddrB, x + 0, y + (-1), maxW, maxH, xW, yW);
	if (mbAddrB != NA)
	{
		//上侧宏块子块索引
		luma4x4BlkIdxB = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
	}


	bool dcPredModePredictedFlag = false;
	/*在 P 和 B 片中，帧内编码的宏块的邻近宏块可能是采用的帧间编码。
		当本句法元素等于 1 时，表示帧内编码的宏块不能用帧间编码的宏块的像素作为自己的预测，
		即帧内编码的宏块只能用邻近帧内编码的宏块的像素作为自己的预测；而本句法元素等于 0 时，表示不存在这种限制。*/
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
		//选择当前宏块4*4的左侧宏块4*4子块的预测模式
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
		//选择当前宏块4*4的上侧宏块 4*4子块的预测模式
		if (macroblock[mbAddrB]->mode == H264_MB_PART_PRED_MODE::Intra_4x4)
		{
			intraMxMPredModeB = macroblock[mbAddrB]->Intra4x4PredMode[luma4x4BlkIdxB];
		}
		else //8*8
		{
			intraMxMPredModeB = macroblock[mbAddrB]->Intra4x4PredMode[luma4x4BlkIdxB >> 2];
		}
	}

	//从左侧和上方相邻块的预测模式中选取较小的一个作为预先定义模式
	int predIntra4x4PredMode = min(intraMxMPredModeA, intraMxMPredModeB);

	//码流中指定了要不要用这个预测值。如果用，那么这个预测值就是当前块的帧内预测模式；否则就从后续读取的预测模式中计算。
	if (macroblock[CurrMbAddr]->prev_intra4x4_pred_mode_flag[luma4x4BlkIdx])
	{
		//判断码流中读取的标志位prev_intra4x4_pred_mode_flag，如果该标志位为1，则预先定义模式就是当前块的预测模式
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
//mbAddrN 和luma4x4BlkIdxN（N 等于A or B）推导如下
void ParseSlice::getMbAddrNAndLuma4x4BlkIdxN(
	bool isLuam, int& mbAddrN, const int xN, const int yN, const int maxW, const int maxH, int& xW, int& yW
)
{
	if (xN < 0 && yN < 0)
	{
		//6.4.5 节规定的过程的输入为 mbAddrD = CurrMbAddr – PicWidthInMbs – 1，输出为 mbAddrD 是否可用。
		//另 外，当CurrMbAddr % PicWidthInMbs 等于0 时mbAddrD 将被标识为不可用。
		int mbAddrD = CurrMbAddr - sHeader->sps.PicWidthInMbs - 1;
		//不可用
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

	//表示与宏块mbAddrN左上角的相对（不是相对于当前宏块左上角）位置（xN，yN）。
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

			//逆扫描过程  在解码器端则需要将这个一维数据转换成二维数组或矩阵进行运算
			inverseScanner4x4Process(macroblock[CurrMbAddr]->level4x4[luma4x4BlkIdx], c);
			//调用4*4残差缩放以及变换过程，c 为输入，r 为输出。输出是残差样点值
			scalingTransformProcess(c, r, true, false);

			//计算当前亮度块左上角亮度样点距离当前宏块左上角亮度样点的相对位置
			int xO = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 0);
			int yO = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 1);
			//对近似均匀分布的语法元素，在编码和解码时选择旁路（bypass）模式，可以免除上下文建模，提高编解码的速度。


			//4*4预测过程
			Intra_4x4_prediction(luma4x4BlkIdx, true);

			int u[16] = { 0 };
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; i < 4; j++)
				{
					//Clip1Y( x ) = Clip3( 0， ( 1 << BitDepthY ) − 1 ， x ) 
					Clip3(0, (1 << sHeader->sps.BitDepthY) - 1, 2);
				}
			}

			//环路滤波器之前的图像生成
			Picture_construction_process_prior_to_deblocking_filter_process(u, "4*4", luma4x4BlkIdx, true);
		}


	}


}

void ParseSlice::inverseScanner4x4Process(int value[16], int c[4][4])
{

	//还有一个反域扫描，应该是在场编码的时候才会用到
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

//用于残差4x4块的缩放和变换过程
void ParseSlice::scalingTransformProcess(int c[4][4], int r[4][4], bool isLuam, bool isChromaCb)
{

	int bitDepth = 0;
	getChromaQuantisationParameters(isChromaCb);
	if (isLuam)
	{
		//亮度深度，偏移
		bitDepth = sHeader->sps.BitDepthY;
	}
	else
	{
		bitDepth = sHeader->sps.BitDepthC;
	}

	bool sMbFlag = false;
	//如果mb_type=si，或者为sp条带的帧内预测模式
	if (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI || (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP && isInterMode(macroblock[CurrMbAddr]->mode)))
	{
		sMbFlag = true;
	}

	//变量qP的推导如下

	int qP = 0;
	if (isLuam && !sMbFlag)
	{
		qP = macroblock[CurrMbAddr]->QP1Y;
	}
	else if (isLuam && sMbFlag)
	{
		////QSY 的值用于解码 mb_type 等SI 的SI条带的所有宏块以及预测模式为帧间的SP条带的所有宏块。
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



	//缩放过程
	if (macroblock[CurrMbAddr]->TransformBypassModeFlag)
	{
		memcpy(r, c, sizeof(int) * 16);
	}
	else
	{
		//8.5.12.1 Scaling process for residual 4x4 blocks 量化操作

		//量化是把连续的信号每隔一段取一个点，这样变成离散的了，才能在计算机里存储，
		//反量化把离散的数据变成连续的，这个距离取多长，不至于失真，可以参考香农采样定律
		int d[4][4] = { 0 };
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				if (i == 0 && j == 0
					&& ((isLuam && macroblock[CurrMbAddr]->mode == H264_MB_PART_PRED_MODE::Intra_16x16) || !isLuam)
					)
				{
					d[0][0] = c[0][0];
				}
				else
				{
					//量化参数QP是量化步长Qstep的序号。
					//对于亮度（Luma）编码而言，量化步长Qstep共有52个值，QP取值0~51，对于色度（Chroma）编码，Q的取值0~39。

					//FQ · Qstep
					if (qP >= 24)
					{
						//这里通过qp查表的方式获得qstep
						//这里-4是因为反量化系数推导过程那里多乘以了16,(2的四次方是16，所以-4)
						//这里qP / 6是计算有几倍，因为qp每增加6 qstep增加一倍，所以左移qp / 6
						d[i][j] = (c[i][j] * LevelScale4x4[qP % 6][i][j]) << (qP / 6 - 4);
					}
					else //if (qP < 24)
					{
						d[i][j] = (c[i][j] * LevelScale4x4[qP % 6][i][j] + static_cast<int>(pow(2, 3 - qP / 6))) >> (4 - qP / 6);
					}
				}
			}
		}

		//d[i][j]的范围应该在−2(7 + bitDepth) 和2(7 + bitDepth) − 1

		//dct变换
		int f[4][4] = { 0 };
		int h[4][4] = { 0 };
		//1 维反变换将缩放变换系数的每行（水平的）进行变换。
		for (size_t i = 0; i < 4; i++)
		{
			int ei0 = d[i][0] + d[i][2];
			int ei1 = d[i][0] - d[i][2];
			int ei2 = (d[i][1] >> 1) - d[i][3];
			int ei3 = d[i][1] + (d[i][3] >> 1);

			f[i][0] = ei0 + ei3;
			f[i][1] = ei1 + ei2;
			f[i][2] = ei1 - ei2;
			f[i][3] = ei0 - ei3;
		}

		//1 维反变换对得到的矩阵的每列（纵向）进行变换。
		for (size_t j = 0; j <= 3; j++)
		{
			int g0j = f[0][j] + f[2][j];
			int g1j = f[0][j] - f[2][j];
			int g2j = (f[1][j] >> 1) - f[3][j];
			int g3j = f[1][j] + (f[3][j] >> 1);

			h[0][j] = g0j + g3j;
			h[1][j] = g1j + g2j;
			h[2][j] = g1j - g2j;
			h[3][j] = g0j - g3j;
		}

		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				r[i][j] = (h[i][j] + 32) >> 6;
			}
		}
	}


}
//色度量化参数和缩放功能的推导过程
void ParseSlice::getChromaQuantisationParameters(bool isChromaCb)
{
	//CbCr
	int qPOffset = 0;


	if (isChromaCb)
	{
		//计算色度量化参数的偏移量值
		qPOffset = sHeader->pps.chroma_qp_index_offset;
	}
	else
	{
		qPOffset = sHeader->pps.second_chroma_qp_index_offset;
	}
	//每个色度分量的qPI 值通过下述方式获得  //QpBdOffsetC 色度偏移
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
		//用QSY 代替 QPY，QSC 代替 QPC
		macroblock[CurrMbAddr]->QSY = macroblock[CurrMbAddr]->QPY;
		macroblock[CurrMbAddr]->QSC = macroblock[CurrMbAddr]->QPC;
	}

}




void ParseSlice::Picture_construction_process_prior_to_deblocking_filter_process(int* u, const char* type, const size_t BlkIdx, const bool isLuam)
{
	int xP = InverseRasterScan(CurrMbAddr, 16, 16, sHeader->sps.PicWidthInSamplesL, 0);
	int yP = InverseRasterScan(CurrMbAddr, 16, 16, sHeader->sps.PicWidthInSamplesL, 1);
	int xO = 0;
	int yO = 0;
	if (isLuam)
	{
		int nE = 0;
		if (type == "16*16")
		{
			xO = yO = 0;
			nE = 16;
		}
		else if (type == "4*4")
		{
			xO = InverseRasterScan(BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(BlkIdx % 4, 4, 4, 8, 0);
			yO = InverseRasterScan(BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(BlkIdx % 4, 4, 4, 8, 1);
			nE = 4;
		}
		else
		{
			xO = InverseRasterScan(BlkIdx, 8, 8, 16, 0);
			yO = InverseRasterScan(BlkIdx, 8, 8, 16, 1);
			nE = 8;
		}

		//S′L[ xP + xO + j, yP + yO + i ] = uij    with i, j = 0..nE -  1
		for (size_t i = 0; i < nE - 1; i++)
		{
			for (size_t j = 0; j < nE - 1; j++)
			{

			}
		}
	}



}

//经过大量研究，权衡复杂度和压缩效率之后发现，HEVC的变换核应当满足如下条件：
//1.变换核中的系数都能用8bit表示（包括符号位）；2.变换核的第一个基础向量元（第一个行向量）应当都等于64。
//基于以上两个前提，HEVC中对DCT变换核进行了缩放：对每个变换核中的系数左移 6 + log2(N) / 2 bit并取整，
//最后再对个别系数进行一定的调整之后（调整的原则在于：使得调整后的变换核满足上部分提到的原则4 - 6，并在原则1 - 3之间找到一个最好的平衡点），得到不同尺寸块的变换核。
//————————————————
//原文链接：https ://blog.csdn.net/ftlisdcr/article/details/54345151
//反量化系数推导过程
void ParseSlice::scaling(bool isLuam, bool isChromaCb)
{


	//变量mbIsInterFlag的派生方法如下
	bool mbIsInterFlag = false;
	//帧间
	if (isInterframe(macroblock[CurrMbAddr]->mode))
	{
		mbIsInterFlag = true;
	}
	//通过下述方式得到变量iYCbCr
	int iYCbCr = 0;
	//separate_colour_plane_flag=1 分开编码
	if (sHeader->sps.separate_colour_plane_flag)
	{
		//colour_plane_id等于0、1和2分别对应于Y、Cb和Cr平面。
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
	//取哪一个scalinglist,在BP/MP/EP中，weightScale4x4是全为16的4x4矩阵
	//是否是帧间还是帧内，取对应量化缩放矩阵  <3=帧内，>3<=6对应的帧间   mbIsInterFlag ？3:0
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
	//是否是帧间还是帧内，取对应量化缩放矩阵  0=帧内，1=对应的帧间   mbIsInterFlag
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
