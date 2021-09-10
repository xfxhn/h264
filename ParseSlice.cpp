#include "ParseSlice.h"



/// <summary>
/// ParseNalu& nalu  :nalu(nalu)
/// </summary>
ParseSlice::ParseSlice(SliceHeader& sHeader) :sHeader(sHeader)
{
	decodeFinished = false;
	mbX = 0;
	mbY = 0;

	macroblock = nullptr;
	lumaData = nullptr;
	chromaCbData = nullptr;
	chromaCrData = nullptr;
	memset(LevelScale4x4, 0, sizeof(int) * 6 * 4 * 4);
	memset(LevelScale8x8, 0, sizeof(int) * 6 * 8 * 8);
	CurrMbAddr = 0;

	PicWidthInSamplesL = sHeader.sps.PicWidthInSamplesL;
	PicHeightInSamplesL = sHeader.sps.PicHeightInSamplesL;
	PicWidthInSamplesC = sHeader.sps.PicWidthInSamplesC;
	PicHeightInSamplesC = sHeader.sps.PicHeightInSamplesC;
	PicSizeInMbs = sHeader.PicSizeInMbs;



	sliceNumber = 0;
	mbCount = 0;


	PicOrderCntMsb = 0;
	PicOrderCntLsb = 0;
	TopFieldOrderCnt = 0;
	BottomFieldOrderCnt = 0;

	FrameNumOffset = 0;
	PicOrderCnt = 0;
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

void ParseSlice::saveBmpFile(const char* filename)
{

	int width = PicWidthInSamplesL;
	int height = PicHeightInSamplesL;


	int sizeY = PicWidthInSamplesL * PicHeightInSamplesL;
	int sizeU = PicWidthInSamplesC * PicHeightInSamplesC;
	int sizeV = PicWidthInSamplesC * PicHeightInSamplesC;



	const size_t widthBytes = (width * 24 / 8 + 3) / 4 * 4;


	uint8_t* buffer = new uint8_t[height * widthBytes]();





	convertYuv420(buffer, widthBytes);
	FILE* fp = fopen("./output/xf1.bmp", "wb");
	if (!fp)
	{
		return;
	}


	MyBITMAPINFOHEADER bih;
	/* 文件的魔术数字。 由于对齐要求，它不适合头部结构，所以把它放在外面  高8位为字母’B’，低8位为字母’M’ */
	uint16_t bfType = 0x4d42;

	fwrite(&bfType, sizeof(uint16_t), 1, fp);

	MyBITMAPFILEHEADER bfh;
	//文件大小
	bfh.bfSize = 2 + sizeof(MyBITMAPFILEHEADER) + sizeof(MyBITMAPINFOHEADER) + (height * widthBytes);
	bfh.bfReserved1 = 0;//保留
	bfh.bfReserved2 = 0;//保留
	//偏移量 54个字节
	bfh.bfOffBits = 0x36;

	fwrite(&bfh, sizeof(MyBITMAPFILEHEADER), 1, fp);




	//MyBITMAPINFOHEADER结构体需要的字节
	bih.biSize = sizeof(MyBITMAPINFOHEADER);
	//图像宽
	bih.biWidth = width;
	//图像搞(如果是正的说明是倒向，如果是负的说明正向的)大多时候倒向的
	bih.biHeight = -height;
	//颜色平面数
	bih.biPlanes = 1;
	/*位图 数据记录了位图的每一个像素值，记录顺序是在扫描行内是从左到右，扫描行之间是从下到上。位图的一个像素值所占的字节数：
当biBitCount=1时，8个像素占1个字节；
当biBitCount=4时，2个像素占1个字节；
当biBitCount=8时，1个像素占1个字节；
当biBitCount=24时，1个像素占3个字节,按顺序分别为B,G,R；*/
	bih.biBitCount = 24;
	//压缩类型 0是不压缩
	bih.biCompression = 0;
	//图像大小用BI_RGB可设置为0
	bih.biSizeImage = (height * widthBytes);
	//水平分辨率
	bih.biXPelsPerMeter = 0;
	//垂直分辨率
	bih.biYPelsPerMeter = 0;
	//说明位图实际使用的彩色表中索引数（=0的话说明使用所有调色板）
	bih.biClrUsed = 0;
	//说明对图像显示有重要影响的颜色索引数目，=0表示都重要
	bih.biClrImportant = 0;

	fwrite(&bih, sizeof(MyBITMAPINFOHEADER), 1, fp);

	fwrite(buffer, height * widthBytes, 1, fp);

	fclose(fp);

}


void ParseSlice::convertYuv420(uint8_t* buffer, size_t widthBytes)
{

	//width=386 ,height= 384   色度184
	for (size_t y = 0; y < PicHeightInSamplesL; y++)
	{
		for (size_t x = 0; x < PicWidthInSamplesL; x++)
		{

			uint8_t Y = lumaData[x][y];
			uint8_t U = chromaCbData[x / 2][y / 2];
			uint8_t V = chromaCrData[x / 2][y / 2];


			int b = (1164 * (Y - 16) + 2018 * (U - 128)) / 1000;
			int g = (1164 * (Y - 16) - 813 * (V - 128) - 391 * (U - 128)) / 1000;
			int r = (1164 * (Y - 16) + 1596 * (V - 128)) / 1000;
			/*int b = 1.164 * (Y - 16) + 2.018 * (U - 128);
			int g = 1.164 * (Y - 16) - 0.380 * (U - 128) - 0.813 * (V - 128);
			int r = 1.164 * (Y - 16) + 1.159 * (V - 128);*/
			buffer[y * widthBytes + x * 3 + 0] = Clip3(0, 255, b);
			buffer[y * widthBytes + x * 3 + 1] = Clip3(0, 255, g);// Clip3(0, 255, g);
			buffer[y * widthBytes + x * 3 + 2] = Clip3(0, 255, r);

			//printf("U分量%d,V分量%d\n", U, V);
	/*	if (index % 4 == 0)
		{

		}*/


		}
	}


	/*for (size_t y = 0; y < PicHeightInSamplesC; y++)
	{
		for (size_t x = 0; x < PicWidthInSamplesC; x++)
		{
			uint8_t U = chromaCbData[x][y];
			uint8_t V = chromaCrData[x][y];
			const size_t  index = y * PicWidthInSamplesC + x;
			bu2[index] = U;
			bu3[index] = V;



		}
	}*/

}
//去块滤波器
void ParseSlice::Deblocking_filter_process()
{

	bool fieldMbInFrameFlag = false;
	bool filterInternalEdgesFlag = false;
	bool filterLeftMbEdgeFlag = false;
	bool filterTopMbEdgeFlag = false;
	//对每个宏块进行滤波
	for (size_t currMb = 0; currMb < PicSizeInMbs; currMb++)
	{
		int mbAddrA = NA;
		int mbAddrB = NA;


		int xW = NA;
		int yW = NA;
		constexpr int maxW = 16;
		constexpr int maxH = 16;
		getMbAddrNAndLuma4x4BlkIdxN(mbAddrA, -1, 0, maxW, maxH, xW, yW);
		getMbAddrNAndLuma4x4BlkIdxN(mbAddrB, 0, -1, maxW, maxH, xW, yW);
		//如果MbaffFrameFlag = 1且mb_field_decoding_flag = 1，则fieldMbInFrameFlag设置为1  

		if (sHeader.MbaffFrameFlag && sHeader.field_pic_flag)
		{
			fieldMbInFrameFlag = true;
		}
		else
		{
			fieldMbInFrameFlag = false;
		}

		/*	disable_deblocking_filter_idc
			0：开启去块滤波功能，去块滤波能穿越slice边界。
			1：关闭去块滤波功能。
			2：开启去块滤波功能，仅限slice内部*/

		if (sHeader.disable_deblocking_filter_idc == 1)
		{
			filterInternalEdgesFlag = false;
		}
		else
		{
			filterInternalEdgesFlag = true;
		}


		//currMb宏块是否在这帧图像最左侧边缘，是=false，否则=true
		if ((!sHeader.MbaffFrameFlag && (currMb % sHeader.sps.PicWidthInMbs == 0))
			|| (sHeader.MbaffFrameFlag && ((currMb >> 1) % sHeader.sps.PicWidthInMbs == 0))
			|| (sHeader.disable_deblocking_filter_idc == 1)
			|| (sHeader.disable_deblocking_filter_idc == 2 && mbAddrA == NA)
			)
		{
			filterLeftMbEdgeFlag = false;
		}
		else
		{
			filterLeftMbEdgeFlag = true;
		}


		if ((!sHeader.MbaffFrameFlag && currMb < sHeader.sps.PicWidthInMbs)
			|| (sHeader.MbaffFrameFlag && (currMb >> 1) < sHeader.sps.PicWidthInMbs && sHeader.field_pic_flag)
			|| (sHeader.MbaffFrameFlag && (currMb >> 1) < sHeader.sps.PicWidthInMbs && !sHeader.field_pic_flag && (currMb % 2) == 0)
			|| (sHeader.disable_deblocking_filter_idc == 1)
			|| (sHeader.disable_deblocking_filter_idc == 2 && mbAddrB == NA)
			)
		{
			filterTopMbEdgeFlag = false;
		}
		else
		{
			filterTopMbEdgeFlag = true;
		}


		bool chromaEdgeFlag = false;
		bool verticalEdgeFlag = false;
		bool fieldModeInFrameFilteringFlag = false;
		int iCbCr = 0;

		int xE[16] = { 0 };
		int yE[16] = { 0 };

		bool mbEdgeFlag = false;

		//filterLeftMbEdgeFlag=1 宏块最左侧垂直亮度边缘的滤波(不包含在图像最左侧的宏块)
		if (filterLeftMbEdgeFlag)
		{
			chromaEdgeFlag = false;
			verticalEdgeFlag = true;
			fieldModeInFrameFilteringFlag = fieldMbInFrameFlag;
			iCbCr = 0;


			mbEdgeFlag = true;
			for (size_t k = 0; k < 16; k++)
			{
				xE[k] = 0;
				yE[k] = k;
			}

			Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);


		}

		//宏块内部垂直边缘亮度滤波
		if (filterInternalEdgesFlag)
		{
			chromaEdgeFlag = false;
			verticalEdgeFlag = true;
			fieldModeInFrameFilteringFlag = fieldMbInFrameFlag;
			iCbCr = 0;
			mbEdgeFlag = false;
			if (macroblock[currMb]->transform_size_8x8_flag == false)
			{
				for (size_t k = 0; k < 16; k++)
				{
					xE[k] = 4; //(xEk, yEk) = (4, k)
					yE[k] = k;
				}

				Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);

			}

			for (size_t k = 0; k < 16; k++)
			{
				xE[k] = 8; //(xEk, yEk) = (8, k)
				yE[k] = k;
			}

			Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);


			if (macroblock[currMb]->transform_size_8x8_flag == false)
			{
				for (size_t k = 0; k < 16; k++)
				{
					xE[k] = 12; //(xEk, yEk) = (12, k)
					yE[k] = k;
				}

				Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
			}

		}

		//对顶部水平亮度边缘的过滤

		if (filterTopMbEdgeFlag)
		{
			chromaEdgeFlag = false;
			verticalEdgeFlag = false;
			fieldModeInFrameFilteringFlag = fieldMbInFrameFlag;
			iCbCr = 0;
			mbEdgeFlag = true;

			for (size_t k = 0; k < 16; k++)
			{
				xE[k] = k; //(xEk, yEk) = (k, 0)
				yE[k] = 0;
			}

			Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
		}

		//宏块内部水平边缘亮度滤波
		if (filterInternalEdgesFlag)
		{
			chromaEdgeFlag = false;
			verticalEdgeFlag = false;
			fieldModeInFrameFilteringFlag = fieldMbInFrameFlag;
			iCbCr = 0;
			mbEdgeFlag = false;


			if (macroblock[currMb]->transform_size_8x8_flag == false)
			{
				for (size_t k = 0; k < 16; k++)
				{
					xE[k] = k; //(xEk, yEk) = (4, k)
					yE[k] = 4;
				}

				Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);

			}

			for (size_t k = 0; k < 16; k++)
			{
				xE[k] = k; //(xEk, yEk) = (8, k)
				yE[k] = 8;
			}

			Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
			if (macroblock[currMb]->transform_size_8x8_flag == false)
			{
				for (size_t k = 0; k < 16; k++)
				{
					xE[k] = k; //(xEk, yEk) = (12, k)
					yE[k] = 12;
				}

				Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
			}
		}



		if (sHeader.sps.ChromaArrayType != 0)
		{
			if (filterLeftMbEdgeFlag)
			{
				chromaEdgeFlag = true;
				verticalEdgeFlag = true;
				fieldModeInFrameFilteringFlag = fieldMbInFrameFlag;
				mbEdgeFlag = true;

				for (size_t k = 0; k < sHeader.sps.MbHeightC; k++)
				{
					xE[k] = 0; //(xEk, yEk) = (0, k)
					yE[k] = k;
				}
				iCbCr = 0;
				Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
				iCbCr = 1;
				Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
			}
			//内部垂直色度边的滤波
			if (filterInternalEdgesFlag)
			{
				chromaEdgeFlag = true;
				verticalEdgeFlag = true;
				fieldModeInFrameFilteringFlag = fieldMbInFrameFlag;
				mbEdgeFlag = false;


				if (sHeader.sps.ChromaArrayType != 3 || macroblock[currMb]->transform_size_8x8_flag == false)
				{

					for (size_t k = 0; k < sHeader.sps.MbHeightC; k++)
					{
						xE[k] = 4; //(xEk, yEk) =  (4, k)
						yE[k] = k;
					}

					iCbCr = 0;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
					iCbCr = 1;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
				}

				if (sHeader.sps.ChromaArrayType == 3)
				{
					for (size_t k = 0; k < sHeader.sps.MbHeightC; k++)
					{
						xE[k] = 8; //(xEk, yEk) =  (8, k)
						yE[k] = k;
					}
					iCbCr = 0;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
					iCbCr = 1;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
				}

				if (sHeader.sps.ChromaArrayType == 3 && macroblock[currMb]->transform_size_8x8_flag == false)
				{
					for (size_t k = 0; k < sHeader.sps.MbHeightC; k++)
					{
						xE[k] = 12; //(xEk, yEk) =  (12, k)
						yE[k] = k;
					}
					iCbCr = 0;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
					iCbCr = 1;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
				}
			}



			if (filterTopMbEdgeFlag)
			{
				chromaEdgeFlag = true;
				verticalEdgeFlag = false;
				fieldModeInFrameFilteringFlag = fieldMbInFrameFlag;
				mbEdgeFlag = true;

				for (size_t k = 0; k < sHeader.sps.MbWidthC; k++)
				{
					xE[k] = k; //(xEk, yEk) =(k,  0)
					yE[k] = 0;
				}
				iCbCr = 0;
				Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
				iCbCr = 1;
				Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
			}

			//内部水平
			if (filterInternalEdgesFlag)
			{
				chromaEdgeFlag = true;
				verticalEdgeFlag = false;
				fieldModeInFrameFilteringFlag = fieldMbInFrameFlag;
				mbEdgeFlag = false;

				if (sHeader.sps.ChromaArrayType != 3 || macroblock[currMb]->transform_size_8x8_flag == false)
				{
					for (size_t k = 0; k < sHeader.sps.MbHeightC; k++)
					{
						xE[k] = k; //(xEk, yEk) = (k, 4)
						yE[k] = 4;
					}

					iCbCr = 0;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
					iCbCr = 1;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
				}


				if (sHeader.sps.ChromaArrayType != 1)
				{
					for (size_t k = 0; k < sHeader.sps.MbHeightC; k++)
					{
						xE[k] = k; //(xEk, yEk) = (k, 4)
						yE[k] = 8;
					}

					iCbCr = 0;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
					iCbCr = 1;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
				}

				//422
				if (sHeader.sps.ChromaArrayType == 2)
				{
					for (size_t k = 0; k < sHeader.sps.MbHeightC; k++)
					{
						xE[k] = k; //(xEk, yEk) = (k, 12)
						yE[k] = 12;
					}
					iCbCr = 0;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
					iCbCr = 1;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
				}

				if (sHeader.sps.ChromaArrayType == 3 && macroblock[currMb]->transform_size_8x8_flag == false)
				{
					for (size_t k = 0; k < sHeader.sps.MbHeightC; k++)
					{
						xE[k] = k; //(xEk, yEk) = (k, 12)
						yE[k] = 12;
					}
					iCbCr = 0;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
					iCbCr = 1;
					Filtering_process_for_block_edges(currMb, mbAddrA, chromaEdgeFlag, verticalEdgeFlag, fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, xE, yE);
				}
			}
		}

	}

}
void ParseSlice::Filtering_process_for_block_edges(int mbAddr, int mbAddrN, bool chromaEdgeFlag, bool verticalEdgeFlag, bool fieldModeInFrameFilteringFlag, int iCbCr, bool mbEdgeFlag, int xE[16], int yE[16])
{
	int nE = 0;
	//一个边沿包含的像素数量，亮度为16，色度420为8；
	if (chromaEdgeFlag)
	{
		nE = verticalEdgeFlag ? sHeader.sps.MbHeightC : sHeader.sps.MbWidthC;
	}
	else
	{
		nE = 16;
	}

	uint8_t** buff = nullptr;

	if (chromaEdgeFlag == false)
	{
		buff = lumaData;
		//PicWidthInSamples = PicWidthInSamplesL;
	}
	else if (chromaEdgeFlag == true && iCbCr == 0)
	{
		buff = chromaCbData;
		//PicWidthInSamples = PicWidthInSamplesC;
	}
	else if (chromaEdgeFlag == true && iCbCr == 1)
	{
		buff = chromaCrData;
		//PicWidthInSamples = PicWidthInSamplesC;
	}
	else
	{
		exit(-1);
		return printError("chromaEdgeFlag 错误");
	}


	const int dy = 1 + fieldModeInFrameFilteringFlag;


	const int xI = InverseRasterScan(mbAddr, 16, 16, PicWidthInSamplesL, 0);
	const int yI = InverseRasterScan(mbAddr, 16, 16, PicWidthInSamplesL, 1);

	//mbAddr宏块左上角样点位置到当前图像左上角的距离
	int xP = 0;
	int yP = 0;


	if (chromaEdgeFlag)
	{
		xP = xI / sHeader.sps.SubWidthC;
		yP = (yI + sHeader.sps.SubHeightC - 1) / sHeader.sps.SubHeightC;
	}
	else
	{
		xP = xI;
		yP = yI;
	}


	// p3 p2 p1 p0 | q0 q1 q2 q3

	for (size_t k = 0; k < nE; k++)
	{
		int p[4] = { 0 };
		int q[4] = { 0 };
		//获取每个参考样点的值
		for (size_t i = 0; i < 4; i++)
		{
			if (verticalEdgeFlag)
			{
				q[i] = buff[xP + xE[k] + i][yP + dy * yE[k]];
				p[i] = buff[xP + xE[k] - i - 1][yP + dy * yE[k]];
			}
			else
			{
				q[i] = buff[xP + xE[k]][yP + dy * (yE[k] + i) - (yE[k] % 2)];
				p[i] = buff[xP + xE[k]][yP + dy * (yE[k] - i - 1) - (yE[k] % 2)];
			}
		}


		//计算p0到宏块左上角的样点坐标 (用于后面计算4*4,8*8非0系数幅值)
		int mb_p0_x = 0;
		int mb_p0_y = 0;

		int mb_q0_x = 0;
		int mb_q0_y = 0;

		if (verticalEdgeFlag)
		{
			mb_p0_x = xE[k] - 1;
			mb_p0_y = yE[k];

			mb_q0_x = xE[k];
			mb_q0_y = yE[k];

			//表示左侧4个样点在另一个宏块里
			if (mb_p0_x < 0)
			{
				if (chromaEdgeFlag)
				{
					mb_p0_x += sHeader.sps.MbWidthC;
				}
				else
				{
					mb_p0_x += 16;
				}
			}
		}
		else
		{
			/*
			  (xEk, yEk) = (k, 4)
			  p3
			  p2
			  p1
			  p0
			  --
			  q0
			  q1
			  q2
			  q3
			  --
			*/
			mb_p0_x = xE[k];
			mb_p0_y = yE[k] - 1;

			mb_q0_x = xE[k];
			mb_q0_y = yE[k];

			if (mb_p0_y < 0)
			{
				if (chromaEdgeFlag)
				{
					mb_p0_y += sHeader.sps.MbHeightC;
				}
				else
				{
					mb_p0_y += 16;
				}
			}

		}

		int pp[3] = { 0 };
		int qq[3] = { 0 };


		Filtering_process_for_a_set_of_samples_across_a_horizontal_or_vertical_block_edge(mbAddr, mbAddrN, chromaEdgeFlag, verticalEdgeFlag,
			fieldModeInFrameFilteringFlag, iCbCr, mbEdgeFlag, p, q, pp, qq, mb_p0_x, mb_p0_y, mb_q0_x, mb_q0_y);


		for (size_t i = 0; i < 3; i++)
		{
			if (verticalEdgeFlag)
			{
				buff[xP + xE[k] + i][yP + dy * yE[k]] = qq[i];
				buff[xP + xE[k] - i - 1][yP + dy * yE[k]] = pp[i];
			}
			else
			{
				buff[xP + xE[k]][yP + dy * (yE[k] + i) - (yE[k] % 2)] = qq[i];
				buff[xP + xE[k]][yP + dy * (yE[k] - i - 1) - (yE[k] % 2)] = pp[i];
			}
		}

	}

}
void ParseSlice::Filtering_process_for_a_set_of_samples_across_a_horizontal_or_vertical_block_edge(int mbAddr, int mbAddrN, bool chromaEdgeFlag, bool verticalEdgeFlag, bool fieldModeInFrameFilteringFlag,
	int iCbCr, bool mbEdgeFlag, const  int p[4], const int q[4], int pp[3], int qq[3], const int mb_p0_x, const int mb_p0_y, const int mb_q0_x, const int mb_q0_y)
{
	int bS = 0;
	if (chromaEdgeFlag)
	{
		const int mb_x_p0_chroma = sHeader.sps.SubWidthC * mb_p0_x;
		const int mb_y_p0_chroma = sHeader.sps.SubWidthC * mb_p0_y;
		const int mb_x_q0_chroma = sHeader.sps.SubWidthC * mb_q0_x;
		const int mb_y_q0_chroma = sHeader.sps.SubWidthC * mb_q0_y;
		bS = Derivation_process_for_the_luma_content_dependent_boundary_filtering_strength(mbAddr, false, mbAddrN,
			mbEdgeFlag, p[0], q[0], verticalEdgeFlag, mb_x_p0_chroma, mb_y_p0_chroma, mb_x_q0_chroma, mb_y_q0_chroma);
	}
	else
	{
		bS = Derivation_process_for_the_luma_content_dependent_boundary_filtering_strength(mbAddr, false, mbAddrN, mbEdgeFlag, p[0], q[0], verticalEdgeFlag, mb_p0_x, mb_p0_y, mb_q0_x, mb_q0_y);
	}

	//sHeader.FilterOffsetA

	const int mbAddr_p0 = mbEdgeFlag ? mbAddrN : mbAddr;
	const int mbAddr_q0 = mbAddr;
	//FilterOffsetA以及FilterOffsetB则为偏移量，偏移量用于调整滤波强度
	const int8_t filterOffsetA = macroblock[mbAddr_q0]->FilterOffsetA;
	const int8_t filterOffsetB = macroblock[mbAddr_q0]->FilterOffsetB;

	int qPp = 0;
	int qPq = 0;
	// p3 p2 p1 p0 | q0 q1 q2 q3
	if (!chromaEdgeFlag)
	{
		if (macroblock[mbAddr_p0]->mbType == H264_MB_TYPE::I_PCM)
		{
			qPp = 0;
		}
		else
		{
			qPp = macroblock[mbAddr_p0]->QPY;
		}
		if (macroblock[mbAddr_q0]->mbType == H264_MB_TYPE::I_PCM)
		{
			qPq = 0;
		}
		else
		{
			qPq = macroblock[mbAddr_q0]->QPY;
		}
	}
	else
	{
		int QPY = 0;
		const bool isChromaCb = iCbCr == 0;
		if (macroblock[mbAddr_p0]->mbType == H264_MB_TYPE::I_PCM)
		{
			QPY = 0;
			qPp = getQPC(QPY, isChromaCb);
		}
		else
		{
			QPY = macroblock[mbAddr_p0]->QPY;
			qPp = getQPC(QPY, isChromaCb);
		}

		if (macroblock[mbAddr_q0]->mbType == H264_MB_TYPE::I_PCM)
		{
			QPY = 0;
			qPq = getQPC(QPY, isChromaCb);
		}
		else
		{
			QPY = macroblock[mbAddr_q0]->QPY;
			qPq = getQPC(QPY, isChromaCb);
		}
	}
	//α表示块与块之间的边界阈值，β表示块内部边界的阈值
	int alpha = 0;
	int beta = 0;

	//filterSamplesFlag它是指是否滤波了输入样点
	bool filterSamplesFlag = false;
	//表格索引
	int indexA = 0;
	Derivation_process_for_the_thresholds_for_each_block_edge(p[0], q[0], p[1], q[1], chromaEdgeFlag, bS, filterOffsetA, filterOffsetB, qPp, qPq, indexA, alpha, beta, filterSamplesFlag);

	const bool chromaStyleFilteringFlag = chromaEdgeFlag && (sHeader.sps.ChromaArrayType != 3);
	if (filterSamplesFlag)
	{
		if (bS < 4)
		{
			Filtering_process_for_edges_with_bS_less_than_4(p, q, pp, qq, chromaEdgeFlag, bS, beta, indexA, chromaStyleFilteringFlag);
		}
		else//(bS == 4)
		{
			Filtering_process_for_edges_for_bS_equal_to_4(p, q, pp, qq, alpha, beta, chromaStyleFilteringFlag);
		}
	}
	else
	{
		pp[0] = p[0];
		pp[1] = p[1];
		pp[2] = p[2];

		qq[0] = q[0];
		qq[1] = q[1];
		qq[2] = q[2];
	}

}
//滤波强度推导
int ParseSlice::Derivation_process_for_the_luma_content_dependent_boundary_filtering_strength(const int mbAddr, bool MbaffFrameFlag, int mbAddrN, bool mbEdgeFlag, int p0, int q0, bool verticalEdgeFlag,
	const int mb_p0_x, const int mb_p0_y, const int mb_q0_x, const int mb_q0_y)
{

	//如果MbaffFrameFlag等于1，样点p0和q0位于不同的宏块对中，其中一个是域宏块对，另外一个是帧宏块对，将mixedModeEdgeFlag置为1.否则，将mixedModeEdgeFlag置0。

	constexpr bool mixedModeEdgeFlag = false;

	const int mbAddr_p0 = mbEdgeFlag ? mbAddrN : mbAddr;
	const int mbAddr_q0 = mbAddr;

	const int luma4x4BlkIdx_p0 = ParseSlice::Derivation_process_for_4x4_luma_block_indices(mb_p0_x, mb_p0_y);
	const int luma8x8BlkIdx_p0 = ParseSlice::Derivation_process_for_8x8_luma_block_indices(mb_p0_x, mb_p0_y);

	const int luma4x4BlkIdx_q0 = ParseSlice::Derivation_process_for_4x4_luma_block_indices(mb_q0_x, mb_q0_y);
	const int luma8x8BlkIdx_q0 = ParseSlice::Derivation_process_for_8x8_luma_block_indices(mb_q0_x, mb_q0_y);
	int bS = 0;
	if (mbEdgeFlag)
	{
		//样点p0和q0都在帧宏块内，而且两个样点p0和q0之中的两个或者一个在采用帧内宏块预测模式编码的宏块内

		if (sHeader.sps.frame_mbs_only_flag && (isInterMode(macroblock[mbAddr_p0]->mode) || isInterMode(macroblock[mbAddr_q0]->mode))
			|| (sHeader.sps.frame_mbs_only_flag && (
				(macroblock[mbAddr_p0]->sliceNumber == macroblock[mbAddr_q0]->sliceNumber)
				&& ((SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP || (SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI))
				)
			|| ((MbaffFrameFlag || sHeader.field_pic_flag)
				&& verticalEdgeFlag
				&& (isInterMode(macroblock[mbAddr_p0]->mode) || isInterMode(macroblock[mbAddr_q0]->mode))
				)
			|| ((MbaffFrameFlag || sHeader.field_pic_flag)
				&& verticalEdgeFlag
				&& ((SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP || (SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI)
				)
			)
		{
			bS = 4;
		}

	}
	else if (
		(!mixedModeEdgeFlag
			&& (isInterMode(macroblock[mbAddr_p0]->mode) || isInterMode(macroblock[mbAddr_q0]->mode))
			)
		|| (!mixedModeEdgeFlag
			&& ((macroblock[mbAddr_p0]->sliceNumber == macroblock[mbAddr_q0]->sliceNumber)
				&& ((SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP || (SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI))
			)
		//mixedModeEdgeFlag==1
		)
	{
		bS = 3;
	}
	else if ((macroblock[mbAddr_p0]->transform_size_8x8_flag && macroblock[mbAddr_p0]->mb_luma_8x8_non_zero_count_coeff[luma8x8BlkIdx_p0] > 0)
		|| (!macroblock[mbAddr_p0]->transform_size_8x8_flag && macroblock[mbAddr_p0]->mb_luma_4x4_non_zero_count_coeff[luma4x4BlkIdx_p0] > 0)
		|| (macroblock[mbAddr_q0]->transform_size_8x8_flag && macroblock[mbAddr_q0]->mb_luma_8x8_non_zero_count_coeff[luma8x8BlkIdx_q0] > 0)
		|| (!macroblock[mbAddr_q0]->transform_size_8x8_flag && macroblock[mbAddr_q0]->mb_luma_4x4_non_zero_count_coeff[luma4x4BlkIdx_q0] > 0)
		)
	{
		bS = 2;
	}
	else if (mixedModeEdgeFlag || (false)) //帧间相关
	{
		bS = 1;
	}
	else
	{
		bS = 0;
	}

	return bS;

}

//判断真假边界，参考文档
//https://blog.csdn.net/weixin_30577801/article/details/96313356?utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromMachineLearnPai2%7Edefault-6.pc_relevant_baidujshouduan&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromMachineLearnPai2%7Edefault-6.pc_relevant_baidujshouduan
//每个块边缘的阈值推导过程
void ParseSlice::Derivation_process_for_the_thresholds_for_each_block_edge(int p0, int q0, int p1, int q1, bool chromaEdgeFlag, int bS, int filterOffsetA, int filterOffsetB, int qPp, int qPq, int& indexA, int& alpha, int& beta, bool& filterSamplesFlag)
{
	//指定平均量化参数的变量
	const int qPav = (qPp + qPq + 1) >> 1;
	indexA = Clip3(0, 51, qPav + filterOffsetA);
	const int indexB = Clip3(0, 51, qPav + filterOffsetB);

	//表8-16 indexA和indexB对应的阈值
	constexpr int alphas[52] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 4, 4, 5, 6,
		7, 8, 9, 10, 12, 13, 15, 17, 20, 22,
		25, 28, 32, 36, 40, 45, 50, 56, 63, 71,
		80, 90, 101, 113, 127, 144, 162, 182, 203, 226,
		255, 255
	};

	constexpr int betas[52] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 2, 2, 2, 3,
		3, 3, 3, 4, 4, 4, 6, 6, 7, 7,
		8, 8, 9, 9, 10, 10, 11, 11, 12, 12,
		13, 13, 14, 14, 15, 15, 16, 16, 17, 17,
		18, 18
	};

	if (!chromaEdgeFlag)
	{
		alpha = alphas[indexA] * (1 << (sHeader.sps.BitDepthY - 8));
		beta = betas[indexB] * (1 << (sHeader.sps.BitDepthY - 8));
	}
	else
	{
		alpha = alphas[indexA] * (1 << (sHeader.sps.BitDepthC - 8));
		beta = betas[indexB] * (1 << (sHeader.sps.BitDepthC - 8));
	}

	filterSamplesFlag = (bS != 0 && std::abs(p0 - q0) < alpha && std::abs(p1 - p0) < beta && std::abs(q1 - q0) < beta);


}

//滤波强度小于4推导
void ParseSlice::Filtering_process_for_edges_with_bS_less_than_4(const int p[4], const int q[4], int pp[3], int qq[3], bool chromaEdgeFlag, int bS, int beta, int indexA, bool chromaStyleFilteringFlag)
{

	constexpr int tC0Table[3][52] = {
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 2, 2, 2, 2, 3, 3, 3,
			4, 4, 4, 5, 6, 6, 7, 8, 9, 10,
			11, 13
		},
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 2, 2, 2, 2, 3, 3, 3, 4, 4,
			5, 5, 6, 7, 8, 8, 10, 11, 12, 13,
			15, 17
		},
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
			2, 3, 3, 3, 4, 4, 4, 5, 6, 6,
			7, 8, 9, 10, 11, 13, 14, 16, 18, 20,
			23, 25
		}
	};
	int tC0 = 0;
	if (chromaEdgeFlag)
	{
		tC0 = tC0Table[bS - 1][indexA] * (1 << (sHeader.sps.BitDepthC - 8));
	}
	else
	{
		tC0 = tC0Table[bS - 1][indexA] * (1 << (sHeader.sps.BitDepthY - 8));
	}

	// p3 p2 p1 p0 | q0 q1 q2 q3

	//参考像素的二级差分值  差值求绝对值，反应像素值的变化剧烈程度
	const int ap = std::abs(p[2] - p[0]);
	const int aq = std::abs(q[2] - q[0]);

	//阈值变量
	int tC = 0;
	if (chromaStyleFilteringFlag)
	{
		tC = tC0 + 1;
	}
	else
	{
		tC = tC0 + ((ap < beta) ? 1 : 0) + ((aq < beta) ? 1 : 0);
	}

	//delta差值运算图解 https://images2015.cnblogs.com/blog/421096/201605/421096-20160517004048404-1392900326.png
	const int delta = Clip3(-tC, tC, ((((q[0] - p[0]) << 2) + (p[1] - q[1]) + 4) >> 3));

	if (chromaEdgeFlag)
	{
		pp[0] = Clip3(0, (1 << sHeader.sps.BitDepthC) - 1, p[0] + delta);
		qq[0] = Clip3(0, (1 << sHeader.sps.BitDepthC) - 1, q[0] + delta);
	}
	else
	{
		pp[0] = Clip3(0, (1 << sHeader.sps.BitDepthY) - 1, p[0] + delta);
		qq[0] = Clip3(0, (1 << sHeader.sps.BitDepthY) - 1, q[0] + delta);
	}



	if (!chromaStyleFilteringFlag && ap < beta)
	{
		pp[1] = p[1] + Clip3(-tC0, tC0, (p[2] + ((p[0] + q[0] + 1) >> 1) - (p[1] << 1)) >> 1);
	}
	else //if (chromaStyleFilteringFlag is equal to 1 or ap is greater than or equal to beta)
	{
		pp[1] = p[1];
	}

	if (!chromaStyleFilteringFlag && aq < beta)
	{
		qq[1] = q[1] + Clip3(-tC0, tC0, (q[2] + ((p[0] + q[0] + 1) >> 1) - (q[1] << 1)) >> 1);
	}
	else //if (chromaEdgeFlag == 1)
	{
		qq[1] = q[1];
	}

	pp[2] = p[2];
	qq[2] = q[2];
}

void ParseSlice::Filtering_process_for_edges_for_bS_equal_to_4(const int p[4], const int q[4], int pp[3], int qq[3], int alpha, int beta, bool chromaStyleFilteringFlag)
{
	//参考像素的二级差分值  差值求绝对值，反应像素值的变化剧烈程度
	const int ap = std::abs(p[2] - p[0]);
	const int aq = std::abs(q[2] - q[0]);

	//满足下式细节信息不多采用强滤波
	if (!chromaStyleFilteringFlag && (ap < beta && std::abs(p[0] - q[0]) < ((alpha >> 2) + 2)))
	{
		pp[0] = (p[2] + 2 * p[1] + 2 * p[0] + 2 * q[0] + q[1] + 4) >> 3; //抽头滤波器
		pp[1] = (p[2] + p[1] + p[0] + q[0] + 2) >> 2;
		pp[2] = (2 * p[3] + 3 * p[2] + p[1] + p[0] + q[0] + 4) >> 3;
	}
	else
	{
		pp[0] = (2 * p[1] + p[0] + q[1] + 2) >> 2;
		pp[1] = p[1];
		pp[2] = p[2];
	}


	if (!chromaStyleFilteringFlag && (aq < beta && std::abs(p[0] - q[0]) < ((alpha >> 2) + 2)))
	{
		qq[0] = (p[1] + 2 * p[0] + 2 * q[0] + 2 * q[1] + q[2] + 4) >> 3; //抽头滤波器
		qq[1] = (p[0] + q[0] + q[1] + q[2] + 2) >> 2;
		qq[2] = (2 * q[3] + 3 * q[2] + q[1] + q[0] + p[0] + 4) >> 3;
	}
	else
	{
		qq[0] = (2 * q[1] + q[0] + p[1] + 2) >> 2;
		qq[1] = q[1];
		qq[2] = q[2];
	}
}





int ParseSlice::Derivation_process_for_4x4_luma_block_indices(int x, int y)
{
	return 8 * (y / 8) + 4 * (x / 8) + 2 * ((y % 8) / 4) + ((x % 8) / 4);
}
int ParseSlice::Derivation_process_for_8x8_luma_block_indices(int x, int y)
{
	return 2 * (y / 8) + (x / 8);
}

//色度DC哈达玛变换 
void ParseSlice::transformDecodeChromaDCProcess(int c[4][2], int dcC[4][2], int MbWidthC, int MbHeightC, bool isChromaCb)
{

	//只有当ChromaArrayType等于1或2时才会调用这个过程。

	int BitDepth = sHeader.sps.BitDepthC;


	getChromaQuantisationParameters(isChromaCb);

	int qP = macroblock[CurrMbAddr]->QP1C;

	/*int MbWidthC = sHeader.sps.MbWidthC;
	int MbHeightC = sHeader.sps.MbHeightC;*/
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
		if (sHeader.sps.ChromaArrayType == 1)
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
		else if (sHeader.sps.ChromaArrayType == 2)
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
			maxH = sHeader.sps.MbHeightC;
			maxW = sHeader.sps.MbWidthC;
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
			|| (isInterframe(macroblock[mbAddrN]->mode) && sHeader.pps.constrained_intra_pred_flag)
			|| (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI && sHeader.pps.constrained_intra_pred_flag)
			|| (x > 3) && (luma4x4BlkIdx == 3 || luma4x4BlkIdx == 11)
			)
		{
			P(x, y) = -1;
		}
		else
		{
			//xM  yM mbAddrN宏块左上角样点位置到当前图像左上角的距离
			int xM = InverseRasterScan(mbAddrN, 16, 16, sHeader.sps.PicWidthInSamplesL, 0);
			int yM = InverseRasterScan(mbAddrN, 16, 16, sHeader.sps.PicWidthInSamplesL, 1);

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


	//9种帧内4x4预测模式
	getIntra4x4PredMode(luma4x4BlkIdx, isLuam);

	const uint8_t Intra4x4PredMode = macroblock[CurrMbAddr]->Intra4x4PredMode[luma4x4BlkIdx];

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
			val = 1 << (sHeader.sps.BitDepthY - 1); //128 在h264
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

	int p[9 * 17]; //x范围[-1,15]，y范围[-1,7]，共9行17列，原点为pp[1][1]
	int32_t p1[9 * 17]; //存储参考滤波样点过程之后的值

	memset(p, -1, sizeof(int) * 9 * 17);
	memset(p1, -1, sizeof(int) * 9 * 17);
#define P(x, y)      p[((y) + 1) * 17 + ((x) + 1)]
#define P1(x, y)     p1[((y) + 1) * 17 + ((x) + 1)]

	const int xO = InverseRasterScan(luma8x8BlkIdx, 8, 8, 16, 0);
	const int yO = InverseRasterScan(luma8x8BlkIdx, 8, 8, 16, 1);

	for (size_t i = 0; i < 25; i++)
	{



		int maxW = 0;
		int maxH = 0;
		if (isLuam)
		{
			maxW = maxH = 16;
		}
		else
		{
			maxH = sHeader.sps.MbHeightC;
			maxW = sHeader.sps.MbWidthC;
		}
		int mbAddrN = NA;
		int xW = NA;
		int yW = NA;

		const int x = referenceCoordinateX[i];
		const int y = referenceCoordinateY[i];


		const int xN = xO + x;
		const int yN = yO + y;

		getMbAddrNAndLuma4x4BlkIdxN(mbAddrN, xN, yN, maxW, maxH, xW, yW);

		if (mbAddrN == NA || (isInterframe(macroblock[mbAddrN]->mode) && sHeader.pps.constrained_intra_pred_flag == 1))
		{
			P(x, y) = -1;//不可用于Intra_8x8预测
		}
		else
		{
			//xM  yM mbAddrN宏块左上角样点位置到当前图像左上角的距离
			int xM = InverseRasterScan(mbAddrN, 16, 16, sHeader.sps.PicWidthInSamplesL, 0);
			int yM = InverseRasterScan(mbAddrN, 16, 16, sHeader.sps.PicWidthInSamplesL, 1);

			P(x, y) = lumaData[xM + xW][yM + yW];
		}

	}



	if (P(8, -1) < 0 && P(9, -1) < 0 && P(10, -1) < 0 && P(11, -1) < 0 && P(12, -1) < 0 && P(13, -1) < 0 && P(14, -1) < 0 && P(15, -1) < 0 && P(7, -1) >= 0)
	{
		P(8, -1) = P(7, -1);
		P(9, -1) = P(7, -1);
		P(10, -1) = P(7, -1);
		P(11, -1) = P(7, -1);
		P(12, -1) = P(7, -1);
		P(13, -1) = P(7, -1);
		P(14, -1) = P(7, -1);
		P(15, -1) = P(7, -1);
	}


	//Intra_8x8 样点预测时的滤波参考样点

	if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
		&& P(8, -1) >= 0 && P(9, -1) >= 0 && P(10, -1) >= 0 && P(11, -1) >= 0 && P(12, -1) >= 0 && P(13, -1) >= 0 && P(14, -1) >= 0 && P(15, -1) >= 0
		)
	{
		//p′[ 0, −1 ]推导
		if (P(-1, -1) >= 0)
		{
			P1(0, -1) = (P(-1, -1) + 2 * P(0, -1) + P(1, -1) + 2) >> 2;
		}
		else
		{
			P1(0, -1) = (3 * P(0, -1) + P(1, -1) + 2) >> 2;
		}

		for (size_t x = 0; x < 15; x++)
		{
			P1(x, -1) = (P(x - 1, -1) + 2 * P(x, -1) + P(x + 1, -1) + 2) >> 2;
		}

		P1(15, -1) = (P(14, -1) + 3 * P(15, -1) + 2) >> 2;
	}

	//p′[ −1, −1 ]
	if (P(-1, -1) >= 0)
	{
		//当样本p[0，−1]和p[−1,0]被标记为“不可用于Intra_8x8预测”时，派生的样本p '[−1，−1]不用于内部预测过程。 
		if (P(0, -1) < 0 || P(-1, 0) < 0)
		{
			if (P(0, -1) >= 0)
			{
				P1(-1, -1) = (3 * P(-1, -1) + P(0, -1) + 2) >> 2;
			}
			else if (P(0, -1) < 0 && P(-1, 0) >= 0)
			{
				P1(-1, -1) = (3 * P(-1, -1) + P(-1, 0) + 2) >> 2;
			}
			else //if (P(0, -1) < 0 && P(-1, 0) < 0)
			{
				P1(-1, -1) = P(-1, -1);
			}
		}
		else //if (P(0, -1) >= 0 || P(-1, 0) >= 0)
		{
			P1(-1, -1) = (P(0, -1) + 2 * P(-1, -1) + P(-1, 0) + 2) >> 2;
		}
	}



	if (P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0)
	{
		//p′[ −1, 0 ]
		if (P(-1, -1) >= 0)
		{
			P1(-1, 0) = (P(-1, -1) + 2 * P(-1, 0) + P(-1, 1) + 2) >> 2;
		}
		else//if (P(-1, -1) < 0)
		{
			P1(-1, 0) = (3 * P(-1, 0) + P(-1, 1) + 2) >> 2;
		}

		for (size_t y = 1; y < 7; y++)
		{
			P1(-1, y) = (P(-1, y - 1) + 2 * P(-1, y) + P(-1, y + 1) + 2) >> 2;
		}

		P1(-1, 7) = (P(-1, 6) + 3 * P(-1, 7) + 2) >> 2;
	}


	memcpy(p, p1, sizeof(int32_t) * 9 * 17);

	getIntra8x8PredMode(luma8x8BlkIdx, isLuam);


	const uint8_t Intra8x8PredMode = macroblock[CurrMbAddr]->Intra8x8PredMode[luma8x8BlkIdx];

	if (Intra8x8PredMode == 0) //Intra_8x8_Vertical
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0)
		{

			for (size_t y = 0; y < 8; y++)
			{
				for (size_t x = 0; x < 8; x++)
				{
					macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = P(x, -1);
				}
			}

		}
	}
	else if (Intra8x8PredMode == 1)//Intra_8x8_Horizontal
	{
		if (P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0)
		{
			for (size_t y = 0; y < 8; y++)
			{
				for (size_t x = 0; x < 8; x++)
				{
					macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = P(-1, y);
				}
			}
		}
	}
	else if (Intra8x8PredMode == 2) //Intra_8x8_DC
	{
		int val = 0;
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0
			)
		{
			val = (P(0, -1) + P(1, -1) + P(2, -1) + P(3, -1) + P(4, -1) + P(5, -1) + P(6, -1) + P(7, -1)
				+ P(-1, 0) + P(-1, 1) + P(-1, 2) + P(-1, 3) + P(-1, 4) + P(-1, 5) + P(-1, 6) + P(-1, 7) + 8) >> 4;
		}
		else if ((P(0, -1) < 0 || P(1, -1) < 0 || P(2, -1) < 0 || P(3, -1) < 0 || P(4, -1) < 0 || P(5, -1) < 0 || P(6, -1) < 0 || P(7, -1) < 0)
			&& (P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0)
			)
		{
			val = (P(-1, 0) + P(-1, 1) + P(-1, 2) + P(-1, 3) + P(-1, 4) + P(-1, 5) + P(-1, 6) + P(-1, 7) + 4) >> 3;
		}
		else if ((P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0)
			&& (P(-1, 0) < 0 || P(-1, 1) < 0 || P(-1, 2) < 0 || P(-1, 3) < 0 || P(-1, 4) < 0 || P(-1, 5) < 0 || P(-1, 6) < 0 || P(-1, 7) < 0)
			)
		{
			val = (P(0, -1) + P(1, -1) + P(2, -1) + P(3, -1) + P(4, -1) + P(5, -1) + P(6, -1) + P(7, -1) + 4) >> 3;
		}
		else //some samples p[ x, -1 ], with x = 0..7, and some samples p[ -1, y ], with y = 0..7, are marked as "not available for Intra_8x8 prediction")
		{
			val = 1 << (sHeader.sps.BitDepthY - 1); //mean_value = 1 << (8 - 1) = 128;
		}


		for (size_t y = 0; y < 8; y++)
		{
			for (size_t x = 0; x < 8; x++)
			{
				macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = val;
			}
		}
	}
	else if (Intra8x8PredMode == 3) //Intra_8x8_Diagonal_Down_Left
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(8, -1) >= 0 && P(9, -1) >= 0 && P(10, -1) >= 0 && P(11, -1) >= 0 && P(12, -1) >= 0 && P(13, -1) >= 0 && P(14, -1) >= 0 && P(15, -1) >= 0
			)
		{
			for (size_t y = 0; y < 8; y++)
			{
				for (size_t x = 0; x < 8; x++)
				{
					if (x == 7 && y == 7)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(14, -1) + 3 * P(15, -1) + 2) >> 2; //red8x8L[y * 8 + x]
					}
					else //if (x != 7 || y != 7)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(x + y, -1) + 2 * P(x + y + 1, -1) + P(x + y + 2, -1) + 2) >> 2; //red8x8L[y * 8 + x]
					}
				}
			}
		}
	}
	else if (Intra8x8PredMode == 4) //Intra_8x8_Diagonal_Down_Right
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(-1, -1) >= 0 && P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0
			)
		{
			for (size_t y = 0; y < 8; y++)
			{
				for (size_t x = 0; x < 8; x++)
				{
					if (x > y)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(x - y - 2, -1) + 2 * P(x - y - 1, -1) + P(x - y, -1) + 2) >> 2; //red8x8L[y * 8 + x]
					}
					else if (x < y)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(-1, y - x - 2) + 2 * P(-1, y - x - 1) + P(-1, y - x) + 2) >> 2; //red8x8L[y * 8 + x]
					}
					else //if (x == y)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(0, -1) + 2 * P(-1, -1) + P(-1, 0) + 2) >> 2; //red8x8L[y * 8 + x]
					}
				}
			}
		}
	}
	else if (Intra8x8PredMode == 5)//Intra_8x8_Vertical_Right
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(-1, -1) >= 0 && P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0
			)
		{
			for (size_t y = 0; y < 8; y++)
			{
				for (size_t x = 0; x < 8; x++)
				{
					size_t zVR = 2 * x - y;

					if (zVR == 0 || zVR == 2 || zVR == 4 || zVR == 6 || zVR == 8 || zVR == 10 || zVR == 12 || zVR == 14)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(x - (y >> 1) - 1, -1) + P(x - (y >> 1), -1) + 1) >> 1; //red8x8L[y * 8 + x]
					}
					else if (zVR == 1 || zVR == 3 || zVR == 5 || zVR == 7 || zVR == 9 || zVR == 11 || zVR == 13)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(x - (y >> 1) - 2, -1) + 2 * P(x - (y >> 1) - 1, -1) + P(x - (y >> 1), -1) + 2) >> 2; //red8x8L[y * 8 + x]
					}
					else if (zVR == -1)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(-1, 0) + 2 * P(-1, -1) + P(0, -1) + 2) >> 2; //red8x8L[y * 8 + x]
					}
					else //if (zVR == -2 || zVR == -3 || zVR == -4 || zVR == -5 || zVR == -6 || zVR == -7)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(-1, y - 2 * x - 1) + 2 * P(-1, y - 2 * x - 2) + P(-1, y - 2 * x - 3) + 2) >> 2; //red8x8L[y * 8 + x]
					}
				}
			}
		}
	}
	else if (Intra8x8PredMode == 6) //Intra_8x8_Horizontal_Down
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(-1, -1) >= 0 && P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0
			)
		{
			for (size_t y = 0; y < 8; y++)
			{
				for (size_t x = 0; x < 8; x++)
				{
					size_t zHD = 2 * y - x;

					if (zHD == 0 || zHD == 2 || zHD == 4 || zHD == 6 || zHD == 8 || zHD == 10 || zHD == 12 || zHD == 14)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(-1, y - (x >> 1) - 1) + P(-1, y - (x >> 1)) + 1) >> 1; //red8x8L[y * 8 + x]
					}
					else if (zHD == 1 || zHD == 3 || zHD == 5 || zHD == 7 || zHD == 9 || zHD == 11 || zHD == 13)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(-1, y - (x >> 1) - 2) + 2 * P(-1, y - (x >> 1) - 1) + P(-1, y - (x >> 1)) + 2) >> 2; //red8x8L[y * 8 + x]
					}
					else if (zHD == -1)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(-1, 0) + 2 * P(-1, -1) + P(0, -1) + 2) >> 2; //red8x8L[y * 8 + x]
					}
					else //if (zHD == -2 || zHD == -3 || zHD == -4 || zHD == -5 || zHD == -6 || zHD == -7)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(x - 2 * y - 1, -1) + 2 * P(x - 2 * y - 2, -1) + P(x - 2 * y - 3, -1) + 2) >> 2; //red8x8L[y * 8 + x]
					}
				}
			}
		}
	}
	else if (Intra8x8PredMode == 7)//Intra_8x8_Vertical_Left
	{
		if (P(0, -1) >= 0 && P(1, -1) >= 0 && P(2, -1) >= 0 && P(3, -1) >= 0 && P(4, -1) >= 0 && P(5, -1) >= 0 && P(6, -1) >= 0 && P(7, -1) >= 0
			&& P(8, -1) >= 0 && P(9, -1) >= 0 && P(10, -1) >= 0 && P(11, -1) >= 0 && P(12, -1) >= 0 && P(13, -1) >= 0 && P(14, -1) >= 0 && P(15, -1) >= 0
			)
		{
			for (size_t y = 0; y < 8; y++)
			{
				for (size_t x = 0; x < 8; x++)
				{
					if (y == 0 || y == 2 || y == 4 || y == 6)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(x + (y >> 1), -1) + P(x + (y >> 1) + 1, -1) + 1) >> 1; //red8x8L[y * 8 + x]
					}
					else //if (y == 1 || y == 3 || y == 5 || y == 7)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(x + (y >> 1), -1) + 2 * P(x + (y >> 1) + 1, -1) + P(x + (y >> 1) + 2, -1) + 2) >> 2; //red8x8L[y * 8 + x]
					}
				}
			}
		}
	}
	else if (Intra8x8PredMode == 8)//Intra_8x8_Horizontal_Up
	{
		if (P(-1, 0) >= 0 && P(-1, 1) >= 0 && P(-1, 2) >= 0 && P(-1, 3) >= 0 && P(-1, 4) >= 0 && P(-1, 5) >= 0 && P(-1, 6) >= 0 && P(-1, 7) >= 0)
		{
			for (size_t y = 0; y < 8; y++)
			{
				for (size_t x = 0; x < 8; x++)
				{
					size_t zHU = x + 2 * y;

					if (zHU == 0 || zHU == 2 || zHU == 4 || zHU == 6 || zHU == 8 || zHU == 10 || zHU == 12)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(-1, y + (x >> 1)) + P(-1, y + (x >> 1) + 1) + 1) >> 1; //red8x8L[y * 8 + x]
					}
					else if (zHU == 1 || zHU == 3 || zHU == 5 || zHU == 7 || zHU == 9 || zHU == 11)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(-1, y + (x >> 1)) + 2 * P(-1, y + (x >> 1) + 1) + P(-1, y + (x >> 1) + 2) + 2) >> 2; //red8x8L[y * 8 + x]
					}
					else if (zHU == 13)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = (P(-1, 6) + 3 * P(-1, 7) + 2) >> 2; //red8x8L[y * 8 + x]
					}
					else //if (zHU > 13)
					{
						macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][x][y] = P(-1, 7); //red8x8L[y * 8 + x]
					}
				}
			}
		}
	}
	else
	{
		printError("8*8预测模式错误");

	}



