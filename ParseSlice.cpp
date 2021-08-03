#include "ParseSlice.h"



/// <summary>
/// ParseNalu& nalu  :nalu(nalu)
/// </summary>
ParseSlice::ParseSlice(ParseNalu& nalu, SliceHeader* sHeader) :nalu(nalu)
{
	mbX = 0;
	mbY = 0;

	macroblock = nullptr;
	lumaData = nullptr;
	chromaCbData = nullptr;
	chromaCrData = nullptr;
	memset(LevelScale4x4, 0, sizeof(int) * 6 * 4 * 4);
	memset(LevelScale8x8, 0, sizeof(int) * 6 * 8 * 8);
	CurrMbAddr = 0;
	this->sHeader = sHeader;

	PicWidthInSamplesL = sHeader->sps.PicWidthInSamplesL;
	PicHeightInSamplesL = sHeader->sps.PicHeightInSamplesL;
	PicWidthInSamplesC = sHeader->sps.PicWidthInSamplesC;
	PicHeightInSamplesC = sHeader->sps.PicHeightInSamplesC;
	PicSizeInMbs = sHeader->PicSizeInMbs;



	sliceNumber = 0;
	mbCount = 0;
}

bool ParseSlice::parse()
{


	chromaCbData = new uint8_t * [PicWidthInSamplesC];
	chromaCrData = new uint8_t * [PicWidthInSamplesC];

	for (size_t i = 0; i < PicWidthInSamplesC; i++)
	{
		chromaCbData[i] = new uint8_t[PicHeightInSamplesC]();
		chromaCrData[i] = new uint8_t[PicHeightInSamplesC]();
	}





	lumaData = new uint8_t * [PicWidthInSamplesL];
	for (size_t i = 0; i < PicWidthInSamplesL; i++)
	{
		lumaData[i] = new uint8_t[PicHeightInSamplesL]();
	}






	this->macroblock = new Macroblock * [PicSizeInMbs];

	for (size_t i = 0; i < PicSizeInMbs; i++)
	{
		macroblock[i] = new Macroblock;
	}

	return false;
}
void ParseSlice::free()
{

	if (chromaCbData)
	{
		for (size_t i = 0; i < PicWidthInSamplesC; i++)
		{
			if (chromaCbData[i])
			{
				delete[] chromaCbData[i];
				chromaCbData[i] = nullptr;
			}
		}
		delete[] chromaCbData;
		chromaCbData = nullptr;
	}

}
void ParseSlice::init()
{


}


ParseSlice::~ParseSlice()
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

	if (chromaCbData)
	{
		for (size_t i = 0; i < PicWidthInSamplesC; i++)
		{
			if (chromaCbData[i])
			{
				delete[] chromaCbData[i];
				chromaCbData[i] = nullptr;
			}
		}
		delete[] chromaCbData;
		chromaCbData = nullptr;
	}

	if (chromaCrData)
	{
		for (size_t i = 0; i < PicWidthInSamplesC; i++)
		{
			if (chromaCrData[i])
			{
				delete[] chromaCrData[i];
				chromaCrData[i] = nullptr;
			}
		}
		delete[] chromaCrData;
		chromaCrData = nullptr;
	}

}

//色度DC哈达玛变换 
void ParseSlice::transformDecodeChromaDCProcess(int c[4][2], int dcC[4][2], int MbWidthC, int MbHeightC, bool isChromaCb)
{

	//只有当ChromaArrayType等于1或2时才会调用这个过程。

	int BitDepth = sHeader->sps.BitDepthC;


	getChromaQuantisationParameters(isChromaCb);

	int qP = macroblock[CurrMbAddr]->QP1C;

	/*int MbWidthC = sHeader->sps.MbWidthC;
	int MbHeightC = sHeader->sps.MbHeightC;*/
	if (macroblock[CurrMbAddr]->TransformBypassModeFlag)
	{
		for (size_t i = 0; i < (MbWidthC / 4); i++)
		{
			for (size_t j = 0; j < (MbHeightC / 4); j++)
			{
				dcC[i][j] = c[i][j];
			}
		}
	}
	else
	{
		if (sHeader->sps.ChromaArrayType == 1)
		{



			int a[2][2] = {
				{1, 1},
				{1,-1}
			};

			int g[2][2] = { 0 };
			int f[2][2] = { 0 };

			for (size_t i = 0; i < 2; i++)
			{
				for (size_t j = 0; j < 2; j++)
				{
					for (size_t k = 0; k < 2; k++)
					{
						g[i][j] += a[i][k] * c[k][j];
					}
				}
			}

			for (size_t i = 0; i < 2; i++)
			{
				for (size_t j = 0; j < 2; j++)
				{
					for (size_t k = 0; k < 2; k++)
					{
						f[i][j] += g[i][k] * a[k][j];
					}
				}
			}

			for (size_t i = 0; i < 2; i++)
			{
				for (size_t j = 0; j < 2; j++)
				{
					dcC[i][j] = ((f[i][j] * LevelScale4x4[qP % 6][0][0]) << (qP / 6)) >> 5;
				}
			}

		}
		else if (sHeader->sps.ChromaArrayType == 2)
		{
			int a[4][4] = {
				{1,1,1,1},
				{1,1,-1,-1},
				{1,-1,-1,1},
				{1,-1,1,-1},

			};
			int b[2][2] = { {1,1},{1,-1} };

			int g[4][2] = { {0} };
			int f[4][2] = { {0} };
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 2; j++)
				{
					for (size_t k = 0; k < 4; k++)
					{
						g[i][j] += a[i][k] * c[k][j];
					}
				}
			}

			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 2; j++)
				{
					for (size_t k = 0; k < 2; k++)
					{
						f[i][j] += g[i][k] * b[k][j];
					}
				}
			}

			int qPDc = qP + 3;
			if (qPDc >= 36)
			{
				for (size_t i = 0; i < 4; i++)
				{
					for (size_t j = 0; j < 2; j++)
					{
						dcC[i][j] = (f[i][j] * LevelScale4x4[qPDc % 6][0][0]) << (qPDc / 6 - 6);
					}
				}
			}
			else
			{
				for (size_t i = 0; i < 4; i++)
				{
					for (size_t j = 0; j < 2; j++)
					{
						dcC[i][j] = (f[i][j] * LevelScale4x4[qPDc % 6][0][0] + static_cast<int>(pow(2, 5 - qPDc / 6))) >> (6 - qP / 6);
					}
				}
			}

		}
	}

}

