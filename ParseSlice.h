#pragma once
#include "Common.h"
#include "SliceHeader.h"
#include "SliceData.h"
#include "ParsePPS.h"
#include "ParseSPS.h"
#include "ParseNalu.h"
#include "Macroblock.h"


struct  MyBITMAPFILEHEADER                     /**** BMP file header structure ****/
{
	uint32_t   bfSize;           /* Size of file */
	uint16_t   bfReserved1;      /* Reserved */
	uint16_t   bfReserved2;      /* ... */
	uint32_t   bfOffBits;        /* Offset to bitmap data */
};

struct MyBITMAPINFOHEADER		/**** BMP file info structure ****/
{
	uint32_t biSize;           /* Size of info header */
	int32_t  biWidth;          /* Width of image */
	int32_t  biHeight;         /* Height of image */
	uint16_t biPlanes;         /* Number of color planes */
	uint16_t biBitCount;       /* Number of bits per pixel */
	uint32_t biCompression;    /* Type of compression to use */
	uint32_t biSizeImage;      /* Size of image data */
	int32_t  biXPelsPerMeter;  /* X pixels per meter */
	int32_t  biYPelsPerMeter;  /* Y pixels per meter */
	uint32_t biClrUsed;        /* Number of colors used */
	uint32_t biClrImportant;   /* Number of important colors */
};



class ParseSlice
{
	//public:
	//	ParseNalu& nalu;
	//	Macroblock** macroblock;
	//	SliceHeader* sHeader;
public:
	const ParseNalu& nalu;
	SliceHeader* sHeader;
	Macroblock** macroblock;
private:

public:

	uint8_t** lumaData;//存储解码后图片的Y分量数据
	uint8_t** chromaCbData;//存储解码后图片的cb分量数据
	uint8_t** chromaCrData;//存储解码后图片的cr分量数据

	uint32_t	CurrMbAddr;  //当前宏块的位置
	uint32_t	mbX;        //当前宏块x的位置（相对于图片左上角位置）
	uint32_t	mbY;		//当前宏块y的位置（相对于图片左上角位置）


	int LevelScale4x4[6][4][4];
	int LevelScale8x8[6][8][8];




	uint32_t PicWidthInSamplesL;
	uint32_t PicHeightInSamplesL;
	uint32_t PicWidthInSamplesC;
	uint32_t PicHeightInSamplesC;
	uint32_t PicSizeInMbs;




	uint32_t  sliceNumber; //当前帧第几个slice

	uint32_t  mbCount; //解码的宏块的计数

public:
	ParseSlice(ParseNalu& nalu, SliceHeader* sHeader);

	bool parse();

	void init();

	~ParseSlice();

	void saveBmpFile(const char* filename);

	void Deblocking_filter_process();

	void transformDecode4x4LuamResidualProcess();

	void transformDecode8x8LuamResidualProcess();

	void transformDecode16x16LuamResidualProcess(const int i16x16DClevel[16], const int i16x16AClevel[16][16], bool isLuam, bool isChromaCb);

	void transformDecodeChromaResidualProcess(bool isChromaCb);











	void getChromaQuantisationParameters(bool isChromaCb);


	void getMbAddrNAndLuma4x4BlkIdxN(int& mbAddrN, const int xN, const int yN, const int maxW, const int maxH, int& xW, int& yW);


	void Picture_construction_process_prior_to_deblocking_filter_process(int* u, const char* type, const size_t BlkIdx, const bool isLuam, bool isChromaCb = true);

	void scaling(bool isLuam, bool isChromaCb);

private:
	void convertYuv420(uint8_t* buffer, size_t widthBytes);


	static int Derivation_process_for_4x4_luma_block_indices(int x, int y);
	static int Derivation_process_for_8x8_luma_block_indices(int x, int y);

	int getQPC(int QPY, bool isChromaCb);

	void getIntra4x4PredMode(size_t luma4x4BlkIdx, bool isLuam);

	void getIntra8x8PredMode(size_t luma8x8BlkIdx, bool isLuam);

	void transformDecodeChromaArrayTypeEqualTo3Process(bool isChromaCb);

	void transformDecodeIntra_16x16DCProcess(int c[4][4], int dcY[4][4], bool isLuam, bool isChromaCb);

	void transformDecodeChromaDCProcess(int c[4][2], int dcC[4][2], int MbWidthC, int MbHeightC, bool isChromaCb);

	void Intra_4x4_prediction(size_t luma4x4BlkIdx, bool isLuam);

	void Intra_8x8_prediction(size_t luma8x8BlkIdx, bool isLuam);

	void Intra_16x16_prediction(bool isLuam);

	void Intra_chroma_prediction(bool isChromaCb);

	void inverseScanner4x4Process(const int value[16], int c[4][4]);

	void inverseScanner8x8Process(int value[64], int c[8][8]);


	void scalingTransformProcess(int c[4][4], int r[4][4], bool isLuam, bool isChromaCb);

	void Scaling_and_transformation_process_for_residual_8x8_blocks(int c[8][8], int r[8][8], bool isLuam, bool isChromaCb);

	void Filtering_process_for_block_edges(int mbAddr, int mbAddrN, bool chromaEdgeFlag, bool verticalEdgeFlag, bool fieldModeInFrameFilteringFlag, int iCbCr, bool mbEdgeFlag, int xE[16], int yE[16]);
	void Filtering_process_for_a_set_of_samples_across_a_horizontal_or_vertical_block_edge(int mbAddr, int mbAddrN, bool chromaEdgeFlag, bool verticalEdgeFlag, bool fieldModeInFrameFilteringFlag,
		int iCbCr, bool mbEdgeFlag, const int p[4], const int q[4], int pp[3], int qq[3], const int mb_p0_x, const int mb_p0_y, const int mb_q0_x, const int mb_q0_y);
	int Derivation_process_for_the_luma_content_dependent_boundary_filtering_strength(const int mbAddr, bool MbaffFrameFlag, int mbAddrN, bool mbEdgeFlag, int p0, int q0, bool verticalEdgeFlag, const int mb_p0_x, const int mb_p0_y, const int mb_q0_x, const int mb_q0_y);

	void Derivation_process_for_the_thresholds_for_each_block_edge(int p0, int q0, int p1, int q1, bool chromaEdgeFlag, int bS, int filterOffsetA, int  filterOffsetB, int qPp, int qPq, int& indexA, int& alpha, int& beta, bool& filterSamplesFlag);
	void Filtering_process_for_edges_with_bS_less_than_4(const int p[4], const int q[4], int pp[3], int qq[3], bool chromaEdgeFlag, int bS, int beta, int indexA, bool chromaStyleFilteringFlag);
	void Filtering_process_for_edges_for_bS_equal_to_4(const int p[4], const int q[4], int pp[3], int qq[3], int alpha, int beta, bool chromaStyleFilteringFlag);
};