#undef P
#undef P1

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
			maxH = sHeader.sps.MbHeightC;
			maxW = sHeader.sps.MbWidthC;
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
			|| (isInterframe(macroblock[mbAddrN]->mode) && sHeader.pps.constrained_intra_pred_flag)
			|| (macroblock[CurrMbAddr]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI && sHeader.pps.constrained_intra_pred_flag)
			)
		{
			P(x, y) = -1;
		}
		else
		{
			//xM  yM mbAddrN宏块左上角样点位置到当前图像左上角的距离
			int xM = InverseRasterScan(mbAddrN, 16, 16, sHeader.sps.PicWidthInSamplesL, 0);
			int yM = InverseRasterScan(mbAddrN, 16, 16, sHeader.sps.PicWidthInSamplesL, 1);

			P(x, y) = lumaData[xM + xW][yM + yW];
		}
	}




	const uint8_t Intra16x16PredMode = macroblock[CurrMbAddr]->Intra16x16PredMode;
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
			val = 1 << (sHeader.sps.BitDepthY - 1); //128 在h264
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
					macroblock[CurrMbAddr]->luma16x16PredSamples[x][y] = Clip3(0, (1 << sHeader.sps.BitDepthY) - 1, (a + b * (x - 7) + c * (y - 7) + 16) >> 5);
				}
			}
		}

	}