void ParseSlice::Intra_4x4_prediction(size_t luma4x4BlkIdx, bool isLuam)
{
	//9种帧内4x4预测模式
	getIntra4x4PredMode(luma4x4BlkIdx, isLuam);

	int Intra4x4PredMode = macroblock[CurrMbAddr]->Intra4x4PredMode[luma4x4BlkIdx];



	//相对4*4块左上角像素块的位置，13个参考像素x和y的坐标
	//参考文档https://blog.csdn.net/shaqoneal/article/details/78820128


	constexpr int referenceCoordinateX[13] = { -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7 };
	constexpr int referenceCoordinateY[13] = { -1,  0,  1,  2,  3, -1, -1, -1, -1, -1, -1, -1, -1 };

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
		//int luma4x4BlkIdxN = 0;
		//xW, yW 当前子块距离mbAddrN左上角样点位置的距离
		getMbAddrNAndLuma4x4BlkIdxN(mbAddrN, xN, yN, maxW, maxH, xW, yW);

		//if (mbAddrN != NA)
		//{
		//	//参考像素所处宏块的子块索引
		//	luma4x4BlkIdxN = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
		//}

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
			//xM  yM mbAddrN宏块左上角样点位置到当前图像左上角的距离
			int xM = InverseRasterScan(mbAddrN, 16, 16, sHeader->sps.PicWidthInSamplesL, 0);
			int yM = InverseRasterScan(mbAddrN, 16, 16, sHeader->sps.PicWidthInSamplesL, 1);

			P(x, y) = lumaData[xM + xW][yM + yW];
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

void ParseSlice::Intra_8x8_prediction(size_t luma8x8BlkIdx, bool isLuam)
{

	constexpr int referenceCoordinateX[25] = { -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 };
	constexpr int referenceCoordinateY[25] = { -1,  0,  1,  2,  3,  4,  5,  6,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

	int p[9 * 17] = { 0 }; //x范围[-1,15]，y范围[-1,7]，共9行17列，原点为pp[1][1]

	memset(p, -1, sizeof(int) * 9 * 17);
#define P(x, y)      p[((y) + 1) * 17 + ((x) + 1)]

	const int xO = InverseRasterScan(luma8x8BlkIdx, 8, 8, 16, 0);
	const int yO = InverseRasterScan(luma8x8BlkIdx, 8, 8, 16, 1);

	for (size_t i = 0; i < 25; i++)
	{

		const int x = referenceCoordinateX[i];
		const int y = referenceCoordinateY[i];
		const int xN = xO + x;
		const int yN = yO + y;

		int maxW = 0;
		int maxH = 0;
		if (isLuam)
		{
			maxW = maxH = 16;
		}
		else
		{
			maxH = sHeader->sps.MbHeightC;
			maxW = sHeader->sps.MbWidthC;
		}
		int mbAddrN = NA;
		int xW = NA;
		int yW = NA;
		getMbAddrNAndLuma4x4BlkIdxN(mbAddrN, xN, yN, maxW, maxH, xW, yW);



	}


#undef P

}

void ParseSlice::Intra_16x16_prediction(bool isLuam)
{
	//相对于当前块左上角开始，x和y的坐标
	int referenceCoordinateX[33] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 };
	int referenceCoordinateY[33] = { -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };


	int p[17 * 17] = { -1 };
	memset(p, -1, sizeof(int) * 17 * 17);
#define P(x, y)    p[((y) + 1) * 17 + ((x) + 1)]
	for (size_t i = 0; i < 33; i++)
	{
		int maxW = 0;
		int maxH = 0;
		if (isLuam)
		{
			maxW = 16;
			maxH = 16;
		}
		else //if (isChroma == 1)
		{
			//色度块高度和宽度
			maxH = sHeader->sps.MbHeightC;
			maxW = sHeader->sps.MbWidthC;
		}


		int x = referenceCoordinateX[i];
		int y = referenceCoordinateY[i];
		int mbAddrN = NA;
		int xW = NA;
		int yW = NA;
		//int luma4x4BlkIdxN = 0;

		//xW, yW 当前子块距离mbAddrN左上角样点位置的距离
		getMbAddrNAndLuma4x4BlkIdxN(mbAddrN, x, y, maxW, maxH, xW, yW);


		if (mbAddrN == NA
			|| (isInterframe(macroblock[mbAddrN]->mode) && sHeader->pps.constrained_intra_pred_flag)
			|| (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI && sHeader->pps.constrained_intra_pred_flag)
			)
		{
			P(x, y) = -1;
		}
		else
		{
			//xM  yM mbAddrN宏块左上角样点位置到当前图像左上角的距离
			int xM = InverseRasterScan(mbAddrN, 16, 16, sHeader->sps.PicWidthInSamplesL, 0);
			int yM = InverseRasterScan(mbAddrN, 16, 16, sHeader->sps.PicWidthInSamplesL, 1);

			P(x, y) = lumaData[xM + xW][yM + yW];
		}
	}




	uint8_t Intra16x16PredMode = macroblock[CurrMbAddr]->Intra16x16PredMode;
	if (Intra16x16PredMode == 0)//垂直
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(8, -1) >= 0 && P(9, -1) >= 0 && P(10, -1) >= 0 && P(11, -1) >= 0 && P(12, -1) >= 0 && P(13, -1) >= 0 && P(14, -1) >= 0 && P(15, -1) >= 0
			)
		{
			for (size_t y = 0; y < 16; y++)
			{
				for (size_t x = 0; x < 16; x++)
				{
					macroblock[CurrMbAddr]->luma16x16PredSamples[x][y] = P(x, -1);
				}
			}
		}
	}
	else if (Intra16x16PredMode == 1)//水平
	{
		if (P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0
			&& P(-1, 8) >= 0 && P(-1, 9) >= 0 && P(-1, 10) >= 0 && P(-1, 11) >= 0 && P(-1, 12) >= 0 && P(-1, 13) >= 0 && P(-1, 14) >= 0 && P(-1, 15) >= 0
			)
		{
			for (size_t y = 0; y < 16; y++)
			{
				for (size_t x = 0; x < 16; x++)
				{
					macroblock[CurrMbAddr]->luma16x16PredSamples[x][y] = P(-1, y);
				}
			}
		}
	}
	else if (Intra16x16PredMode == 2)
	{
		//四舍五入求均值
		int val = 0;

		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(8, -1) >= 0 && P(9, -1) >= 0 && P(10, -1) >= 0 && P(11, -1) >= 0 && P(12, -1) >= 0 && P(13, -1) >= 0 && P(14, -1) >= 0 && P(15, -1) >= 0
			&& P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0
			&& P(-1, 8) >= 0 && P(-1, 9) >= 0 && P(-1, 10) >= 0 && P(-1, 11) >= 0 && P(-1, 12) >= 0 && P(-1, 13) >= 0 && P(-1, 14) >= 0 && P(-1, 15) >= 0
			)
		{
			val = (P(0, -1) + P(1, -1) + P(2, -1) + P(3, -1) + P(4, -1) + P(5, -1) + P(6, -1) + P(7, -1)
				+ P(8, -1) + P(9, -1) + P(10, -1) + P(11, -1) + P(12, -1) + P(13, -1) + P(14, -1) + P(15, -1)
				+ P(-1, 0) + P(-1, 1) + P(-1, 2) + P(-1, 3) + P(-1, 4) + P(-1, 5) + P(-1, 6) + P(-1, 7)
				+ P(-1, 8) + P(-1, 9) + P(-1, 10) + P(-1, 11) + P(-1, 12) + P(-1, 13) + P(-1, 14) + P(-1, 15) + 16) >> 5;
		}
		else if (!(P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(8, -1) >= 0 && P(9, -1) >= 0 && P(10, -1) >= 0 && P(11, -1) >= 0 && P(12, -1) >= 0 && P(13, -1) >= 0 && P(14, -1) >= 0 && P(15, -1) >= 0)
			&& (P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0
				&& P(-1, 8) >= 0 && P(-1, 9) >= 0 && P(-1, 10) >= 0 && P(-1, 11) >= 0 && P(-1, 12) >= 0 && P(-1, 13) >= 0 && P(-1, 14) >= 0 && P(-1, 15) >= 0)
			)
		{
			val = (P(-1, 0) + P(-1, 1) + P(-1, 2) + P(-1, 3) + P(-1, 4) + P(-1, 5) + P(-1, 6) + P(-1, 7)
				+ P(-1, 8) + P(-1, 9) + P(-1, 10) + P(-1, 11) + P(-1, 12) + P(-1, 13) + P(-1, 14) + P(-1, 15) + 8) >> 4;
		}
		else if ((P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(8, -1) >= 0 && P(9, -1) >= 0 && P(10, -1) >= 0 && P(11, -1) >= 0 && P(12, -1) >= 0 && P(13, -1) >= 0 && P(14, -1) >= 0 && P(15, -1) >= 0)
			&& !(P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0
				&& P(-1, 8) >= 0 && P(-1, 9) >= 0 && P(-1, 10) >= 0 && P(-1, 11) >= 0 && P(-1, 12) >= 0 && P(-1, 13) >= 0 && P(-1, 14) >= 0 && P(-1, 15) >= 0)
			)
		{
			val = (P(0, -1) + P(1, -1) + P(2, -1) + P(3, -1) + P(4, -1) + P(5, -1) + P(6, -1) + P(7, -1)
				+ P(8, -1) + P(9, -1) + P(10, -1) + P(11, -1) + P(12, -1) + P(13, -1) + P(14, -1) + P(15, -1) + 8) >> 4;
		}
		else
		{
			//h264一个亮度像素都是8个bit
			val = 1 << (sHeader->sps.BitDepthY - 1); //128 在h264
		}


		for (size_t x = 0; x < 16; x++)
		{
			for (size_t y = 0; y < 16; y++)
			{
				macroblock[CurrMbAddr]->luma16x16PredSamples[x][y] = val;
			}
		}
	}
	else if (Intra16x16PredMode == 3)
	{
		//由x，y，z轴三个点连接的面，并延伸投影到二维面上，对应的矩阵就是预测值

		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(8, -1) >= 0 && P(9, -1) >= 0 && P(10, -1) >= 0 && P(11, -1) >= 0 && P(12, -1) >= 0 && P(13, -1) >= 0 && P(14, -1) >= 0 && P(15, -1) >= 0
			&& P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0
			&& P(-1, 8) >= 0 && P(-1, 9) >= 0 && P(-1, 10) >= 0 && P(-1, 11) >= 0 && P(-1, 12) >= 0 && P(-1, 13) >= 0 && P(-1, 14) >= 0 && P(-1, 15) >= 0
			)
		{
			int32_t H = 0;
			int32_t V = 0;

			for (int32_t x = 0; x <= 7; x++)
			{
				H += (x + 1) * (P(8 + x, -1) - P(6 - x, -1));
			}

			for (int32_t y = 0; y <= 7; y++)
			{
				V += (y + 1) * (P(-1, 8 + y) - P(-1, 6 - y));
			}

			int32_t a = 16 * (P(-1, 15) + P(15, -1));
			int32_t b = (5 * H + 32) >> 6;
			int32_t c = (5 * V + 32) >> 6;

			for (int32_t y = 0; y < 16; y++)
			{
				for (int32_t x = 0; x < 16; x++)
				{
					macroblock[CurrMbAddr]->luma16x16PredSamples[x][y] = Clip3(0, (1 << sHeader->sps.BitDepthY) - 1, (a + b * (x - 7) + c * (y - 7) + 16) >> 5);
				}
			}
		}

	}


#undef P;


}
void ParseSlice::Intra_chroma_prediction(bool isChromaCb)
{
	if (sHeader->sps.ChromaArrayType == 3)
	{

	}
	else // 420, 422
	{
		const int MbWidthC = sHeader->sps.MbWidthC;
		const int MbHeightC = sHeader->sps.MbHeightC;

		const int maxSamplesVal = MbWidthC + MbHeightC + 1;
		int* referenceCoordinateX = new int[maxSamplesVal]();
		int* referenceCoordinateY = new int[maxSamplesVal]();

		for (int i = -1; i < MbHeightC; i++)
		{
			referenceCoordinateX[i + 1] = -1;
			referenceCoordinateY[i + 1] = i;
		}

		for (int i = 0; i < MbWidthC; i++)
		{
			referenceCoordinateX[(MbHeightC + 1 + i)] = i;
			referenceCoordinateY[(MbHeightC + 1 + i)] = -1;;
		}
		//最大有17行，9列(422左边有16个像素点，上面有8个像素点，左上角一个)


		int* samples = new int[(MbWidthC + 1) * (MbHeightC + 1)];


		memset(samples, -1, sizeof(int) * (MbWidthC + 1) * (MbHeightC + 1));

#define P(x, y)   samples[((y) + 1) * (MbHeightC + 1) + ((x) + 1)]



		for (size_t i = 0; i < maxSamplesVal; i++)
		{
			const int x = referenceCoordinateX[i];
			const int y = referenceCoordinateY[i];


			int mbAddrN = NA;
			int xW = NA;
			int yW = NA;
			//int luma4x4BlkIdxN = 0;
			//xW, yW 当前子块距离mbAddrN左上角样点位置的距离
			getMbAddrNAndLuma4x4BlkIdxN(mbAddrN, x, y, MbWidthC, MbHeightC, xW, yW);


			if (mbAddrN == NA
				|| (isInterframe(macroblock[mbAddrN]->mode) && sHeader->pps.constrained_intra_pred_flag)
				|| (macroblock[mbAddrN]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI && sHeader->pps.constrained_intra_pred_flag && macroblock[CurrMbAddr]->fix_slice_type != SLIECETYPE::H264_SLIECE_TYPE_SI)
				)
			{
				P(x, y) = -1;
			}
			else
			{
				//xL  yL mbAddrN宏块左上角样点位置到当前图像左上角的距离
				int xL = InverseRasterScan(mbAddrN, 16, 16, sHeader->sps.PicWidthInSamplesL, 0);
				int yL = InverseRasterScan(mbAddrN, 16, 16, sHeader->sps.PicWidthInSamplesL, 1);

				//macroblock mbAddr左上角色度样本的位置(xM, yM)
				int xM = (xL >> 4) * MbWidthC;
				int	yM = ((yL >> 4) * MbHeightC) + (yL % 2);

				if (isChromaCb)
				{
					P(x, y) = chromaCbData[xM + xW][yM + yW];
				}
				else
				{
					P(x, y) = chromaCrData[xM + xW][yM + yW];
				}


			}
		}


		int IntraChromaPredMode = macroblock[CurrMbAddr]->intra_chroma_pred_mode;

		if (IntraChromaPredMode == 0) //DC
		{
			for (size_t chroma4x4BlkIdx = 0; chroma4x4BlkIdx < (1 << (sHeader->sps.ChromaArrayType + 1)); chroma4x4BlkIdx++)
			{
				//相对于宏块的左上色度样本的索引为chroma4x4BlkIdx的4x4色度块的左上色度样本的位置(x, y)。
				const int xO = InverseRasterScan(chroma4x4BlkIdx, 4, 4, 8, 0);
				const int yO = InverseRasterScan(chroma4x4BlkIdx, 4, 4, 8, 1);


				int val = 0;
				if ((xO == 0 && yO == 0) || (xO > 0 && yO > 0))
				{
					if (P(0 + xO, -1) >= 0 && P(1 + xO, -1) >= 0 && P(2 + xO, -1) >= 0 && P(3 + xO, -1) >= 0
						&& P(-1, 0 + yO) >= 0 && P(-1, 1 + yO) >= 0 && P(-1, 2 + yO) >= 0 && P(-1, 3 + yO) >= 0
						)
					{
						val = (P(0 + xO, -1) + P(1 + xO, -1) + P(2 + xO, -1) + P(3 + xO, -1)
							+ P(-1, 0 + yO) + P(-1, 1 + yO) + P(-1, 2 + yO) + P(-1, 3 + yO) + 4) >> 3;
					}
					//(P(0, -1) < 0 || P(1, -1) < 0 || P(2, -1) < 0 || P(3, -1) < 0)
					else if (!(P(0 + xO, -1) >= 0 && P(1 + xO, -1) >= 0 && P(2 + xO, -1) >= 0 && P(3 + xO, -1) >= 0)
						&& (P(-1, 0 + yO) >= 0 && P(-1, 1 + yO) >= 0 && P(-1, 2 + yO) >= 0 && P(-1, 3 + yO) >= 0)
						)
					{
						val = (P(-1, 0 + yO) + P(-1, 1 + yO) + P(-1, 2 + yO) + P(-1, 3 + yO) + 2) >> 2;
					}
					else if ((P(0 + xO, -1) > 0 && P(1 + xO, -1) > 0 && P(2 + xO, -1) > 0 && P(3 + xO, -1) > 0)
						&& !(P(-1, 0 + yO) > 0 && P(-1, 1 + yO) > 0 && P(-1, 2 + yO) > 0 && P(-1, 3 + yO) > 0)
						)
					{
						val = (P(0 + xO, -1) + P(1 + xO, -1) + P(2 + xO, -1) + P(3 + xO, -1) + 2) >> 2;
					}
					else //some samples p[ x + xO, −1 ], with x = 0..3, and some samples p[ −1, y +yO ], with y = 0..3, are marked as "not available for Intra chroma prediction"
					{
						val = (1 << (sHeader->sps.BitDepthC - 1));
					}
				}
				else if (xO > 0 && yO == 0)
				{
					if (P(0 + xO, -1) >= 0 && P(1 + xO, -1) >= 0 && P(2 + xO, -1) >= 0 && P(3 + xO, -1) >= 0)
					{
						val = (P(0 + xO, -1) + P(1 + xO, -1) + P(2 + xO, -1) + P(3 + xO, -1) + 2) >> 2;
					}
					else if (P(-1, 0 + yO) >= 0 && P(-1, 1 + yO) >= 0 && P(-1, 2 + yO) >= 0 && P(-1, 3 + yO) > 0)
					{
						val = (P(-1, 0 + yO) + P(-1, 1 + yO) + P(-1, 2 + yO) + P(-1, 3 + yO) + 2) >> 2;
					}
					else //some samples p[ x + xO, −1 ], with x = 0..3, and some samples p[ −1, y +yO ], with y = 0..3, are marked as "not available for Intra chroma prediction"
					{
						val = (1 << (sHeader->sps.BitDepthC - 1));
					}
				}
				else if (xO == 0 && yO > 0)
				{
					if (P(-1, 0 + yO) >= 0 && P(-1, 1 + yO) >= 0 && P(-1, 2 + yO) >= 0 && P(-1, 3 + yO) > 0)
					{
						val = (P(-1, 0 + yO) + P(-1, 1 + yO) + P(-1, 2 + yO) + P(-1, 3 + yO) + 2) >> 2;
					}
					else if (P(0 + xO, -1) >= 0 && P(1 + xO, -1) >= 0 && P(2 + xO, -1) >= 0 && P(3 + xO, -1) > 0)
					{
						val = (P(0 + xO, -1) + P(1 + xO, -1) + P(2 + xO, -1) + P(3 + xO, -1) + 2) >> 2;
					}
					else //some samples p[ x + xO, −1 ], with x = 0..3, and some samples p[ −1, y +yO ], with y = 0..3, are marked as "not available for Intra chroma prediction"
					{
						val = (1 << (sHeader->sps.BitDepthC - 1));
					}
				}


				for (size_t y = 0; y < 4; y++)
				{
					for (size_t x = 0; x < 4; x++)
					{
						macroblock[CurrMbAddr]->chromaPredSamples[x + xO][y + yO] = val;
					}
				}
				int a = 1;
			}
			int a = 1;
		}
		else if (IntraChromaPredMode == 1)
		{
			bool flag = true;

			for (size_t y = 0; y < MbHeightC; y++)
			{
				if (P(-1, y) < 0) {
					flag = false;
					break;
				}
			}

			if (flag)
			{
				for (size_t y = 0; y < MbHeightC; y++)
				{
					for (size_t x = 0; x < MbWidthC; x++)
					{
						macroblock[CurrMbAddr]->chromaPredSamples[x][y] = P(-1, y);
					}
				}
			}
		}
		else if (IntraChromaPredMode == 2)
		{
			bool flag = true;
			for (size_t x = 0; x < MbWidthC; x++)
			{
				if (P(x, -1) < 0)
				{
					flag = false;
					break;
				}
			}
			if (flag)
			{
				for (size_t y = 0; y < MbHeightC; y++)
				{
					for (size_t x = 0; x < MbWidthC; x++)
					{
						macroblock[CurrMbAddr]->chromaPredSamples[x][y] = P(x, -1);
					}
				}
			}
		}
		else if (IntraChromaPredMode == 3)
		{
			int flag = true;
			for (size_t x = 0; x < MbWidthC; x++)
			{
				if (P(x, -1) < 0)
				{
					flag = false;
					break;
				}
			}

			for (int y = -1; y < MbHeightC; y++)
			{
				if (P(-1, y) < 0)
				{
					flag = false;
					break;
				}
			}

			//This mode shall be used only when the samples p[ x, −1 ], with x = 0..MbWidthC − 1 and p[ −1, y ], with y = −1..MbHeightC − 1 are marked as "available for Intra chroma prediction".
			if (flag)
			{
				int xCF = ((sHeader->sps.ChromaArrayType == 3) ? 4 : 0);
				int yCF = ((sHeader->sps.ChromaArrayType != 1) ? 4 : 0);

				int H = 0;
				int V = 0;

				for (int x1 = 0; x1 <= 3 + xCF; x1++)
				{
					H += (x1 + 1) * (P(4 + xCF + x1, -1) - P(2 + xCF - x1, -1));
				}

				for (int y1 = 0; y1 <= 3 + yCF; y1++)
				{
					V += (y1 + 1) * (P(-1, 4 + yCF + y1) - P(-1, 2 + yCF - y1));
				}

				int a = 16 * (P(-1, MbHeightC - 1) + P(MbWidthC - 1, -1));
				int b = ((34 - 29 * (sHeader->sps.ChromaArrayType == 3)) * H + 32) >> 6;
				int c = ((34 - 29 * (sHeader->sps.ChromaArrayType != 1)) * V + 32) >> 6;

				for (int y = 0; y < MbHeightC; y++)
				{
					for (int x = 0; x < MbWidthC; x++)
					{
						macroblock[CurrMbAddr]->chromaPredSamples[x][y] = Clip3(0, (1 << sHeader->sps.BitDepthC) - 1, (a + b * (x - 3 - xCF) + c * (y - 3 - yCF) + 16) >> 5);
					}
				}
			}
		}




		delete[] referenceCoordinateX;
		delete[] referenceCoordinateY;
		delete[] samples;
#undef P

	}


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
	/*
	*   0,-1
	   ---------------
  -1,0 |0,0
	   |
	   |
	   |
	*/


	int xW = NA;
	int yW = NA;

	//亮度位置的差分值 表6-2（ xD, yD ）
	//当前子块距离mbAddrN左上角样点距离（ xW, yW)
	getMbAddrNAndLuma4x4BlkIdxN(mbAddrA, x + (-1), y + 0, maxW, maxH, xW, yW);

	if (mbAddrA != NA)
	{
		//左侧宏块子块索引
		luma4x4BlkIdxA = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
	}

	getMbAddrNAndLuma4x4BlkIdxN(mbAddrB, x + 0, y + (-1), maxW, maxH, xW, yW);
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
	int& mbAddrN, const int xN, const int yN, const int maxW, const int maxH, int& xW, int& yW
)
{

#define isMbUsable(mbAddr, CurrMbAddr) (mbAddr < 0 || mbAddr > CurrMbAddr || macroblock[CurrMbAddr]->sliceNumber != macroblock[mbAddr]->sliceNumber)


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

#undef isMbUsable

}