#undef P;


}

void ParseSlice::Intra_chroma_prediction(bool isChromaCb)
{
	if (sHeader.sps.ChromaArrayType == 3)
	{

	}
	else // 420, 422
	{
		const int MbWidthC = sHeader.sps.MbWidthC;
		const int MbHeightC = sHeader.sps.MbHeightC;

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
				|| (isInterframe(macroblock[mbAddrN]->mode) && sHeader.pps.constrained_intra_pred_flag)
				|| (macroblock[mbAddrN]->fix_slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI && sHeader.pps.constrained_intra_pred_flag && macroblock[CurrMbAddr]->fix_slice_type != SLIECETYPE::H264_SLIECE_TYPE_SI)
				)
			{
				P(x, y) = -1;
			}
			else
			{
				//xL  yL mbAddrN宏块左上角样点位置到当前图像左上角的距离
				int xL = InverseRasterScan(mbAddrN, 16, 16, sHeader.sps.PicWidthInSamplesL, 0);
				int yL = InverseRasterScan(mbAddrN, 16, 16, sHeader.sps.PicWidthInSamplesL, 1);

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
			for (size_t chroma4x4BlkIdx = 0; chroma4x4BlkIdx < (1 << (sHeader.sps.ChromaArrayType + 1)); chroma4x4BlkIdx++)
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
						val = (1 << (sHeader.sps.BitDepthC - 1));
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
						val = (1 << (sHeader.sps.BitDepthC - 1));
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
						val = (1 << (sHeader.sps.BitDepthC - 1));
					}
				}


				for (size_t y = 0; y < 4; y++)
				{
					for (size_t x = 0; x < 4; x++)
					{
						macroblock[CurrMbAddr]->chromaPredSamples[x + xO][y + yO] = val;
					}
				}
			}
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
				int xCF = ((sHeader.sps.ChromaArrayType == 3) ? 4 : 0);
				int yCF = ((sHeader.sps.ChromaArrayType != 1) ? 4 : 0);

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
				int b = ((34 - 29 * (sHeader.sps.ChromaArrayType == 3)) * H + 32) >> 6;
				int c = ((34 - 29 * (sHeader.sps.ChromaArrayType != 1)) * V + 32) >> 6;

				for (int y = 0; y < MbHeightC; y++)
				{
					for (int x = 0; x < MbWidthC; x++)
					{
						macroblock[CurrMbAddr]->chromaPredSamples[x][y] = Clip3(0, (1 << sHeader.sps.BitDepthC) - 1, (a + b * (x - 3 - xCF) + c * (y - 3 - yCF) + 16) >> 5);
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
		maxH = sHeader.sps.MbHeightC;
		maxW = sHeader.sps.MbWidthC;
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

	//当前子块距离mbAddrN左上角样点距离（ xW, yW)
	int xW = NA;
	int yW = NA;

	//亮度位置的差分值 表6-2（ xD, yD ）

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
		(mbAddrA != NA && isInterframe(macroblock[mbAddrA]->mode) && sHeader.pps.constrained_intra_pred_flag) ||
		(mbAddrB != NA && isInterframe(macroblock[mbAddrB]->mode)) && sHeader.pps.constrained_intra_pred_flag)
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
			intraMxMPredModeA = macroblock[mbAddrA]->Intra8x8PredMode[luma4x4BlkIdxA >> 2];
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
			intraMxMPredModeB = macroblock[mbAddrB]->Intra8x8PredMode[luma4x4BlkIdxB >> 2];
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


void ParseSlice::getIntra8x8PredMode(size_t luma8x8BlkIdx, bool isLuam)
{
	int maxW = 0;
	int maxH = 0;
	if (isLuam)
	{
		maxW = maxH = 16;
	}
	else
	{
		//色度块高度和宽度
		maxH = sHeader.sps.MbHeightC;
		maxW = sHeader.sps.MbWidthC;
	}
	/*xN = (luma8x8BlkIdx % 2) * 8 + xD
	yN = (luma8x8BlkIdx / 2) * 8 + yD*/
	const int x = (luma8x8BlkIdx % 2) * 8;
	const int y = (luma8x8BlkIdx / 2) * 8;



	//等于CurrMbAddr或等于包含（xN，yN）的相邻宏块的地址，及其可用性状态
	int mbAddrA = NA;
	int mbAddrB = NA;

	//位于4×4块luma4x4BlkIdx左侧和上侧的4×4亮度块的索引及其可用性状态
	int luma8x8BlkIdxA = NA;
	int luma8x8BlkIdxB = NA;

	int xW = NA;
	int yW = NA;
	getMbAddrNAndLuma4x4BlkIdxN(mbAddrA, x + (-1), y + (0), maxW, maxH, xW, yW);
	if (mbAddrA != NA)
	{
		//左侧宏块子块索引
		luma8x8BlkIdxA = 2 * (yW / 8) + (xW / 8);
	}

	getMbAddrNAndLuma4x4BlkIdxN(mbAddrB, x + (0), y + (-1), maxW, maxH, xW, yW);

	if (mbAddrB != NA)
	{
		//左侧宏块子块索引
		luma8x8BlkIdxB = 2 * (yW / 8) + (xW / 8);
	}

	bool dcPredModePredictedFlag = false;

	if (mbAddrA == NA
		|| mbAddrB == NA
		|| (mbAddrA != NA && isInterframe(macroblock[mbAddrA]->mode) && sHeader.pps.constrained_intra_pred_flag)
		|| (mbAddrB != NA && isInterframe(macroblock[mbAddrB]->mode) && sHeader.pps.constrained_intra_pred_flag)
		)
	{
		dcPredModePredictedFlag = true;
	}
	else
	{
		dcPredModePredictedFlag = false;
	}


	int intraMxMPredModeA = 0;
	int intraMxMPredModeB = 0;

	if (dcPredModePredictedFlag ||
		(macroblock[mbAddrA]->mode != H264_MB_PART_PRED_MODE::Intra_4x4 && macroblock[mbAddrA]->mode != H264_MB_PART_PRED_MODE::Intra_8x8))
	{
		intraMxMPredModeA = 2;//Prediction_Mode_Intra_8x8_DC = 2;
	}
	else
	{
		if (macroblock[mbAddrA]->mode == H264_MB_PART_PRED_MODE::Intra_8x8)
		{
			intraMxMPredModeA = macroblock[mbAddrA]->Intra8x8PredMode[luma8x8BlkIdxA];
		}
		else//以Intra_4x4预测方式编码
		{
			//变量n是通过帧场自适应来设置不同的值，不支持场编码n=1
			int n = 1;
			intraMxMPredModeA = macroblock[mbAddrA]->Intra4x4PredMode[luma8x8BlkIdxA * 4 + n];
		}
	}

	if (dcPredModePredictedFlag || (macroblock[mbAddrB]->mode != H264_MB_PART_PRED_MODE::Intra_4x4 && macroblock[mbAddrB]->mode != H264_MB_PART_PRED_MODE::Intra_8x8))
	{
		intraMxMPredModeB = 2;//Prediction_Mode_Intra_8x8_DC = 2;
	}
	else
	{
		if (macroblock[mbAddrB]->mode == H264_MB_PART_PRED_MODE::Intra_8x8)
		{
			intraMxMPredModeB = macroblock[mbAddrB]->Intra8x8PredMode[luma8x8BlkIdxB];
		}
		else//以Intra_4x4预测方式编码
		{
			//N=B的时候 n=2
			constexpr int n = 2;
			intraMxMPredModeB = macroblock[mbAddrB]->Intra4x4PredMode[luma8x8BlkIdxB * 4 + n];
		}
	}
	const int predIntra8x8PredMode = std::min(intraMxMPredModeA, intraMxMPredModeB);


	if (macroblock[CurrMbAddr]->prev_intra8x8_pred_mode_flag[luma8x8BlkIdx])
	{
		macroblock[CurrMbAddr]->Intra8x8PredMode[luma8x8BlkIdx] = predIntra8x8PredMode;
	}
	else
	{
		if (macroblock[CurrMbAddr]->rem_intra8x8_pred_mode[luma8x8BlkIdx] < predIntra8x8PredMode)
		{
			macroblock[CurrMbAddr]->Intra8x8PredMode[luma8x8BlkIdx] = macroblock[CurrMbAddr]->rem_intra8x8_pred_mode[luma8x8BlkIdx];
		}
		else
		{
			macroblock[CurrMbAddr]->Intra8x8PredMode[luma8x8BlkIdx] = macroblock[CurrMbAddr]->rem_intra8x8_pred_mode[luma8x8BlkIdx] + 1;
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
		int mbAddrD = CurrMbAddr - sHeader.sps.PicWidthInMbs - 1;
		//不可用

		if (isMbUsable(mbAddrD, CurrMbAddr) || CurrMbAddr % sHeader.sps.PicWidthInMbs == 0)
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
		if (isMbUsable(mbAddrA, CurrMbAddr) || CurrMbAddr % sHeader.sps.PicWidthInMbs == 0)
		{

		}
		else
		{
			mbAddrN = mbAddrA;
		}
	}
	else if ((xN >= 0 && xN <= maxW - 1) && yN < 0)
	{
		int mbAddrB = CurrMbAddr - sHeader.sps.PicWidthInMbs;
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
		int mbAddrC = CurrMbAddr - sHeader.sps.PicWidthInMbs + 1;
		if (isMbUsable(mbAddrC, CurrMbAddr) || (CurrMbAddr + 1) % sHeader.sps.PicWidthInMbs == 0)
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
			/*int xO = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 0);
			int yO = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 1);*/

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
					u[i * 4 + j] = Clip3(0, (1 << sHeader.sps.BitDepthY) - 1, macroblock[CurrMbAddr]->lumaPredSamples[luma4x4BlkIdx][j][i] + r[i][j]);
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

		/*int xO = InverseRasterScan(luma8x8BlkIdx, 8, 8, 16, 0);
		int yO = InverseRasterScan(luma8x8BlkIdx, 8, 8, 16, 1);*/

		Intra_8x8_prediction(luma8x8BlkIdx, true);

		int u[64] = { 0 };

		for (size_t i = 0; i < 8; i++)
		{
			for (size_t j = 0; j < 8; j++)
			{
				u[i * 8 + j] = Clip3(0, (1 << sHeader.sps.BitDepthY) - 1, macroblock[CurrMbAddr]->luma8x8PredSamples[luma8x8BlkIdx][j][i] + r[i][j]);
			}
		}

		Picture_construction_process_prior_to_deblocking_filter_process(u, "8*8", luma8x8BlkIdx, true);

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
			u[i * 16 + j] = Clip3(0, (1 << sHeader.sps.BitDepthY) - 1, macroblock[CurrMbAddr]->luma16x16PredSamples[j][i] + rMb[j][i]);
		}
	}
	Picture_construction_process_prior_to_deblocking_filter_process(u, "16*16", 0, true);

}

//色度解码
void ParseSlice::transformDecodeChromaResidualProcess(bool isChromaCb)
{
	if (sHeader.sps.ChromaArrayType == 0)
	{
		printError("ChromaArrayType==0");
		exit(0);
	}


	if (sHeader.sps.ChromaArrayType == 3)
	{
		//对ChromaArrayType等于3 444 的色度样本的转换解码过程的规范
		transformDecodeChromaArrayTypeEqualTo3Process(isChromaCb);
	}
	else
	{

		int MbWidthC = sHeader.sps.MbWidthC;
		int MbHeightC = sHeader.sps.MbHeightC;
		//420 8 8
		int numChroma4x4Blks = (MbWidthC / 4) * (MbHeightC / 4);
		int iCbCr = (isChromaCb) ? 0 : 1;



		int dcC[4][2] = { 0 };
		if (sHeader.sps.ChromaArrayType == 1) //420
		{
			int c[2][2] = { 0 };

			//420最大DC系数有4个
			c[0][0] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][0];
			c[0][1] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][1];
			c[1][0] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][2];
			c[1][1] = macroblock[CurrMbAddr]->ChromaDCLevel[iCbCr][3];
			transformDecodeChromaDCProcess(c, dcC, MbWidthC, MbHeightC, isChromaCb);
		}
		else if (sHeader.sps.ChromaArrayType == 2)  //422
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
				u[i * MbWidthC + j] = Clip3(0, (1 << sHeader.sps.BitDepthC) - 1, macroblock[CurrMbAddr]->chromaPredSamples[j][i] + rMb[j][i]);
			}
		}

		Picture_construction_process_prior_to_deblocking_filter_process(u, "4*4", 0, false, isChromaCb);

		delete[] u;
		u = nullptr;

	}

}