//4*4亮度残差
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

			//对近似均匀分布的语法元素，在编码和解码时选择旁路（bypass）模式，可以免除上下文建模，提高编解码的速度,不经过量化和dct。
			if (macroblock[CurrMbAddr]->TransformBypassModeFlag
				&& macroblock[CurrMbAddr]->mode == H264_MB_PART_PRED_MODE::Intra_4x4
				&& (macroblock[CurrMbAddr]->Intra4x4PredMode[luma4x4BlkIdx] == 0 || macroblock[CurrMbAddr]->Intra4x4PredMode[luma4x4BlkIdx] == 1)
				)
			{
				printError("不支持旁路变换解码");
				exit(0);
			}


			//4*4预测过程
			Intra_4x4_prediction(luma4x4BlkIdx, true);

			/*macroblock[CurrMbAddr]->lumaPredSamples;

			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 4; y++)
				{
					macroblock[CurrMbAddr]->predL[xO + x][yO + y] = macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][x][y];
				}
			}*/


			int u[16] = { 0 };
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					//macroblock[CurrMbAddr]->predL[xO + j][yO + i]
					//Clip1Y( x ) = Clip3( 0， ( 1 << BitDepthY ) − 1 ， x ) 
					u[i * 4 + j] = Clip3(0, (1 << sHeader->sps.BitDepthY) - 1, macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][j][i] + r[i][j]);
				}
			}

			////环路滤波器之前的图像生成
			Picture_construction_process_prior_to_deblocking_filter_process(u, "4*4", luma4x4BlkIdx, true);
		}


	}


}