void ParseSlice::Inter_prediction_process(DPB& dpb)
{

	Macroblock* mb = macroblock[CurrMbAddr];


	int NumMbPart = 0;
	if (mb->mbType == H264_MB_TYPE::B_Skip || mb->mbType == H264_MB_TYPE::B_Direct_16x16)
	{
		NumMbPart = 4;//mbPartIdx在0…3之间取值
	}
	else
	{
		NumMbPart = mb->NumMbPart; //mbPartIdx在0…NumMbPart( mb_type ) − 1
	}

	//xM  yM mbAddrN宏块左上角样点位置到当前图像左上角的距离
	int xM = InverseRasterScan(CurrMbAddr, 16, 16, sHeader.sps.PicWidthInSamplesL, 0);
	int yM = InverseRasterScan(CurrMbAddr, 16, 16, sHeader.sps.PicWidthInSamplesL, 1);

	for (size_t mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
	{
		//宏块分区mbPartIdx的左上luma样本相对于宏块左上样本的位置(x, y)。  
		int xP = InverseRasterScan(mbPartIdx, mb->MbPartWidth, mb->MbPartHeight, 16, 0);
		int yP = InverseRasterScan(mbPartIdx, mb->MbPartWidth, mb->MbPartHeight, 16, 1);

		int partWidth = 0;
		int partHeight = 0;

		int partWidthC = 0;
		int partHeightC = 0;

		int NumSubMbPart = 0;

		if (mb->mbType != H264_MB_TYPE::P_8x8
			&& mb->mbType != H264_MB_TYPE::P_8x8ref0
			&& mb->mbType != H264_MB_TYPE::B_Skip
			&& mb->mbType != H264_MB_TYPE::B_Direct_16x16
			&& mb->mbType != H264_MB_TYPE::B_8x8
			)
		{
			NumSubMbPart = 1;
			partWidth = mb->MbPartWidth;
			partHeight = mb->MbPartHeight;
		}
		else if (mb->mbType == H264_MB_TYPE::P_8x8
			|| mb->mbType == H264_MB_TYPE::P_8x8ref0
			|| (mb->mbType == H264_MB_TYPE::B_8x8 && mb->subMbType[mbPartIdx] != H264_MB_TYPE::B_Direct_8x8)
			)
		{
			NumSubMbPart = mb->NumSubMbPart[mbPartIdx]; //subMbPartIdx在0…NumSubMbPart( sub_mb_type ) − 1之间取值

			partWidth = mb->SubMbPartWidth[mbPartIdx];
			partHeight = mb->SubMbPartHeight[mbPartIdx];
		}
		else //B_Skip,B_Direct_16x16,B_Direct_8x8
		{
			NumSubMbPart = 4; //subMbPartIdx在0…3之间取值

			partWidth = 4;
			partHeight = 4;
		}


		if (sHeader.sps.ChromaArrayType != 0)
		{
			partWidthC = partWidth / sHeader.sps.SubWidthC;
			partHeightC = partHeight / sHeader.sps.SubHeightC;
		}

		for (size_t subMbPartIdx = 0; subMbPartIdx < NumSubMbPart; subMbPartIdx++)
		{
			//宏块分区subMbPartIdx的左上luma样本相对于子宏块左上样本的位置(x, y)。
			int xS = 0;
			int yS = 0;
			mb->SubMbPartWidth[mbPartIdx];
			mb->SubMbPartHeight[mbPartIdx];
			if (macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::P_8x8
				|| macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::P_8x8ref0
				|| macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::B_8x8
				)
			{
				xS = InverseRasterScan(subMbPartIdx, partWidth, partHeight, 8, 0);
				yS = InverseRasterScan(subMbPartIdx, partWidth, partHeight, 8, 1);
			}
			else
			{
				xS = InverseRasterScan(subMbPartIdx, 4, 4, 8, 0);
				yS = InverseRasterScan(subMbPartIdx, 4, 4, 8, 1);
			}



			int MvCnt = 0;


			int mvL0[2] = { 0 };
			int mvL1[2] = { 0 };
			int mvCL0[2] = { 0 };
			int mvCL1[2] = { 0 };
			int refIdxL0 = 0;
			int refIdxL1 = 0;
			int predFlagL0 = 0;
			int predFlagL1 = 0;
			//子宏块分割块的运动矢量个数
			int subMvCnt = 0;
			Derivation_process_for_motion_vector_components_and_reference_indices(dpb, mbPartIdx, subMbPartIdx,
				mvL0, mvL1, mvCL0, mvCL1, refIdxL0, refIdxL1, predFlagL0, predFlagL1, subMvCnt);


			MvCnt += subMvCnt;

			int logWDL = 0;
			int w0L = 1;
			int w1L = 1;
			int o0L = 0;
			int o1L = 0;
			int logWDCb = 0;
			int w0Cb = 1;
			int w1Cb = 1;
			int o0Cb = 0;
			int o1Cb = 0;
			int logWDCr = 0;
			int w0Cr = 1;
			int w1Cr = 1;
			int o0Cr = 0;
			int o1Cr = 0;
			//加权预测的过程
			if (sHeader.pps.weighted_pred_flag && ((sHeader.slice_type == 0) || (sHeader.slice_type == 3)) //H264_SLIECE_TYPE_P=0
				|| (sHeader.pps.weighted_bipred_idc > 0 && sHeader.slice_type == 1) //H264_SLIECE_TYPE_B=1
				)
			{
				Derivation_process_for_prediction_weights(dpb, refIdxL0, refIdxL1, predFlagL0, predFlagL1,
					logWDL, w0L, w1L, o0L, o1L, logWDCb, w0Cb, w1Cb, o0Cb, o1Cb, logWDCr, w0Cr, w1Cr, o0Cr, o1Cr);
			}

			//帧间预测样点的解码过程

			//当前分割块左上角样点与图像左上角亮度样点的相对位置
			const int xAL = xM + xP + xS;
			const int yAL = yM + yP + yS;

		}



	}


}

//帧间预测样点的解码过程
void ParseSlice::Decoding_process_for_Inter_prediction_samples(DPB& dpb, const int xAL, const int yAL, int mbPartIdx, int subMbPartIdx,
	int partWidth, int partHeight, int partWidthC, int partHeightC, int mvL0[2], int mvL1[2], int mvCL0[2], int mvCL1[2],
	int refIdxL0, int refIdxL1, int predFlagL0, int predFlagL1)
{
	uint8_t* predPartL = new uint8_t[partWidth * partHeight]();
	uint8_t* predPartCb = new uint8_t[partWidthC * partHeightC]();
	uint8_t* predPartCr = new uint8_t[partWidthC * partHeightC]();



	uint8_t* predPartL0L = new uint8_t[partWidth * partHeight]();
	uint8_t* predPartL1L = new uint8_t[partWidth * partHeight]();

	uint8_t* predPartL0Cb = new uint8_t[partWidthC * partHeightC]();
	uint8_t* predPartL1Cb = new uint8_t[partWidthC * partHeightC]();
	uint8_t* predPartL0Cr = new uint8_t[partWidthC * partHeightC]();
	uint8_t* predPartL1Cr = new uint8_t[partWidthC * partHeightC]();


	if (predFlagL0 == 1)
	{
		//参考图像选择过程
		//如果当前宏块为帧宏块
		Picture* refPic = dpb.RefPicList0[predFlagL0];


		Fractional_sample_interpolation_process(xAL, yAL, mbPartIdx, subMbPartIdx,
			partWidth, partHeight, partWidthC, partHeightC, mvL0, mvCL0, refPic, predPartL0L, predPartL0Cb, predPartL0Cr);

	}


}

//分数样本插值过程
void ParseSlice::Fractional_sample_interpolation_process(const int xAL, const int yAL, int mbPartIdx, int subMbPartIdx,
	int partWidth, int partHeight, int partWidthC, int partHeightC, int mvLX[2], int mvCLX[2], Picture* refPic,
	uint8_t* predPartLXL, uint8_t* predPartLXCb, uint8_t* predPartLXCr)
{


	//(xFracL, yFracL)为以四分之一样本单位给出的偏移量
	for (size_t yL = 0; yL < partHeight; yL++)
	{
		for (size_t xL = 0; xL < partWidth; xL++)
		{
			//整样点为单位给出的色度样点位置
			int xIntL = xAL + (mvLX[0] >> 2) + xL;
			int yIntL = yAL + (mvLX[1] >> 2) + yL;
			//1/4 样点为单位给出的偏移量
			int xFracL = mvLX[0] & 3;
			int yFracL = mvLX[1] & 3;

			predPartLXL[yL * partWidth + xL] = Luma_sample_interpolation_process(xIntL, yIntL, xFracL, yFracL, refPic);
		}
	}

	if (sHeader.sps.ChromaArrayType != 0)
	{
		//整样点为单位给出的色度样点位置
		int xIntC = 0;
		int yIntC = 0;
		//1/8 样点为单位给出的偏移量
		int xFracC = 0;
		int yFracC = 0;
		for (size_t yC = 0; yC < partHeightC; yC++)
		{
			for (size_t xC = 0; xC < partWidthC; xC++)
			{

				if (sHeader.sps.ChromaArrayType == 1)
				{
					xIntC = (xAL / sHeader.sps.SubWidthC) + (mvCLX[0] >> 3) + xC;
					yIntC = (yAL / sHeader.sps.SubHeightC) + (mvCLX[1] >> 3) + yC;
					xFracC = mvCLX[0] & 7;
					yFracC = mvCLX[1] & 7;
				}
				else if (sHeader.sps.ChromaArrayType == 2)
				{
					xIntC = (xAL / sHeader.sps.SubWidthC) + (mvCLX[0] >> 3) + xC;
					yIntC = (yAL / sHeader.sps.SubHeightC) + (mvCLX[1] >> 2) + yC;
					xFracC = mvCLX[0] & 7;
					yFracC = (mvCLX[1] & 3) << 1;
				}
				else//ChromaArrayType == 3
				{
					xIntC = xAL + (mvLX[0] >> 2) + xC;
					yIntC = yAL + (mvLX[1] >> 2) + yC;
					xFracC = (mvCLX[0] & 3);
					yFracC = (mvCLX[1] & 3);
				}

				if (sHeader.sps.ChromaArrayType != 3)
				{
					predPartLXCb[yC * partWidthC + xC] = Chroma_sample_interpolation_process(xIntC, yIntC, xFracC, yFracC, refPic, true);
					predPartLXCr[yC * partWidthC + xC] = Chroma_sample_interpolation_process(xIntC, yIntC, xFracC, yFracC, refPic, false);
				}
				else//ChromaArrayType is equal to 3
				{
					predPartLXCb[yC * partWidthC + xC] = Luma_sample_interpolation_process(xIntC, yIntC, xFracC, yFracC, refPic);
					predPartLXCr[yC * partWidthC + xC] = Luma_sample_interpolation_process(xIntC, yIntC, xFracC, yFracC, refPic);
				}
			}
		}
	}
}

//亮度样点插值过程
uint8_t ParseSlice::Luma_sample_interpolation_process(int xIntL, int yIntL, int xFracL, int yFracL, Picture* refPic)
{
	//如果MbaffFrameFlag 等于0 ，或者mb_field_decoding_flag 等于0 
	int refPicHeightEffectiveL = PicHeightInSamplesL;
	/*
		G点是原点(0,0)  x11是(-2,-2)  U点是(1,3)  x31是(-2,-2)

	   x11	 x12  A     aa    B   x13    x14



	   x21		  C     bb    D



		E    F    G  a  b  c  H    I    J
				  d  e  f  g
		cc   dd   h  i  j  k  m    ee   ff
				  n  p  q  r
		K    L    M     s  N  P    Q



	   x31	x32	  R     gg    S



				  T     hh    U

*/
#define getLumaSample(xDZL,yDZL) refPic->lumaData[Clip3(0, refPic->PicWidthInSamplesL - 1, xIntL+xDZL)][Clip3(0,refPicHeightEffectiveL - 1,yIntL+yDZL)]
	int A = getLumaSample(0, -2);
	int B = getLumaSample(1, -2);
	int C = getLumaSample(0, -1);
	int D = getLumaSample(1, -1);
	int E = getLumaSample(-2, 0);
	int F = getLumaSample(-1, 0);
	int G = getLumaSample(0, 0); //坐标原点
	int H = getLumaSample(1, 0);
	int I = getLumaSample(2, 0);
	int J = getLumaSample(3, 0);
	int K = getLumaSample(-2, 1);
	int L = getLumaSample(-1, 1);
	int M = getLumaSample(0, 1);
	int N = getLumaSample(1, 1);
	int P = getLumaSample(2, 1);
	int Q = getLumaSample(3, 1);
	int R = getLumaSample(0, 2);
	int S = getLumaSample(1, 2);
	int T = getLumaSample(0, 3);
	int U = getLumaSample(1, 3);

	int X11 = getLumaSample(-2, -2); //A所在的行与E所在的列的交点
	int X12 = getLumaSample(-1, -2);
	int X13 = getLumaSample(2, -2);

	int X14 = getLumaSample(3, -2);
	int X21 = getLumaSample(-2, -1);
	int X22 = getLumaSample(-1, -1);
	int X23 = getLumaSample(2, -1);
	int X24 = getLumaSample(3, -1);

	int X31 = getLumaSample(-2, 2);
	int X32 = getLumaSample(-1, 2);
	int X33 = getLumaSample(2, 2);
	int X34 = getLumaSample(3, 2);

	int X41 = getLumaSample(-2, 3);
	int X42 = getLumaSample(-1, 3);
	int X43 = getLumaSample(2, 3);
	int X44 = getLumaSample(3, 3);

	//（ 1, −5, 20, 20, −5, 1）
#define TapFilter(v1,v2,v3,v4,v5,v6) (v1 - 5*v2 + 20*v3 + 20*v4 - 5*v5 + v6)


#define Clip1Y(x) Clip3(0, (1<<sHeader.sps.BitDepthY)-1, x)
	int b1 = TapFilter(E, F, G, H, I, J);
	int h1 = TapFilter(A, C, G, M, R, T);
	int s1 = TapFilter(K, L, M, N, P, Q);
	int m1 = TapFilter(B, D, H, N, S, U);

	int b = Clip1Y((b1 + 16) >> 5);
	int h = Clip1Y((h1 + 16) >> 5);
	int s = Clip1Y((s1 + 16) >> 5);
	int m = Clip1Y((m1 + 16) >> 5);

	int aa = TapFilter(X11, X12, A, B, X13, X14);
	int bb = TapFilter(X21, X22, C, D, X23, X24);
	int gg = TapFilter(X31, X32, R, S, X33, X34);
	int hh = TapFilter(X41, X42, T, U, X43, X44);

	int cc = TapFilter(X11, X21, E, K, X31, X41);
	int dd = TapFilter(X12, X22, F, L, X32, X42);
	int ee = TapFilter(X13, X23, I, P, X33, X43);
	int ff = TapFilter(X14, X24, J, Q, X34, X44);

	int j1 = TapFilter(cc, dd, h1, m1, ee, ff);

	int j = Clip1Y((j1 + 512) >> 10);
	//计算两个最接近的整数和半整数位置的平均值
	int a = (G + b + 1) >> 1;
	int c = (H + b + 1) >> 1;
	int d = (G + h + 1) >> 1;
	int n = (M + h + 1) >> 1;
	int f = (b + j + 1) >> 1;
	int i = (h + j + 1) >> 1;
	int k = (j + m + 1) >> 1;
	int q = (j + s + 1) >> 1;
	//计算对角线方向上两个最接近的半整数位置的平均值
	int e = (b + h + 1) >> 1;
	int g = (b + m + 1) >> 1;
	int p = (h + s + 1) >> 1;
	int r = (m + s + 1) >> 1;

	//表 8-12
	int predPartLXL[4][4] = {
		{G, d, h, n},
		{a, e, i, p},
		{b, f, j, q},
		{c, g, k, r}
	};

#undef Clip1Y
#undef TapFilter
#undef getLumaSample

	return predPartLXL[xFracL][yFracL];
}

//色度样点插值过程
uint8_t ParseSlice::Chroma_sample_interpolation_process(int xIntC, int yIntC, int xFracC, int yFracC, Picture* refPic, bool isChromaCb)
{
	//如果MbaffFrameFlag 等于0 ， 或者mb_field_decoding_flag 等于0 
	int refPicHeightEffectiveC = PicHeightInSamplesC;


	int xAC = Clip3(0, refPic->PicWidthInSamplesC - 1, xIntC); // (8-262)
	int xBC = Clip3(0, refPic->PicWidthInSamplesC - 1, xIntC + 1); // (8-263)
	int xCC = Clip3(0, refPic->PicWidthInSamplesC - 1, xIntC); // (8-264)
	int xDC = Clip3(0, refPic->PicWidthInSamplesC - 1, xIntC + 1); // (8-265)

	int yAC = Clip3(0, refPicHeightEffectiveC - 1, yIntC); // (8-266)
	int yBC = Clip3(0, refPicHeightEffectiveC - 1, yIntC); // (8-267)
	int yCC = Clip3(0, refPicHeightEffectiveC - 1, yIntC + 1); // (8-268)
	int yDC = Clip3(0, refPicHeightEffectiveC - 1, yIntC + 1); // (8-269)

	uint8_t** buffer = isChromaCb ? refPic->chromaCbData : refPic->chromaCrData;

	uint8_t A = buffer[xAC][xBC];
	uint8_t B = buffer[xBC][yBC];
	uint8_t C = buffer[xCC][yCC];
	uint8_t D = buffer[xDC][yDC];

	return ((8 - xFracC) * (8 - yFracC) * A + xFracC * (8 - yFracC) * B + (8 - xFracC) * yFracC * C + xFracC * yFracC * D + 32) >> 6;
}


//加权预测的过程
void ParseSlice::Derivation_process_for_prediction_weights(DPB& dpb, int refIdxL0, int refIdxL1, int predFlagL0, int predFlagL1, int& logWDL, int& w0L, int& w1L, int& o0L, int& o1L, int& logWDCb, int& w0Cb, int& w1Cb, int& o0Cb, int& o1Cb, int& logWDCr, int& w0Cr, int& w1Cr, int& o0Cr, int& o1Cr)
{
	bool implicitModeFlag = false;
	bool explicitModeFlag = false;

	if (sHeader.pps.weighted_bipred_idc == 2 && sHeader.slice_type == 1 && predFlagL0 == 1 && predFlagL1 == 1)
	{
		implicitModeFlag = 1;
		explicitModeFlag = 0;
	}
	else if (sHeader.pps.weighted_bipred_idc == 1 && sHeader.slice_type == 1 && (predFlagL0 + predFlagL1 == 1 || predFlagL0 + predFlagL1 == 2))
	{
		implicitModeFlag = 0;
		explicitModeFlag = 1;
	}
	else if (sHeader.pps.weighted_pred_flag == 1 && (sHeader.slice_type == 0 || sHeader.slice_type == 3) && predFlagL0 == 1)
	{
		implicitModeFlag = 0;
		explicitModeFlag = 1;
	}
	else
	{
		implicitModeFlag = 0;
		explicitModeFlag = 0;
	}


	if (implicitModeFlag == 1)
	{
		//隐式模式加权预测
		logWDL = 5;
		o0L = 0;
		o1L = 0;
		if (sHeader.sps.ChromaArrayType != 0)
		{
			logWDCb = 5;
			o0Cb = 0;
			o1Cb = 0;

			logWDCr = 5;
			o0Cr = 0;
			o1Cr = 0;
		}

		//field_pic_flag is equal to 1 or the current macroblock is a frame macroblock
		Picture* currPicOrField = new Picture(this, sHeader);
		Picture* pic0 = dpb.RefPicList1[refIdxL1];
		Picture* pic1 = dpb.RefPicList0[refIdxL0];


		int tb = Clip3(-128, 127, DiffPicOrderCnt(currPicOrField, pic0)); //(8-201)
		int td = Clip3(-128, 127, DiffPicOrderCnt(pic1, pic0)); //(8-202)
		int tx = (16384 + std::abs(td / 2)) / td; //(8-197)
		int DistScaleFactor = Clip3(-1024, 1023, (tb * tx + 32) >> 6); //(8-198)

		if (DiffPicOrderCnt(pic1, pic0) == 0
			|| pic0->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE
			|| pic1->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE
			|| (DistScaleFactor >> 2) < -64
			|| (DistScaleFactor >> 2) > 128)
		{
			w0L = 32;
			w1L = 32;

			if (sHeader.sps.ChromaArrayType != 0)
			{
				w0Cb = 32;
				w1Cb = 32;
				w0Cr = 32;
				w1Cr = 32;
			}
		}
		else
		{
			w0L = 64 - (DistScaleFactor >> 2);
			w1L = DistScaleFactor >> 2;

			if (sHeader.sps.ChromaArrayType != 0)
			{
				w0Cb = 64 - (DistScaleFactor >> 2);
				w1Cb = DistScaleFactor >> 2;
				w0Cr = 64 - (DistScaleFactor >> 2);
				w1Cr = DistScaleFactor >> 2;
			}
		}

		delete currPicOrField;
		currPicOrField = nullptr;
	}
	else if (explicitModeFlag == 1)
	{
		//显式模式加权预测

		//MbaffFrameFlag is equal to 0 or the current macroblock is a frame macroblock
		int refIdxL0WP = refIdxL0;
		int refIdxL1WP = refIdxL1;
		//如果是亮度样点
		logWDL = sHeader.luma_log2_weight_denom;
		w0L = sHeader.luma_weight_l0[refIdxL0WP];
		w1L = sHeader.luma_weight_l1[refIdxL1WP];
		o0L = sHeader.luma_offset_l0[refIdxL0WP] * (1 << (sHeader.sps.BitDepthY - 8));
		o1L = sHeader.luma_offset_l1[refIdxL1WP] * (1 << (sHeader.sps.BitDepthY - 8));

		//色度
		if (sHeader.sps.ChromaArrayType != 0)
		{
			logWDCb = sHeader.chroma_log2_weight_denom;
			w0Cb = sHeader.chroma_weight_l0[refIdxL0WP][0];
			w1Cb = sHeader.chroma_weight_l1[refIdxL1WP][0];
			o0Cb = sHeader.chroma_offset_l0[refIdxL0WP][0] * (1 << (sHeader.sps.BitDepthC - 8));
			o1Cb = sHeader.chroma_offset_l1[refIdxL1WP][0] * (1 << (sHeader.sps.BitDepthC - 8));

			logWDCr = sHeader.chroma_log2_weight_denom;
			w0Cr = sHeader.chroma_weight_l0[refIdxL0WP][1];
			w1Cr = sHeader.chroma_weight_l1[refIdxL1WP][1];
			o0Cr = sHeader.chroma_offset_l0[refIdxL0WP][1] * (1 << (sHeader.sps.BitDepthC - 8));
			o1Cr = sHeader.chroma_offset_l1[refIdxL1WP][1] * (1 << (sHeader.sps.BitDepthC - 8));
		}
	}
	else//implicitModeFlag is equal to 0 and explicitModeFlag is equal to 0
	{
		//变量logWDC, w0C, w1C, o0C, o1C不在当前宏块的重构过程中使用。  
	}

	if (explicitModeFlag == 1 && predFlagL0 == 1 && predFlagL1 == 1)
	{
		//−128 <= w0C + w1C <= ( ( logWDC == 7 ) ? 127 : 128 )
		if (!(-128 <= w0L + w1L && w0L + w1L <= ((logWDL == 7) ? 127 : 128)))
		{
			printError("w0L + w1L必须大于等于-128，小于等于127，或者128");
		}

		if (sHeader.sps.ChromaArrayType != 0)
		{
			if (!(-128 <= w0Cb + w1Cb && w0Cb + w1Cb <= ((logWDCb == 7) ? 127 : 128)))
			{
				printError("w0L + w1L必须大于等于-128，小于等于127，或者128");
			}
			if (!(-128 <= w0Cr + w1Cr && w0Cb + w1Cr <= ((logWDCr == 7) ? 127 : 128)))
			{
				printError("w0L + w1L必须大于等于-128，小于等于127，或者128");
			}
		}
	}
}

//运动矢量分量和参考指标的推导过程
void ParseSlice::Derivation_process_for_motion_vector_components_and_reference_indices(DPB& dpb, int mbPartIdx, int subMbPartIdx,
	int mvL0[2], int mvL1[2], int mvCL0[2], int mvCL1[2], int& refIdxL0, int& refIdxL1, int& predFlagL0, int& predFlagL1, int& subMvCnt)
{
	int mbAddrA = NA;
	int mbAddrB = NA;
	int mbAddrC = NA;

	int mvL0A[2] = { 0 };
	int mvL0B[2] = { 0 };
	int mvL0C[2] = { 0 };
	int refIdxL0A = 0;
	int refIdxL0B = 0;
	int refIdxL0C = 0;

	H264_MB_TYPE currSubMbType = H264_MB_TYPE::NA;

	const H264_MB_TYPE mbType = macroblock[CurrMbAddr]->mbType;
	if (mbType == H264_MB_TYPE::P_Skip)
	{

		refIdxL0 = 0;
		//P和SP条带中跳过宏块的亮度运动矢量推导过程

		Derivation_process_for_motion_data_of_neighbouring_partitions(mbPartIdx, subMbPartIdx, currSubMbType, false,
			mbAddrA, mbAddrB, mbAddrC, mvL0A, mvL0B, mvL0C, refIdxL0A, refIdxL0B, refIdxL0C);

		//亮度运动矢量mvL0和参考索引refIdxL0
		if (mbAddrA == NA
			|| mbAddrB == NA
			|| (refIdxL0A == 0 && mvL0A[0] == 0 && mvL0A[1] == 0)
			|| (refIdxL0B == 0 && mvL0B[0] == 0 && mvL0B[1] == 0)
			)
		{
			mvL0[0] = 0;
			mvL0[1] = 0;
		}
		else
		{
			//mbPartIdx = 0, subMbPartIdx = 0, refIdxL0和currSubMbType = "na" 作为输入
			Derivation_process_for_luma_motion_vector_prediction(mbPartIdx, subMbPartIdx, currSubMbType, false, refIdxL0, mvL0);
		}

		predFlagL0 = 1;
		predFlagL1 = 0;
		mvL1[0] = NA;
		mvL1[1] = NA;
		subMvCnt = 1;
	}
	else if (mbType == H264_MB_TYPE::B_Skip || mbType == H264_MB_TYPE::B_Direct_16x16 || macroblock[CurrMbAddr]->subMbType[mbPartIdx] == H264_MB_TYPE::B_Direct_8x8)
	{
		//B条带中B_Skip, B_Direct_16x16和B_Direct_8x8宏块的亮度运动矢量导出过程
		Derivation_process_for_luma_motion_vectors_for_B_Skip_or_B_Direct_16x16_or_B_Direct_8x8(dpb, mbPartIdx, subMbPartIdx,
			refIdxL0, refIdxL1, mvL0, mvL1, predFlagL0, predFlagL1, subMvCnt);
	}
	else
	{
		int mvpL0[2] = { 0 };
		int mvpL1[2] = { 0 };
		H264_MB_PART_PRED_MODE mode = Macroblock::getMbPartPredMode(macroblock[CurrMbAddr]->fix_slice_type, macroblock[CurrMbAddr]->fix_mb_type, macroblock[CurrMbAddr]->transform_size_8x8_flag, mbPartIdx);
		H264_MB_PART_PRED_MODE subMode = macroblock[CurrMbAddr]->subMode[mbPartIdx];


		//Pred_L0表示使用列表0预测调用帧间预测过程。Pred_L0是一种帧间宏块预测模式
		if (mode == H264_MB_PART_PRED_MODE::Pred_L0 || mode == H264_MB_PART_PRED_MODE::BiPred
			|| subMode == H264_MB_PART_PRED_MODE::Pred_L0 || subMode == H264_MB_PART_PRED_MODE::BiPred
			)
		{
			refIdxL0 = macroblock[CurrMbAddr]->ref_idx_l0[mbPartIdx];
			predFlagL0 = 1;
		}
		else
		{
			refIdxL0 = -1;
			predFlagL0 = 0;
		}

		if (mode == H264_MB_PART_PRED_MODE::Pred_L1 || mode == H264_MB_PART_PRED_MODE::BiPred
			|| subMode == H264_MB_PART_PRED_MODE::Pred_L1 || subMode == H264_MB_PART_PRED_MODE::BiPred
			)
		{
			refIdxL1 = macroblock[CurrMbAddr]->ref_idx_l1[mbPartIdx];
			predFlagL1 = 1;
		}
		else
		{
			refIdxL1 = -1;
			predFlagL1 = 0;
		}

		subMvCnt = predFlagL0 + predFlagL1;


		if (macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::B_8x8)
		{
			currSubMbType = macroblock[CurrMbAddr]->subMbType[mbPartIdx];
		}
		else
		{
			currSubMbType = H264_MB_TYPE::NA;
		}

		if (predFlagL0 == 1)
		{
			//输出为运动矢量 mvLX（X 为0 或  1）的预测值  mvpLX。
			Derivation_process_for_luma_motion_vector_prediction(mbPartIdx, subMbPartIdx, currSubMbType, false, refIdxL0, mvpL0);

			mvL0[0] = mvpL0[0] + macroblock[CurrMbAddr]->mvd_l0[mbPartIdx][subMbPartIdx][0];
			mvL0[1] = mvpL0[1] + macroblock[CurrMbAddr]->mvd_l0[mbPartIdx][subMbPartIdx][1];
		}

		if (predFlagL1 == 1)
		{
			//输出为运动矢量 mvLX（X 为0 或  1）的预测值  mvpLX。
			Derivation_process_for_luma_motion_vector_prediction(mbPartIdx, subMbPartIdx, currSubMbType, true, refIdxL1, mvpL1);

			mvL1[0] = mvpL1[0] + macroblock[CurrMbAddr]->mvd_l1[mbPartIdx][subMbPartIdx][0];
			mvL1[1] = mvpL1[1] + macroblock[CurrMbAddr]->mvd_l1[mbPartIdx][subMbPartIdx][1];
		}
	}

	if (sHeader.sps.ChromaArrayType != 0)
	{
		//色度运动矢量的推导过程
		if (predFlagL0 == 1)
		{
			//如果ChromaArrayType不等于1，或者当前宏块为帧宏块
			mvCL0[0] = mvL0[0];
			mvCL0[1] = mvL0[1];

		}

		if (predFlagL1 == 1)
		{
			//如果ChromaArrayType不等于1，或者当前宏块为帧宏块
			mvCL1[0] = mvL1[0];
			mvCL1[1] = mvL1[1];
		}
	}
}

//亮度运动矢量预测值的推导过程

//对于一个帧间编码宏块，最多可以分割成16个4×4像素大小的子块来进行运动估计。
//每一个子块都按照实际的运动矢量进行编码和传输需要较多的比特数。为了提升编码的效率，H.264中定义了运动矢量预测的方法。
//每一个子块的运动矢量MV由计算得到的预测矢量MVP和运动矢量残差MVD得到。其中，MVD从码流中相应的语法元素解析得到，MVP由相邻像素块的信息计算得到
void ParseSlice::Derivation_process_for_luma_motion_vector_prediction(int mbPartIdx, int subMbPartIdx, H264_MB_TYPE currSubMbType, bool listSuffixFlag, int refIdxLX, int mvpLX[2])
{

	int mbAddrA = NA;
	int mbAddrB = NA;
	int mbAddrC = NA;

	int mvLXA[2] = { 0 };
	int mvLXB[2] = { 0 };
	int mvLXC[2] = { 0 };
	int refIdxLXA = 0;
	int refIdxLXB = 0;
	int refIdxLXC = 0;


	Derivation_process_for_motion_data_of_neighbouring_partitions(mbPartIdx, subMbPartIdx, currSubMbType, listSuffixFlag,
		mbAddrA, mbAddrB, mbAddrC, mvLXA, mvLXB, mvLXC, refIdxLXA, refIdxLXB, refIdxLXC);


	int MbPartWidth = macroblock[CurrMbAddr]->MbPartWidth;
	int MbPartHeight = macroblock[CurrMbAddr]->MbPartHeight;

	if (MbPartWidth == 16 && MbPartHeight == 8 && mbPartIdx == 0 && refIdxLXB == refIdxLX)
	{
		mvpLX[0] = mvLXB[0];
		mvpLX[1] = mvLXB[1];
	}
	else if (MbPartWidth == 16 && MbPartHeight == 8 && mbPartIdx == 1 && refIdxLXA == refIdxLX)
	{
		mvpLX[0] = mvLXA[0];
		mvpLX[1] = mvLXA[1];
	}
	else if (MbPartWidth == 8 && MbPartHeight == 16 && mbPartIdx == 0 && refIdxLXA == refIdxLX)
	{
		mvpLX[0] = mvLXA[0];
		mvpLX[1] = mvLXA[1];
	}
	else if (MbPartWidth == 8 && MbPartHeight == 16 && mbPartIdx == 1 && refIdxLXC == refIdxLX)
	{
		mvpLX[0] = mvLXC[0];
		mvpLX[1] = mvLXC[1];
	}
	else
	{
		//中值亮度运动矢量预测值的推导过程
		if (mbAddrB == NA && mbAddrC == NA && mbAddrA != NA)
		{
			mvLXB[0] = mvLXA[0];
			mvLXB[1] = mvLXA[1];
			mvLXC[0] = mvLXA[0];
			mvLXC[1] = mvLXA[1];
			refIdxLXB = refIdxLXA;
			refIdxLXC = refIdxLXA;
		}

		if (refIdxLXA == refIdxLX && refIdxLXB != refIdxLX && refIdxLXC != refIdxLX)
		{
			mvpLX[0] = mvLXA[0];
			mvpLX[1] = mvLXA[1];
		}
		else if (refIdxLXA != refIdxLX && refIdxLXB == refIdxLX && refIdxLXC != refIdxLX)
		{
			mvpLX[0] = mvLXB[0];
			mvpLX[1] = mvLXB[1];
		}
		else if (refIdxLXA != refIdxLX && refIdxLXB != refIdxLX && refIdxLXC == refIdxLX)
		{
			mvpLX[0] = mvLXC[0];
			mvpLX[1] = mvLXC[1];
		}
		else
		{
			mvpLX[0] = Median(mvLXA[0], mvLXB[0], mvLXC[0]);
			mvpLX[1] = Median(mvLXA[1], mvLXB[1], mvLXC[1]);
		}
	}
}

//色度运动矢量的推导过程
void ParseSlice::Derivation_process_for_chroma_motion_vectors(int mvLX[2], int mvCLX[2])
{
	//如果ChromaArrayType不等于1，或者当前宏块为帧宏块

	mvCLX[0] = mvLX[0];
	mvCLX[1] = mvLX[1];

}

////B条带中B_Skip, B_Direct_16x16和B_Direct_8x8宏块的亮度运动矢量导出过程
void ParseSlice::Derivation_process_for_luma_motion_vectors_for_B_Skip_or_B_Direct_16x16_or_B_Direct_8x8(DPB& dpb, int mbPartIdx, int subMbPartIdx,
	int& refIdxL0, int& refIdxL1, int mvL0[2], int mvL1[2], int& predFlagL0, int& predFlagL1, int& subMvCnt)
{
	if (sHeader.direct_spatial_mv_pred_flag)
	{

		//空域直接模式下亮度运动矢量和参考索引的推导过程
		Derivation_process_for_spatial_direct_luma_motion_vector_and_reference_index_prediction_mode(dpb, mbPartIdx, subMbPartIdx,
			refIdxL0, refIdxL1, mvL0, mvL1, predFlagL0, predFlagL1, subMvCnt);
	}
	else
	{
		//时域直接亮度运动矢量的推导过程及参考指标预测模式  
		Derivation_process_for_temporal_direct_luma_motion_vector_and_reference_index_prediction_mode(dpb, mbPartIdx, subMbPartIdx,
			refIdxL0, refIdxL1, mvL0, mvL1, predFlagL0, predFlagL1);

		if (subMbPartIdx == 0)
		{
			subMvCnt = 2;
		}
		else
		{
			subMvCnt = 0;
		}

	}
}

//空域直接模式下亮度运动矢量和参考索引的推导过程
void ParseSlice::Derivation_process_for_spatial_direct_luma_motion_vector_and_reference_index_prediction_mode(DPB& dpb, int mbPartIdx, int subMbPartIdx,
	int& refIdxL0, int& refIdxL1, int mvL0[2], int mvL1[2], int& predFlagL0, int& predFlagL1, int& subMvCnt)
{
	const H264_MB_TYPE currSubMbType = macroblock[CurrMbAddr]->subMbType[mbPartIdx];

	int mbAddrA = NA;
	int mbAddrB = NA;
	int mbAddrC = NA;

	int mvL0A[2] = { 0 };
	int mvL0B[2] = { 0 };
	int mvL0C[2] = { 0 };
	int refIdxL0A = 0;
	int refIdxL0B = 0;
	int refIdxL0C = 0;
	Derivation_process_for_motion_data_of_neighbouring_partitions(mbPartIdx, subMbPartIdx, currSubMbType, false,
		mbAddrA, mbAddrB, mbAddrC, mvL0A, mvL0B, mvL0C, refIdxL0A, refIdxL0B, refIdxL0C);


	mbAddrA = NA;
	mbAddrB = NA;
	mbAddrC = NA;

	int mvL1A[2] = { 0 };
	int mvL1B[2] = { 0 };
	int mvL1C[2] = { 0 };

	int refIdxL1A = 0;
	int refIdxL1B = 0;
	int refIdxL1C = 0;
	Derivation_process_for_motion_data_of_neighbouring_partitions(mbPartIdx, subMbPartIdx, currSubMbType, true,
		mbAddrA, mbAddrB, mbAddrC, mvL1A, mvL1B, mvL1C, refIdxL1A, refIdxL1B, refIdxL1C);


#define MinPositive(x,y) ((x) >= 0 && (y) >= 0) ? (std::min(x, y)) : (std::max(x, y))

	refIdxL0 = MinPositive(refIdxL0A, MinPositive(refIdxL0B, refIdxL0C));
	refIdxL1 = MinPositive(refIdxL1A, MinPositive(refIdxL1B, refIdxL1C));

#undef MinPositive

	bool directZeroPredictionFlag = false;
	if (refIdxL0 < 0 && refIdxL1 < 0)
	{
		refIdxL0 = 0;
		refIdxL1 = 0;
		directZeroPredictionFlag = true;
	}

	int refIdxCol = 0;
	int mvCol[2] = { 0 };
	Picture* colPic = nullptr;
	int mbAddrCol = 0;

	Derivation_process_for_the_co_located_4x4_sub_macroblock_partitions(dpb, mbPartIdx, subMbPartIdx,
		colPic, mbAddrCol, mvCol, refIdxCol);

	bool colZeroFlag = false;
	//mvCol[ 0 ]和mvCol[ 1 ]的单位等于1/4亮度帧样点的单位
	if (dpb.RefPicList1[0]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE
		&& refIdxCol == 0
		&& (mvCol[0] >= -1 && mvCol[0] <= 1) && (mvCol[1] >= -1 && mvCol[1] <= 1)
		)
	{
		colZeroFlag = true;
	}
	else
	{
		colZeroFlag = false;
	}


	if (directZeroPredictionFlag || refIdxL0 < 0 || (refIdxL0 == 0 && colZeroFlag))
	{
		mvL0[0] = 0;
		mvL0[1] = 0;
	}
	else
	{
		mbPartIdx = 0;
		subMbPartIdx = 0;
		Derivation_process_for_luma_motion_vector_prediction(mbPartIdx, subMbPartIdx, currSubMbType, false, refIdxL0, mvL0);
	}

	if (directZeroPredictionFlag || refIdxL1 < 0 || (refIdxL1 == 0 && colZeroFlag))
	{
		mvL1[0] = 0;
		mvL1[1] = 0;
	}
	else
	{
		mbPartIdx = 0;
		subMbPartIdx = 0;
		Derivation_process_for_luma_motion_vector_prediction(mbPartIdx, subMbPartIdx, currSubMbType, true, refIdxL1, mvL1);
	}


	if (refIdxL0 >= 0 && refIdxL1 >= 0)
	{
		predFlagL0 = 1;
		predFlagL1 = 1;
	}
	else if (refIdxL0 >= 0 && refIdxL1 < 0)
	{
		predFlagL0 = 1;
		predFlagL1 = 0;
	}
	else if (refIdxL0 < 0 && refIdxL1 >= 0)
	{
		predFlagL0 = 0;
		predFlagL1 = 1;
	}

	if (subMbPartIdx != 0)
	{
		subMvCnt = 0;
	}
	else
	{
		subMvCnt = predFlagL0 + predFlagL1;
	}
}

//时域直接亮度运动矢量的推导过程及参考指标预测模式  
void ParseSlice::Derivation_process_for_temporal_direct_luma_motion_vector_and_reference_index_prediction_mode(DPB& dpb, int mbPartIdx, int subMbPartIdx,
	int& refIdxL0, int& refIdxL1, int mvL0[2], int mvL1[2], int& predFlagL0, int& predFlagL1)
{
	int refIdxCol = 0;
	int mvCol[2] = { 0 };
	Picture* colPic = nullptr;
	int mbAddrCol = 0;
	Derivation_process_for_the_co_located_4x4_sub_macroblock_partitions(dpb, mbPartIdx, subMbPartIdx,
		colPic, mbAddrCol, mvCol, refIdxCol);

	//MapColToList0( refIdxCol )

	//令refIdxL0Frm为当前参考图像列表RefPicList0中的最小索引值
	size_t refIdxL0Frm = 0;
	for (size_t i = 0; i < dpb.RefPicList0.length; i++)
	{
		if (dpb.RefPicList0[i] == colPic)
		{
			refIdxL0Frm = i;
			break;
		}
	}
	refIdxL0 = ((refIdxCol < 0) ? 0 : refIdxL0Frm);
	refIdxL1 = 0;

	//vertMvScale 等于 One_To_One）， mvCol[ 1 ]保持不变

	//field_pic_flag等于1，或者当前宏块为帧宏块
	Picture* currPicOrField = new Picture(this, sHeader);
	Picture* pic0 = dpb.RefPicList0[refIdxL0];
	Picture* pic1 = dpb.RefPicList1[0];


	//当前宏块中每个 4×4 子宏块分割块的两个运动矢量 mvL0 和 mvL1 推导过程

	if (dpb.RefPicList0[refIdxL0]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE
		|| DiffPicOrderCnt(pic1, pic0) == 0
		)
	{
		mvL0[0] = mvCol[0];
		mvL0[1] = mvCol[1];

		mvL1[0] = 0;
		mvL1[1] = 0;
	}
	else
	{
		int tb = Clip3(-128, 127, DiffPicOrderCnt(currPicOrField, pic0));
		int td = Clip3(-128, 127, DiffPicOrderCnt(pic1, pic0));
		int tx = (16384 + std::abs(td / 2)) / td;

		int DistScaleFactor = Clip3(-1024, 1023, (tb * tx + 32) >> 6);

		mvL0[0] = (DistScaleFactor * mvCol[0] + 128) >> 8;
		mvL0[1] = (DistScaleFactor * mvCol[1] + 128) >> 8;
		mvL1[0] = mvL0[0] - mvCol[0];
		mvL1[1] = mvL0[1] - mvCol[1];
	}

	predFlagL0 = 1;
	predFlagL1 = 1;

	delete currPicOrField;
	currPicOrField = nullptr;
}

//共同位置4x4子宏块分割块的推导过程
void ParseSlice::Derivation_process_for_the_co_located_4x4_sub_macroblock_partitions(DPB& dpb, int mbPartIdx, int subMbPartIdx,
	Picture*& colPic, int& mbAddrCol, int mvCol[2], int& refIdxCol)
{

	//表 8-6－变量colPic的规范
	colPic = dpb.RefPicList1[0];

	int luma4x4BlkIdx = 0;

	if (sHeader.sps.direct_8x8_inference_flag)
	{
		luma4x4BlkIdx = 5 * mbPartIdx;
	}
	else
	{
		luma4x4BlkIdx = (4 * mbPartIdx + subMbPartIdx);
	}

	int xCol = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 0);
	int yCol = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 1);

	//根据表 8-7 表 8-8－mbAddrCol, yM和vertMvScale取值的规范
	mbAddrCol = CurrMbAddr;
	int yM = yCol;
	//vertMvScale = H264_VERT_MV_SCALE_One_To_One;


	/*H264_MB_TYPE mbTypeCol = macroblock[mbAddrCol]->mbType;
	H264_MB_TYPE subMbTypeCol[4] = { H264_MB_TYPE::NA, H264_MB_TYPE::NA, H264_MB_TYPE::NA, H264_MB_TYPE::NA };

	if (mbTypeCol == H264_MB_TYPE::P_8x8 || mbTypeCol == H264_MB_TYPE::P_8x8ref0 || mbTypeCol == H264_MB_TYPE::B_8x8)
	{
		subMbTypeCol[0] = colPic->macroblock[mbAddrCol]->subMbType[0];
		subMbTypeCol[1] = colPic->macroblock[mbAddrCol]->subMbType[1];
		subMbTypeCol[2] = colPic->macroblock[mbAddrCol]->subMbType[2];
		subMbTypeCol[3] = colPic->macroblock[mbAddrCol]->subMbType[3];
	}*/


	int mbPartIdxCol = 0;
	int subMbPartIdxCol = 0;
	ParseSlice::Derivation_process_for_macroblock_and_sub_macroblock_partition_indices(macroblock[mbAddrCol], xCol, yM, mbPartIdxCol, subMbPartIdxCol);

	if (isInterMode(macroblock[mbAddrCol]->mode))
	{
		mvCol[0] = 0;
		mvCol[1] = 0;
		refIdxCol = -1;
	}
	else
	{
		int predFlagL0Col = colPic->macroblock[mbAddrCol]->predFlagL0[mbPartIdxCol];
		int predFlagL1Col = colPic->macroblock[mbAddrCol]->predFlagL1[mbPartIdxCol];

		if (predFlagL0Col == 1)
		{
			mvCol[0] = colPic->macroblock[mbAddrCol]->mvL0[mbPartIdxCol][subMbPartIdxCol][0]; //MvL0[ mbPartIdxCol ][ subMbPartIdxCol ]
			mvCol[1] = colPic->macroblock[mbAddrCol]->mvL0[mbPartIdxCol][subMbPartIdxCol][1];
			refIdxCol = colPic->macroblock[mbAddrCol]->refIdxL0[mbPartIdxCol];
		}
		else//predFlagL0Col is equal to 0 and predFlagL1Col is equal to 1
		{
			mvCol[0] = colPic->macroblock[mbAddrCol]->mvL1[mbPartIdxCol][subMbPartIdxCol][0]; //MvL1[ mbPartIdxCol ][ subMbPartIdxCol ]
			mvCol[1] = colPic->macroblock[mbAddrCol]->mvL1[mbPartIdxCol][subMbPartIdxCol][1];
			refIdxCol = colPic->macroblock[mbAddrCol]->refIdxL1[mbPartIdxCol];
		}
	}

}