//8*8
void ParseSlice::transformDecode8x8LuamResidualProcess()
{
	scaling(true, false);

	for (size_t luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++)
	{
		int c[8][8] = { 0 };
		//逆zigzag扫描
		inverseScanner8x8Process(macroblock[CurrMbAddr]->level8x8[luma8x8BlkIdx], c);
		int r[8][8] = { 0 };
		Scaling_and_transformation_process_for_residual_8x8_blocks(c, r, true, false);


		//对近似均匀分布的语法元素，在编码和解码时选择旁路（bypass）模式，可以免除上下文建模，提高编解码的速度,不经过量化和dct。
		if (macroblock[CurrMbAddr]->TransformBypassModeFlag
			&& macroblock[CurrMbAddr]->mode == H264_MB_PART_PRED_MODE::Intra_8x8
			&& (macroblock[CurrMbAddr]->Intra4x4PredMode[luma8x8BlkIdx] == 0 || macroblock[CurrMbAddr]->Intra4x4PredMode[luma8x8BlkIdx] == 1)
			)
		{
			printError("不支持旁路变换解码");
			exit(0);
		}

		int xO = InverseRasterScan(luma8x8BlkIdx, 8, 8, 16, 0);
		int yO = InverseRasterScan(luma8x8BlkIdx, 8, 8, 16, 1);
	}

}