//8.4.1.3.2相邻分区运动数据的推导过程  
void ParseSlice::Derivation_process_for_motion_data_of_neighbouring_partitions(int mbPartIdx, int subMbPartIdx, H264_MB_TYPE currSubMbType, bool listSuffixFlag,
	int& mbAddrA, int& mbAddrB, int& mbAddrC, int mvLXA[2], int mvLXB[2], int mvLXC[2], int& refIdxLXA, int& refIdxLXB, int& refIdxLXC)
{

	/*
	   |		|
	 D |  B		| C
	---|--------|---
	   |当前宏块	|
	 A |或分割块	|
	   |		|

	*/
	/*int mbAddrA = NA;
	int mbAddrB = NA;
	int mbAddrC = NA;*/
	int mbAddrD = NA;

	int mbPartIdxA = 0;
	int mbPartIdxB = 0;
	int mbPartIdxC = 0;
	int mbPartIdxD = 0;

	int subMbPartIdxA = 0;
	int subMbPartIdxB = 0;
	int subMbPartIdxC = 0;
	int subMbPartIdxD = 0;


	Derivation_process_for_neighbouring_partitions(mbPartIdx, subMbPartIdx, currSubMbType,
		mbAddrA, mbAddrB, mbAddrC, mbAddrD,
		mbPartIdxA, mbPartIdxB, mbPartIdxC, mbPartIdxD,
		subMbPartIdxA, subMbPartIdxB, subMbPartIdxC, subMbPartIdxD
	);
	if (mbAddrC == NA && mbPartIdxC == NA && subMbPartIdxC == NA)
	{
		mbAddrC = mbAddrD;
		mbPartIdxC = mbPartIdxD;
		subMbPartIdxC = subMbPartIdxD;
	}

	if (mbAddrA == NA
		|| isInterMode(macroblock[mbAddrA]->mode)
		|| (listSuffixFlag == false && macroblock[mbAddrA]->predFlagL0[mbPartIdxA] == 0)
		|| (listSuffixFlag == true && macroblock[mbAddrA]->predFlagL1[mbPartIdxA] == 0)
		)
	{
		mvLXA[0] = 0;
		mvLXA[1] = 0;
		refIdxLXA = -1;
	}
	else
	{

		if (listSuffixFlag)
		{
			mvLXA[0] = macroblock[mbAddrA]->mvL1[mbPartIdxA][subMbPartIdxA][0];
			mvLXA[1] = macroblock[mbAddrA]->mvL1[mbPartIdxA][subMbPartIdxA][1];
			refIdxLXA = macroblock[mbAddrA]->refIdxL1[mbPartIdxA];
		}
		else
		{
			mvLXA[0] = macroblock[mbAddrA]->mvL0[mbPartIdxA][subMbPartIdxA][0];
			mvLXA[1] = macroblock[mbAddrA]->mvL0[mbPartIdxA][subMbPartIdxA][1];
			refIdxLXA = macroblock[mbAddrA]->refIdxL0[mbPartIdxA];
		}


		//变量mvLXN[ 1 ]和refIdxLXN进一步处理如下
		//如果当前宏块为场宏块，且宏块mbAddrN为帧宏块.mvLXN[ 1 ] = mvLXN[ 1 ] / 2 ,refIdxLXN = refIdxLXN * 2 
		//如果当前宏块为帧宏块且宏块mbAddrN为场宏块.mvLXN[ 1 ] = mvLXN[ 1 ] * 2,refIdxLXN = refIdxLXN / 2
	}




	if (mbAddrB == NA
		|| isInterMode(macroblock[mbAddrB]->mode)
		|| (listSuffixFlag == false && macroblock[mbAddrB]->predFlagL0[mbPartIdxB] == 0)
		|| (listSuffixFlag == true && macroblock[mbAddrB]->predFlagL1[mbPartIdxB] == 0)
		)
	{
		mvLXB[0] = 0;
		mvLXB[1] = 0;
		refIdxLXB = -1;
	}
	else
	{

		if (listSuffixFlag)
		{
			mvLXB[0] = macroblock[mbAddrB]->mvL1[mbPartIdxB][subMbPartIdxB][0];
			mvLXB[1] = macroblock[mbAddrB]->mvL1[mbPartIdxB][subMbPartIdxB][1];
			refIdxLXB = macroblock[mbAddrB]->refIdxL1[mbPartIdxB];
		}
		else
		{
			mvLXB[0] = macroblock[mbAddrB]->mvL0[mbPartIdxB][subMbPartIdxB][0];
			mvLXB[1] = macroblock[mbAddrB]->mvL0[mbPartIdxB][subMbPartIdxB][1];
			refIdxLXB = macroblock[mbAddrB]->refIdxL0[mbPartIdxB];
		}


		//变量mvLXN[ 1 ]和refIdxLXN进一步处理如下
		//如果当前宏块为场宏块，且宏块mbAddrN为帧宏块.mvLXN[ 1 ] = mvLXN[ 1 ] / 2 ,refIdxLXN = refIdxLXN * 2 
		//如果当前宏块为帧宏块且宏块mbAddrN为场宏块.mvLXN[ 1 ] = mvLXN[ 1 ] * 2,refIdxLXN = refIdxLXN / 2
	}



	if (mbAddrC == NA
		|| isInterMode(macroblock[mbAddrC]->mode)
		|| (listSuffixFlag == false && macroblock[mbAddrC]->predFlagL0[mbPartIdxC] == 0)
		|| (listSuffixFlag == true && macroblock[mbAddrC]->predFlagL1[mbPartIdxC] == 0)
		)
	{
		mvLXC[0] = 0;
		mvLXC[1] = 0;
		refIdxLXC = -1;
	}
	else
	{

		if (listSuffixFlag)
		{
			mvLXC[0] = macroblock[mbAddrC]->mvL1[mbPartIdxC][subMbPartIdxC][0];
			mvLXC[1] = macroblock[mbAddrC]->mvL1[mbPartIdxC][subMbPartIdxC][1];
			refIdxLXC = macroblock[mbAddrC]->refIdxL1[mbPartIdxC];
		}
		else
		{
			mvLXC[0] = macroblock[mbAddrC]->mvL0[mbPartIdxC][subMbPartIdxC][0];
			mvLXC[1] = macroblock[mbAddrC]->mvL0[mbPartIdxC][subMbPartIdxC][1];
			refIdxLXC = macroblock[mbAddrC]->refIdxL0[mbPartIdxC];
		}


		//变量mvLXN[ 1 ]和refIdxLXN进一步处理如下
		//如果当前宏块为场宏块，且宏块mbAddrN为帧宏块.mvLXN[ 1 ] = mvLXN[ 1 ] / 2 ,refIdxLXN = refIdxLXN * 2 
		//如果当前宏块为帧宏块且宏块mbAddrN为场宏块.mvLXN[ 1 ] = mvLXN[ 1 ] * 2,refIdxLXN = refIdxLXN / 2
	}

}