//16*16亮度残差
void ParseSlice::transformDecode16x16LuamResidualProcess(const int i16x16DClevel[16], const int i16x16AClevel[16][16], bool isLuam, bool isChromaCb)
{


	scaling(isLuam, isChromaCb);
	//对宏块中所有4x4 luma块的4x4 luma直流变换系数进行解码。


	int c[4][4] = { 0 };

	inverseScanner4x4Process(i16x16DClevel, c);

	//Intra_16x16宏块类型的DC变换系数的缩放和变换过程
	int dcY[4][4] = { 0 };
	//最难压缩的直流用哈达玛变换
	//因为一个宏块里的直流系数一般都相等或者相似的，哈达玛可以归并同类项
	//哈达玛可以进一步压缩直流分量
	//先进行Hadamard变换
	transformDecodeIntra_16x16DCProcess(c, dcY, isLuam, isChromaCb);


	int rMb[16][16] = { 0 };


	//dcY到luma4x4BlkIdx的索引对应的分配
	int dcYToLuma[16] = {
		dcY[0][0], dcY[0][1], dcY[1][0], dcY[1][1],
		dcY[0][2], dcY[0][3], dcY[1][2], dcY[1][3],
		dcY[2][0], dcY[2][1], dcY[3][0], dcY[3][1],
		dcY[2][2], dcY[2][3], dcY[3][2], dcY[3][3]
	};


	//luma4x4BlkIdx or cb4x4BlkIdx or cr4x4BlkIdx
	for (size_t _4x4BlkIdx = 0; _4x4BlkIdx < 16; _4x4BlkIdx++)
	{
		int lumaList[16] = { 0 };
		lumaList[0] = dcYToLuma[_4x4BlkIdx];  //DC系数

		for (size_t k = 1; k < 16; k++)
		{
			lumaList[k] = i16x16AClevel[_4x4BlkIdx][k - 1];//AC系数
		}

		int c[4][4] = { 0 };
		inverseScanner4x4Process(lumaList, c);

		int r[4][4] = { 0 };
		scalingTransformProcess(c, r, isLuam, false);

		int xO = InverseRasterScan(_4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(_4x4BlkIdx % 4, 4, 4, 8, 0);
		int yO = InverseRasterScan(_4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(_4x4BlkIdx % 4, 4, 4, 8, 1);


		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				rMb[xO + j][yO + i] = r[i][j];
			}
		}
	}

	if (macroblock[CurrMbAddr]->TransformBypassModeFlag && (macroblock[CurrMbAddr]->Intra16x16PredMode == 0 || macroblock[CurrMbAddr]->Intra16x16PredMode == 1))
	{
		printError("旁路变换过程");
		exit(0);
	}

	Intra_16x16_prediction(true);

	int u[16 * 16] = { 0 };

	for (size_t i = 0; i < 16; i++)
	{
		for (size_t j = 0; j < 16; j++)
		{
			u[i * 16 + j] = Clip3(0, (1 << sHeader->sps.BitDepthY) - 1, macroblock[CurrMbAddr]->luma16x16PredSamples[j][i] + rMb[j][i]);
		}
	}
	Picture_construction_process_prior_to_deblocking_filter_process(u, "16*16", 0, true);

}

//色度解码
void ParseSlice::transformDecodeChromaResidualProcess(bool isChromaCb)
{
	if (sHeader->sps.ChromaArrayType == 0)
	{
		printError("ChromaArrayType==0");
		exit(0);
	}


	if (sHeader->sps.ChromaArrayType == 3)
	{
		//对ChromaArrayType等于3 444 的色度样本的转换解码过程的规范
		transformDecodeChromaArrayTypeEqualTo3Process(isChromaCb);
	}
	else
	{

		int MbWidthC = sHeader->sps.MbWidthC;
		int MbHeightC = sHeader->sps.MbHeightC;
		//420 8 8
		int numChroma4x4Blks = (MbWidthC / 4) * (MbHeightC / 4);
		int iCbCr = (isChromaCb) ? 0 : 1;



		int dcC[4][2] = { 0 };
		if (sHeader->sps.ChromaArrayType == 1) //420
		{
			int c[2][2] = { 0 };

			//420最大DC系数有4个
			c[0][0] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][0];
			c[0][1] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][1];
			c[1][0] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][2];
			c[1][1] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][3];
			transformDecodeChromaDCProcess(c, dcC, MbWidthC, MbHeightC, isChromaCb);
		}
		else if (sHeader->sps.ChromaArrayType == 2)  //422
		{
			//422最大DC系数有8个
			/*
			|0,1|
			|2,3|
			|4,5|
			|6,7|
			*/
			int c[4][2] = { 0 };
			c[0][0] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][0];
			c[0][1] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][1];
			c[1][0] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][2];
			c[1][1] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][3];
			c[2][0] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][4];
			c[2][1] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][5];
			c[3][0] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][6];
			c[3][1] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][7];
			transformDecodeChromaDCProcess(c, dcC, MbWidthC, MbHeightC, isChromaCb);
		}

		int dcCToChroma[8] =
		{
			dcC[0][0], dcC[0][1],
			dcC[1][0], dcC[1][1],
			dcC[2][0], dcC[2][1],
			dcC[3][0], dcC[3][1],
		};

		int rMb[8][16] = { 0 };//本应该是rMb[MbWidthC][MbHeightC], 此处按最大的8x16(422)尺寸来申请数组,

		for (size_t chroma4x4BlkIdx = 0; chroma4x4BlkIdx < numChroma4x4Blks; chroma4x4BlkIdx++)
		{

			int chromaList[16] = { 0 };

			chromaList[0] = dcCToChroma[chroma4x4BlkIdx];

			for (size_t k = 1; k < 16; k++)
			{
				chromaList[k] = macroblock[CurrMbAddr]->ChromaACLevel[iCbCr][chroma4x4BlkIdx][k - 1];
			}

			int c[4][4] = { 0 };
			//422 最大8
			inverseScanner4x4Process(chromaList, c);

			int r[4][4] = { 0 };
			scalingTransformProcess(c, r, false, isChromaCb);


			int xO = InverseRasterScan(chroma4x4BlkIdx, 4, 4, 8, 0);
			int yO = InverseRasterScan(chroma4x4BlkIdx, 4, 4, 8, 1);

			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					rMb[xO + j][yO + i] = r[i][j];
				}
			}
		}


		if (macroblock[CurrMbAddr]->TransformBypassModeFlag)
		{
			printError("不支持旁路变换");
			exit(0);
		}
		Intra_chroma_prediction(isChromaCb);


		int* u = new int[MbWidthC * MbHeightC];
		for (size_t i = 0; i < MbWidthC; i++)
		{
			for (size_t j = 0; j < MbHeightC; j++)
			{
				u[i * MbWidthC + j] = Clip3(0, (1 << sHeader->sps.BitDepthC) - 1, macroblock[CurrMbAddr]->chromaPredSamples[j][i] + rMb[j][i]);
			}
		}

		Picture_construction_process_prior_to_deblocking_filter_process(u, "4*4", 0, false, isChromaCb);

		delete[] u;
		u = nullptr;

	}

}







void ParseSlice::inverseScanner4x4Process(const int value[16], int c[4][4])
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
	if (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI
		|| (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP && isInterframe(macroblock[CurrMbAddr]->mode)))
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
	//这里表示要旁路变换，不用经过量化dct变换过程
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
						d[i][j] = (c[i][j] * LevelScale4x4[qP % 6][i][j] + static_cast<int>(std::pow(2, 3 - qP / 6))) >> (4 - qP / 6);
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
//用于残差8x8块的缩放和变换过程
void ParseSlice::Scaling_and_transformation_process_for_residual_8x8_blocks(int c[8][8], int r[8][8], bool isLuam, bool isChromaCb)
{
	getChromaQuantisationParameters(isChromaCb);

	int bitDepth = 0;
	int qP = 0;
	if (isLuam)
	{
		bitDepth = sHeader->sps.BitDepthY;
		qP = macroblock[CurrMbAddr]->QP1Y;
	}
	else //色度
	{
		bitDepth = sHeader->sps.BitDepthC;
		qP = macroblock[CurrMbAddr]->QP1C;
	}
	//这里表示要旁路变换，不用经过量化dct变换过程
	if (macroblock[CurrMbAddr]->TransformBypassModeFlag)
	{
		memcpy(r, c, sizeof(int) * 16);
	}
	else
	{
		//8.5.13.1 Scaling process for residual 8x8 blocks
		int d[8][8] = { 0 };

		for (int32_t i = 0; i < 8; i++)
		{
			for (int32_t j = 0; j < 8; j++)
			{
				if (qP >= 36)
				{
					//这里多了个weightScale4x4，即多乘上了16,故在后面向右移了4位，即将原来书上的">>(QP/6-2)"变成了">>(QP/6-6）
					d[i][j] = (c[i][j] * LevelScale8x8[qP % 6][i][j]) << (qP / 6 - 6);
				}
				else //if (qP < 36)
				{
					d[i][j] = (c[i][j] * LevelScale8x8[qP % 6][i][j] + static_cast<int>(std::pow(2, 5 - qP / 6))) >> (6 - qP / 6);
				}
			}
		}


		//类似4x4 IDC离散余弦反变换蝶形运算

		int g[8][8];
		int m[8][8];

		for (int i = 0; i < 8; i++) //先行变换
		{
			int ei0 = d[i][0] + d[i][4];
			int ei1 = -d[i][3] + d[i][5] - d[i][7] - (d[i][7] >> 1);
			int ei2 = d[i][0] - d[i][4];
			int ei3 = d[i][1] + d[i][7] - d[i][3] - (d[i][3] >> 1);
			int ei4 = (d[i][2] >> 1) - d[i][6];
			int ei5 = -d[i][1] + d[i][7] + d[i][5] + (d[i][5] >> 1);
			int ei6 = d[i][2] + (d[i][6] >> 1);
			int ei7 = d[i][3] + d[i][5] + d[i][1] + (d[i][1] >> 1);

			int fi0 = ei0 + ei6;
			int fi1 = ei1 + (ei7 >> 2);
			int fi2 = ei2 + ei4;
			int fi3 = ei3 + (ei5 >> 2);
			int fi4 = ei2 - ei4;
			int fi5 = (ei3 >> 2) - ei5;
			int fi6 = ei0 - ei6;
			int fi7 = ei7 - (ei1 >> 2);

			g[i][0] = fi0 + fi7;
			g[i][1] = fi2 + fi5;
			g[i][2] = fi4 + fi3;
			g[i][3] = fi6 + fi1;
			g[i][4] = fi6 - fi1;
			g[i][5] = fi4 - fi3;
			g[i][6] = fi2 - fi5;
			g[i][7] = fi0 - fi7;
		}

		for (int j = 0; j < 8; j++) //再列变换
		{
			int h0j = g[0][j] + g[4][j];
			int h1j = -g[3][j] + g[5][j] - g[7][j] - (g[7][j] >> 1);
			int h2j = g[0][j] - g[4][j];
			int h3j = g[1][j] + g[7][j] - g[3][j] - (g[3][j] >> 1);
			int h4j = (g[2][j] >> 1) - g[6][j];
			int h5j = -g[1][j] + g[7][j] + g[5][j] + (g[5][j] >> 1);
			int h6j = g[2][j] + (g[6][j] >> 1);
			int h7j = g[3][j] + g[5][j] + g[1][j] + (g[1][j] >> 1);

			int k0j = h0j + h6j;
			int k1j = h1j + (h7j >> 2);
			int k2j = h2j + h4j;
			int k3j = h3j + (h5j >> 2);
			int k4j = h2j - h4j;
			int k5j = (h3j >> 2) - h5j;
			int k6j = h0j - h6j;
			int k7j = h7j - (h1j >> 2);

			m[0][j] = k0j + k7j;
			m[1][j] = k2j + k5j;
			m[2][j] = k4j + k3j;
			m[3][j] = k6j + k1j;
			m[4][j] = k6j - k1j;
			m[5][j] = k4j - k3j;
			m[6][j] = k2j - k5j;
			m[7][j] = k0j - k7j;
		}

		//------------------------------------
		for (int i = 0; i <= 7; i++)
		{
			for (int j = 0; j <= 7; j++)
			{
				r[i][j] = (m[i][j] + 32) >> 6;
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

	//QPC:各色度分量Cb和Cr的色度量化参数，
	macroblock[CurrMbAddr]->QPC = QPC;
	macroblock[CurrMbAddr]->QP1C = QP1C;



	if ((SLIECETYPE)sHeader->slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP || (SLIECETYPE)sHeader->slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI)
	{
		//用QSY 代替 QPY，QSC 代替 QPC

		macroblock[CurrMbAddr]->QSY = macroblock[CurrMbAddr]->QPY;
		//QSY::解码SP和SI片所需的每个色度分量Cb和Cr的附加色度量化参数(如果适用)。
		macroblock[CurrMbAddr]->QSC = macroblock[CurrMbAddr]->QPC;
	}

}




void ParseSlice::Picture_construction_process_prior_to_deblocking_filter_process(int* u, const char* type, const size_t BlkIdx, const bool isLuam, bool isChromaCb)
{
	//当前宏块 左上角亮度样点距离当前帧左上角的位置
	int xP = InverseRasterScan(CurrMbAddr, 16, 16, sHeader->sps.PicWidthInSamplesL, 0);
	int yP = InverseRasterScan(CurrMbAddr, 16, 16, sHeader->sps.PicWidthInSamplesL, 1);

	int xO = 0;
	int yO = 0;
	if (isLuam)
	{
		size_t nE = 0;
		if (type == "16*16")
		{
			xO = yO = 0;
			nE = 16;
		}
		else if (type == "4*4")
		{
			//得到在宏块中的索引为luma4x4BlkIdx的4x4亮度块左上 角样点的位置。
			xO = InverseRasterScan(BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(BlkIdx % 4, 4, 4, 8, 0);
			yO = InverseRasterScan(BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(BlkIdx % 4, 4, 4, 8, 1);
			nE = 4;
		}
		else
		{
			//得到在宏块中的索引为luma8x8BlkIdx的8x8亮度块左 上角样点的位置。
			xO = InverseRasterScan(BlkIdx, 8, 8, 16, 0);
			yO = InverseRasterScan(BlkIdx, 8, 8, 16, 1);
			nE = 8;
		}

		//S′L[ xP + xO + j, yP + yO + i ] = uij    with i, j = 0..nE -  1
		for (size_t i = 0; i < nE; i++)
		{
			for (size_t j = 0; j < nE; j++)
			{
				lumaData[xP + xO + j][yP + yO + i] = u[i * nE + j];
			}
		}
	}
	else
	{
		int MbWidthC = sHeader->sps.MbWidthC;
		int MbHeightC = sHeader->sps.MbHeightC;


		if (sHeader->sps.ChromaArrayType == 1 || sHeader->sps.ChromaArrayType == 2) // YUV420 or YUV422
		{
			//6.4.7 Inverse 4x4 chroma block scanning process chroma4x4BlkIdx
			/*const int xO = 0;
			const int yO = 0;*/


			uint8_t** chromaData = nullptr;

			if (isChromaCb)
			{
				chromaData = chromaCbData;
			}
			else
			{
				chromaData = chromaCrData;
			}


			for (size_t i = 0; i < MbWidthC; i++)
			{
				for (size_t j = 0; j < MbHeightC; j++)
				{
					chromaData[xP / sHeader->sps.SubWidthC + xO + j][yP / sHeader->sps.SubHeightC + yO + i] = u[i * MbWidthC + j];
				}

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

//transformDecodeChromaArrayTypeEqualTo3Process
void ParseSlice::transformDecodeChromaArrayTypeEqualTo3Process(bool isChromaCb)
{

	if (macroblock[CurrMbAddr]->mode == H264_MB_PART_PRED_MODE::Intra_16x16)
	{
		if (isChromaCb)
		{
			//transformDecode16x16LuamResidualProcess(macroblock[CurrMbAddr]->CbIntra16x16DCLevel, macroblock[CurrMbAddr]->CbIntra16x16DCLevel, false, isChromaCb)
		}
		else
		{

		}
	}


}
//Hadamard变换
void ParseSlice::transformDecodeIntra_16x16DCProcess(int c[4][4], int dcY[4][4], bool isLuam, bool isChromaCb)
{

	int qP = 0;

	if (isLuam)
	{
		qP = macroblock[CurrMbAddr]->QP1Y;
	}
	else
	{
		getChromaQuantisationParameters(isChromaCb);


		qP = macroblock[CurrMbAddr]->QP1C;
	}

	if (macroblock[CurrMbAddr]->TransformBypassModeFlag)
	{
		memcpy(dcY, c, sizeof(int) * 16);
	}
	else
	{
		//4 × 4 luma直流变换系数的逆变换
		const int a[4][4] = {
			{1,1,1,1},
			{1,1,-1,-1},
			{1,-1,-1,1},
			{1,-1,1,-1},
		};

		int g[4][4] = { 0 };
		int f[4][4] = { 0 };
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				for (size_t k = 0; k < 4; k++)
				{
					g[i][j] += a[i][k] * c[k][j];
				}
			}
		}
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				for (size_t k = 0; k < 4; k++)
				{
					f[i][j] += g[i][k] * a[k][j];
				}
			}
		}


		if (qP >= 36)
		{
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					//dcYij = ( fij * LevelScale4x4( qP % 6, 0, 0 ) ) << ( qP / 6 − 6 );
					dcY[i][j] = (f[i][j] * LevelScale4x4[qP % 6][0][0]) << (qP / 6 - 6);
				}
			}
		}
		else //if (qP < 36)
		{
			for (size_t i = 0; i < 4; i++)
			{
				for (int32_t j = 0; j < 4; j++)
				{
					//dcYij = ( fij * LevelScale4x4( qP % 6, 0, 0 ) + ( 1 << ( 5 − qP / 6) ) ) >> ( 6 − qP / 6 );
					dcY[i][j] = (f[i][j] * LevelScale4x4[qP % 6][0][0] + (1 << (5 - qP / 6))) >> (6 - qP / 6);
				}
			}
		}

	}
}