//相邻分割块的推导过程
void ParseSlice::Derivation_process_for_neighbouring_partitions(int mbPartIdx, int subMbPartIdx, H264_MB_TYPE currSubMbType,
	int& mbAddrA, int& mbAddrB, int& mbAddrC, int& mbAddrD, int& mbPartIdxA, int& mbPartIdxB, int& mbPartIdxC, int& mbPartIdxD,
	int& subMbPartIdxA, int& subMbPartIdxB, int& subMbPartIdxC, int& subMbPartIdxD)
{


	int MbPartWidth = macroblock[CurrMbAddr]->MbPartWidth;
	int MbPartHeight = macroblock[CurrMbAddr]->MbPartHeight;
	int SubMbPartWidth = macroblock[CurrMbAddr]->SubMbPartWidth[mbPartIdx];
	int SubMbPartHeight = macroblock[CurrMbAddr]->SubMbPartHeight[mbPartIdx];

	//宏块分区mbPartIdx的左上luma样本相对于宏块左上样本的位置(x, y)
	int x = InverseRasterScan(mbPartIdx, MbPartWidth, MbPartHeight, 16, 0);
	int y = InverseRasterScan(mbPartIdx, MbPartWidth, MbPartHeight, 16, 1);

	int xS = 0;
	int yS = 0;
	if (macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::P_8x8 || macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::P_8x8ref0 || macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::B_8x8)
	{
		//子宏块分区subMbPartIdx的左上luma样本相对于子宏块左上样本的位置(x, y)。
		xS = InverseRasterScan(subMbPartIdx, SubMbPartWidth, SubMbPartHeight, 8, 0);
		yS = InverseRasterScan(subMbPartIdx, SubMbPartWidth, SubMbPartHeight, 8, 1);
	}
	else
	{
		xS = 0;
		yS = 0;
	}

	int predPartWidth = 0;
	if (macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::P_Skip || macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::B_Skip || macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::B_Direct_16x16)
	{
		predPartWidth = 16;
	}
	else if (macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::B_8x8)
	{
		//当currSubMbType等于B_Direct_8x8，并且direct_spatial_mv_pred_flag等于1时，预测的运动矢量为整个宏块的预测运动矢量。
		if (currSubMbType == H264_MB_TYPE::B_Direct_8x8)
		{
			predPartWidth = 16;
		}
		else
		{
			predPartWidth = SubMbPartWidth;
		}
	}
	else if (macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::P_8x8 || macroblock[CurrMbAddr]->mbType == H264_MB_TYPE::P_8x8ref0)
	{
		predPartWidth = SubMbPartWidth;
	}
	else
	{
		predPartWidth = MbPartWidth;
	}


	int xW = 0;
	int yW = 0;

	getMbAddrNAndLuma4x4BlkIdxN(mbAddrA, x + xS - 1, y + yS + 0, 16, 16, xW, yW);
	if (mbAddrA == NA)
	{
		mbAddrA = NA;
		mbPartIdxA = NA;
		subMbPartIdxA = NA;
	}
	else
	{
		ParseSlice::Derivation_process_for_macroblock_and_sub_macroblock_partition_indices(macroblock[mbAddrA], xW, yW, mbPartIdxA, subMbPartIdxA);
	}

	getMbAddrNAndLuma4x4BlkIdxN(mbAddrB, x + xS + 0, y + yS - 1, 16, 16, xW, yW);
	if (mbAddrB == NA)
	{
		mbAddrB = NA;
		mbPartIdxB = NA;
		subMbPartIdxB = NA;
	}
	else
	{
		ParseSlice::Derivation_process_for_macroblock_and_sub_macroblock_partition_indices(macroblock[mbAddrB], xW, yW, mbPartIdxB, subMbPartIdxB);
	}

	getMbAddrNAndLuma4x4BlkIdxN(mbAddrC, x + xS + predPartWidth, y + yS - 1, 16, 16, xW, yW);
	if (mbAddrC == NA)
	{
		mbAddrC = NA;
		mbPartIdxC = NA;
		subMbPartIdxC = NA;
	}
	else
	{
		ParseSlice::Derivation_process_for_macroblock_and_sub_macroblock_partition_indices(macroblock[mbAddrC], xW, yW, mbPartIdxC, subMbPartIdxC);
	}

	getMbAddrNAndLuma4x4BlkIdxN(mbAddrD, x + xS - 1, y + yS - 1, 16, 16, xW, yW);
	if (mbAddrD == NA)
	{
		mbAddrD = NA;
		mbPartIdxD = NA;
		subMbPartIdxD = NA;
	}
	else
	{
		ParseSlice::Derivation_process_for_macroblock_and_sub_macroblock_partition_indices(macroblock[mbAddrD], xW, yW, mbPartIdxD, subMbPartIdxD);
	}
}

int ParseSlice::DiffPicOrderCnt(Picture* picA, Picture* picB)
{
#define PicOrderCnt(picX) std::min(picX->TopFieldOrderCnt, picX->BottomFieldOrderCnt)


	return PicOrderCnt(picA) - PicOrderCnt(picB);


#undef PicOrderCnt
}




void ParseSlice::Derivation_process_for_macroblock_and_sub_macroblock_partition_indices(Macroblock* mb, int xP, int yP, int& mbPartIdxN, int& subMbPartIdxN)
{
	const H264_MB_TYPE mbType = mb->mbType;

	if (mbType == H264_MB_TYPE::I_NxN
		|| mbType == H264_MB_TYPE::I_16x16_0_0_0
		|| mbType == H264_MB_TYPE::I_16x16_1_0_0
		|| mbType == H264_MB_TYPE::I_16x16_2_0_0
		|| mbType == H264_MB_TYPE::I_16x16_3_0_0
		|| mbType == H264_MB_TYPE::I_16x16_0_1_0
		|| mbType == H264_MB_TYPE::I_16x16_1_1_0
		|| mbType == H264_MB_TYPE::I_16x16_2_1_0
		|| mbType == H264_MB_TYPE::I_16x16_3_1_0
		|| mbType == H264_MB_TYPE::I_16x16_0_2_0
		|| mbType == H264_MB_TYPE::I_16x16_1_2_0
		|| mbType == H264_MB_TYPE::I_16x16_2_2_0
		|| mbType == H264_MB_TYPE::I_16x16_3_2_0
		|| mbType == H264_MB_TYPE::I_16x16_0_0_1
		|| mbType == H264_MB_TYPE::I_16x16_1_0_1
		|| mbType == H264_MB_TYPE::I_16x16_2_0_1
		|| mbType == H264_MB_TYPE::I_16x16_3_0_1
		|| mbType == H264_MB_TYPE::I_16x16_0_1_1
		|| mbType == H264_MB_TYPE::I_16x16_1_1_1
		|| mbType == H264_MB_TYPE::I_16x16_2_1_1
		|| mbType == H264_MB_TYPE::I_16x16_3_1_1
		|| mbType == H264_MB_TYPE::I_16x16_0_2_1
		|| mbType == H264_MB_TYPE::I_16x16_1_2_1
		|| mbType == H264_MB_TYPE::I_16x16_2_2_1
		|| mbType == H264_MB_TYPE::I_16x16_3_2_1
		|| mbType == H264_MB_TYPE::I_PCM
		)
	{
		mbPartIdxN = 0;
	}
	else
	{
		const uint8_t MbPartWidth = mb->MbPartWidth;
		const uint8_t MbPartHeight = mb->MbPartHeight;
		mbPartIdxN = (16 / MbPartWidth) * (yP / MbPartHeight) + (xP / MbPartWidth);
	}

	if (mbType != H264_MB_TYPE::P_8x8
		&& mbType != H264_MB_TYPE::P_8x8ref0
		&& mbType != H264_MB_TYPE::B_8x8
		&& mbType != H264_MB_TYPE::B_Skip
		&& mbType != H264_MB_TYPE::B_Direct_16x16
		)
	{
		subMbPartIdxN = 0;
	}
	else if (mbType == H264_MB_TYPE::B_Skip || mbType == H264_MB_TYPE::B_Direct_16x16)
	{
		subMbPartIdxN = 2 * ((yP % 8) / 4) + ((xP % 8) / 4);
	}
	else//mbType is equal to P_8x8, P_8x8ref0, or B_8x8
	{
		const uint8_t MbPartWidth = mb->SubMbPartWidth[mbPartIdxN];
		const uint8_t MbPartHeight = mb->SubMbPartHeight[mbPartIdxN];
		subMbPartIdxN = (8 / MbPartWidth) * ((yP % 8) / MbPartHeight) + ((xP % 8) / MbPartWidth);
	}
}

//POC图像顺序解码过程
void ParseSlice::Decoding_process_for_picture_order_count(DPB& dpb)
{
	//把POC的低位编进码流内
	if (sHeader.sps.pic_order_cnt_type == 0)
	{
		Decoding_process_for_picture_order_count_type_0(dpb);
	}
	else if (sHeader.sps.pic_order_cnt_type == 1)
	{
		Decoding_process_for_picture_order_count_type_1(dpb);
	}
	else if (sHeader.sps.pic_order_cnt_type == 2)
	{
		Decoding_process_for_picture_order_count_type_2(dpb);
	}
	//依赖frame_num求解POC
	//如果是帧或者场对(帧场自适应) PicOrderCnt( picX ) =  Min( TopFieldOrderCnt, BottomFieldOrderCnt )
	//不支持场编码
	PicOrderCnt = std::min(TopFieldOrderCnt, BottomFieldOrderCnt);
}

void ParseSlice::Decoding_process_for_picture_order_count_type_0(DPB& dpb)
{

	//前一个参考图像的 PicOrderCntMsb
	int prevPicOrderCntMsb = 0;
	int prevPicOrderCntLsb = 0;
	if (sHeader.nalu.IdrPicFlag)
	{
		prevPicOrderCntMsb = 0;
		prevPicOrderCntLsb = 0;
	}
	else
	{
		//如果前面的参考图片在解码顺序中包含一个memory_management_control_operation等于5  
		if (dpb.previous->memory_management_control_operation_5_flag)
		{
			//如果在解码顺序上前一个参考图像不是底场，则prevPicOrderCntMsb设置为0,prevPicOrderCntLsb设置为解码顺序上前一个参考图像的TopFieldOrderCnt值。
			prevPicOrderCntMsb = 0;
			prevPicOrderCntLsb = dpb.previous->TopFieldOrderCnt;
		}
		else
		{
			prevPicOrderCntMsb = dpb.previous->PicOrderCntMsb;
			prevPicOrderCntLsb = dpb.previous->pic_order_cnt_lsb;
		}
	}

	if ((sHeader.pic_order_cnt_lsb < prevPicOrderCntLsb)
		&& ((prevPicOrderCntLsb - sHeader.pic_order_cnt_lsb) >= (sHeader.sps.MaxPicOrderCntLsb / 2))
		)
	{
		PicOrderCntMsb = prevPicOrderCntMsb + sHeader.sps.MaxPicOrderCntLsb;
	}
	else if ((sHeader.pic_order_cnt_lsb > prevPicOrderCntLsb)
		&& ((sHeader.pic_order_cnt_lsb - prevPicOrderCntLsb) > (sHeader.sps.MaxPicOrderCntLsb / 2)))
	{
		PicOrderCntMsb = prevPicOrderCntMsb - sHeader.sps.MaxPicOrderCntLsb;
	}
	else
	{
		PicOrderCntMsb = prevPicOrderCntMsb;
	}

	// 当前图像为非底场时
	TopFieldOrderCnt = PicOrderCntMsb + sHeader.pic_order_cnt_lsb;
	// 当前图像为非顶场时
	BottomFieldOrderCnt = TopFieldOrderCnt + sHeader.delta_pic_order_cnt_bottom;
}

void ParseSlice::Decoding_process_for_picture_order_count_type_1(DPB& dpb)
{
	int prevFrameNumOffset = 0;
	if (!sHeader.nalu.IdrPicFlag) //not IDR picture
	{
		if (dpb.previous->memory_management_control_operation_5_flag == 1) //If the previous picture in decoding order included a memory_management_control_operation equal to 5
		{
			prevFrameNumOffset = 0;
		}
		else
		{
			prevFrameNumOffset = dpb.previous->FrameNumOffset;
		}
	}
	//计算变量帧序号偏移
	if (sHeader.nalu.IdrPicFlag)
	{
		FrameNumOffset = 0;
	}
	else if (dpb.previous->frame_num > sHeader.frame_num) //前一图像的帧号比当前图像大
	{
		FrameNumOffset = prevFrameNumOffset + sHeader.sps.MaxFrameNum;
	}
	else
	{
		FrameNumOffset = prevFrameNumOffset;
	}

	int absFrameNum = 0;
	if (sHeader.sps.num_ref_frames_in_pic_order_cnt_cycle != 0)
	{
		absFrameNum = FrameNumOffset + sHeader.frame_num;
	}
	else
	{
		absFrameNum = 0;
	}

	if (sHeader.nalu.nal_ref_idc == 0 && absFrameNum > 0)
	{
		absFrameNum = absFrameNum - 1;
	}
	int picOrderCntCycleCnt = 0;
	int frameNumInPicOrderCntCycle = 0;
	if (absFrameNum > 0)
	{
		picOrderCntCycleCnt = (absFrameNum - 1) / sHeader.sps.num_ref_frames_in_pic_order_cnt_cycle;
		frameNumInPicOrderCntCycle = (absFrameNum - 1) % sHeader.sps.num_ref_frames_in_pic_order_cnt_cycle;
	}


	int expectedPicOrderCnt = 0;
	if (absFrameNum > 0)
	{
		expectedPicOrderCnt = picOrderCntCycleCnt * sHeader.sps.ExpectedDeltaPerPicOrderCntCycle;
		for (size_t i = 0; i <= frameNumInPicOrderCntCycle; i++)
		{
			expectedPicOrderCnt = expectedPicOrderCnt + sHeader.sps.offset_for_ref_frame[i];
		}
	}
	else
	{
		expectedPicOrderCnt = 0;
	}


	if (sHeader.nalu.nal_ref_idc == 0)
	{
		expectedPicOrderCnt = expectedPicOrderCnt + sHeader.sps.offset_for_non_ref_pic;
	}


	if (!sHeader.field_pic_flag) //当前图像为帧
	{
		TopFieldOrderCnt = expectedPicOrderCnt + sHeader.delta_pic_order_cnt[0];
		BottomFieldOrderCnt = TopFieldOrderCnt + sHeader.sps.offset_for_top_to_bottom_field + sHeader.delta_pic_order_cnt[1];
	}
	else if (!sHeader.bottom_field_flag) //当前图像为顶场
	{
		TopFieldOrderCnt = expectedPicOrderCnt + sHeader.delta_pic_order_cnt[0];
	}
	else //当前图像为底场
	{
		BottomFieldOrderCnt = expectedPicOrderCnt + sHeader.sps.offset_for_top_to_bottom_field + sHeader.delta_pic_order_cnt[0];
	}
}

void ParseSlice::Decoding_process_for_picture_order_count_type_2(DPB& dpb)
{
	//	prevFrameNum
	int prevFrameNumOffset = 0;
	if (!sHeader.nalu.IdrPicFlag)
	{
		if (dpb.previous->memory_management_control_operation_5_flag)
		{
			prevFrameNumOffset = 0;
		}
		else
		{
			prevFrameNumOffset = dpb.previous->FrameNumOffset;
		}
	}

	if (sHeader.nalu.IdrPicFlag)
	{
		FrameNumOffset = 0;
	}
	else if (dpb.previous->frame_num > sHeader.frame_num)
	{
		FrameNumOffset = prevFrameNumOffset + sHeader.sps.MaxFrameNum;
	}
	else
	{
		FrameNumOffset = prevFrameNumOffset;
	}

	int tempPicOrderCnt = 0;
	if (sHeader.nalu.IdrPicFlag)
	{
		tempPicOrderCnt = 0;
	}
	else if (sHeader.nalu.nal_ref_idc == 0)
	{
		tempPicOrderCnt = 2 * (FrameNumOffset + sHeader.frame_num) - 1;
	}
	else
	{
		tempPicOrderCnt = 2 * (FrameNumOffset + sHeader.frame_num);
	}

	if (!sHeader.field_pic_flag) //当前图像为帧
	{
		TopFieldOrderCnt = tempPicOrderCnt;
		BottomFieldOrderCnt = tempPicOrderCnt;
	}
	else if (sHeader.bottom_field_flag) //当前图像为底场
	{
		BottomFieldOrderCnt = tempPicOrderCnt;
	}
	else //当前图像为顶场
	{
		TopFieldOrderCnt = tempPicOrderCnt;
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
		bitDepth = sHeader.sps.BitDepthY;
	}
	else
	{
		bitDepth = sHeader.sps.BitDepthC;
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
		qP = sHeader.QSY;
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
		bitDepth = sHeader.sps.BitDepthY;
		qP = macroblock[CurrMbAddr]->QP1Y;
	}
	else //色度
	{
		bitDepth = sHeader.sps.BitDepthC;
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
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				r[i][j] = (m[i][j] + 32) >> 6;
			}
		}
	}
}



int ParseSlice::getQPC(int QPY, bool isChromaCb)
{
	int qPOffset = 0;


	if (isChromaCb)
	{
		//计算色度量化参数的偏移量值
		qPOffset = sHeader.pps.chroma_qp_index_offset;
	}
	else
	{
		qPOffset = sHeader.pps.second_chroma_qp_index_offset;
	}
	//每个色度分量的qPI 值通过下述方式获得  //QpBdOffsetC 色度偏移
	const int qPI = Clip3(-(int)sHeader.sps.QpBdOffsetC, 51, QPY + qPOffset);


	int QPC = 0;
	if (qPI < 30)
	{
		QPC = qPI;
	}
	else
	{
		//int qPIs[] = { 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 }; QPC索引对应下面的QPC的值
		int QPCs[] = { 29, 30, 31, 32, 32, 33, 34, 34, 35, 35, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 39 };

		QPC = QPCs[qPI - 30];
	}
	return QPC;
}
//色度量化参数的推导过程  
void ParseSlice::getChromaQuantisationParameters(bool isChromaCb)
{

	const int QPC = getQPC(macroblock[CurrMbAddr]->QPY, isChromaCb);

	const int QP1C = QPC + sHeader.sps.QpBdOffsetC;

	//QPC:各色度分量Cb和Cr的色度量化参数，
	macroblock[CurrMbAddr]->QPC = QPC;
	macroblock[CurrMbAddr]->QP1C = QP1C;



	if ((SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP || (SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_SI)
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
	int xP = InverseRasterScan(CurrMbAddr, 16, 16, sHeader.sps.PicWidthInSamplesL, 0);
	int yP = InverseRasterScan(CurrMbAddr, 16, 16, sHeader.sps.PicWidthInSamplesL, 1);

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
		else //8*8
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
		int MbWidthC = sHeader.sps.MbWidthC;
		int MbHeightC = sHeader.sps.MbHeightC;


		if (sHeader.sps.ChromaArrayType == 1 || sHeader.sps.ChromaArrayType == 2) // YUV420 or YUV422
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
					chromaData[xP / sHeader.sps.SubWidthC + xO + j][yP / sHeader.sps.SubHeightC + yO + i] = u[i * MbWidthC + j];
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
	if (sHeader.sps.separate_colour_plane_flag)
	{
		//colour_plane_id等于0、1和2分别对应于Y、Cb和Cr平面。
		iYCbCr = sHeader.colour_plane_id;
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
	inverseScanner4x4Process(sHeader.ScalingList4x4[iYCbCr + ((mbIsInterFlag) ? 3 : 0)], weightScale4x4);

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
	inverseScanner8x8Process(sHeader.ScalingList8x8[2 * iYCbCr + mbIsInterFlag], weightScale8x8);


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
				if (i % 4 == 0 && j % 4 == 0)
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
