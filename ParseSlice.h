#pragma once
#include "Common.h"
#include "SliceHeader.h"
#include "SliceData.h"
#include "ParsePPS.h"
#include "ParseSPS.h"
#include "ParseNalu.h"
#include "Macroblock.h"
#include "Picture.h"
#include "DPB.h"

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
	SliceHeader& sHeader;
	Macroblock** macroblock;

	bool decodeFinished;
	//private:
	//	Picture* dpb[16];
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
	uint32_t  sliceCount; //第几个slice(可能一帧里有多个slice)



	//-2^31 到 2^31-1
	int         PicOrderCntMsb;
	int         PicOrderCntLsb;
	int         TopFieldOrderCnt;
	int         BottomFieldOrderCnt;

	int			FrameNumOffset;
	int			PicOrderCnt;



public:
	ParseSlice(SliceHeader& sHeader);

	bool parse();


	~ParseSlice();

	void saveBmpFile(const char* filename);

	void Deblocking_filter_process(SliceHeader& sHeader);

	void transformDecode4x4LuamResidualProcess();

	void transformDecode4x4LuamResidualProcessInter(uint8_t predPartL[16][16]);

	void transformDecode8x8LuamResidualProcess();

	void transformDecode8x8LuamResidualProcessInter(uint8_t predPartL[16][16]);

	void transformDecode16x16LuamResidualProcess(const int i16x16DClevel[16], const int i16x16AClevel[16][16], bool isLuam, bool isChromaCb);

	void transformDecodeChromaResidualProcess(bool isChromaCb);

	void transformDecodeChromaResidualProcessInter(uint8_t predPartC[16][16], bool isChromaCb);

	void Inter_prediction_process(DPB& dpb, uint8_t predPartL[16][16], uint8_t predPartCb[16][16], uint8_t predPartCr[16][16], bool isSkip = false);

	void Sample_construction_process_for_I_PCM_macroblocks();




	void Decoding_process_for_picture_order_count(DPB& bpb);

	void Decoding_process_for_picture_order_count_type_0(DPB& dpb);
	void Decoding_process_for_picture_order_count_type_1(DPB& dpb);
	void Decoding_process_for_picture_order_count_type_2(DPB& dpb);


	void getMbAddrNAndLuma4x4BlkIdxN(int& mbAddrN, const int xN, const int yN, const int maxW, const int maxH, int& xW, int& yW, int _CurrMbAddr = -1);

	static void Derivation_process_for_macroblock_and_sub_macroblock_partition_indices(Macroblock* mb, int xP, int yP, int& mbPartIdxN, int& subMbPartIdxN);

private:
	void convertYuv420(uint8_t* buffer, size_t widthBytes);

	void scaling(bool isLuam, bool isChromaCb);

	void Picture_construction_process_prior_to_deblocking_filter_process(int* u, const char* type, const size_t BlkIdx, const bool isLuam, bool isChromaCb = true);

	static int Derivation_process_for_4x4_luma_block_indices(int x, int y);
	static int Derivation_process_for_8x8_luma_block_indices(int x, int y);
	void getChromaQuantisationParameters(bool isChromaCb);
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

	void Filtering_process_for_block_edges(SliceHeader& header, int mbAddr, int mbAddrN, bool chromaEdgeFlag, bool verticalEdgeFlag, bool fieldModeInFrameFilteringFlag, int iCbCr, bool mbEdgeFlag, int xE[16], int yE[16]);
	void Filtering_process_for_a_set_of_samples_across_a_horizontal_or_vertical_block_edge(SliceHeader& header, int mbAddr, int mbAddrN, bool chromaEdgeFlag, bool verticalEdgeFlag, bool fieldModeInFrameFilteringFlag,
		int iCbCr, bool mbEdgeFlag, const int p[4], const int q[4], int pp[3], int qq[3], const int mb_p0_x, const int mb_p0_y, const int mb_q0_x, const int mb_q0_y);
	int Derivation_process_for_the_luma_content_dependent_boundary_filtering_strength(SliceHeader& header, const int mbAddr, bool MbaffFrameFlag, int mbAddrN, bool mbEdgeFlag, int p0, int q0, bool verticalEdgeFlag, const int mb_p0_x, const int mb_p0_y, const int mb_q0_x, const int mb_q0_y);

	void Derivation_process_for_the_thresholds_for_each_block_edge(SliceHeader& header, int p0, int q0, int p1, int q1, bool chromaEdgeFlag, int bS, int filterOffsetA, int  filterOffsetB, int qPp, int qPq, int& indexA, int& alpha, int& beta, bool& filterSamplesFlag);
	void Filtering_process_for_edges_with_bS_less_than_4(SliceHeader& header, const int p[4], const int q[4], int pp[3], int qq[3], bool chromaEdgeFlag, int bS, int beta, int indexA, bool chromaStyleFilteringFlag);
	void Filtering_process_for_edges_for_bS_equal_to_4(const int p[4], const int q[4], int pp[3], int qq[3], int alpha, int beta, bool chromaStyleFilteringFlag);


	void Decoding_process_for_Inter_prediction_samples(
		int logWDL, int w0L, int w1L, int o0L, int o1L,
		int logWDCb, int w0Cb, int w1Cb, int o0Cb, int o1Cb,
		int logWDCr, int w0Cr, int w1Cr, int o0Cr, int o1Cr,
		DPB& dpb,
		const int xAL, const int yAL,
		int xP, int xS, int yP, int yS,
		int mbPartIdx, int subMbPartIdx,
		int partWidth, int partHeight, int partWidthC, int partHeightC, int mvL0[2], int mvL1[2], int mvCL0[2], int mvCL1[2],
		int refIdxL0, int refIdxL1, int predFlagL0, int predFlagL1,
		uint8_t predPartL[16][16], uint8_t predPartCb[16][16], uint8_t predPartCr[16][16]);

	void Fractional_sample_interpolation_process(const int xAL, const int yAL, int mbPartIdx, int subMbPartIdx,
		int partWidth, int partHeight, int partWidthC, int partHeightC, int mvLX[2], int mvCLX[2], Picture* refPic,
		uint8_t* predPartLXL, uint8_t* predPartLXCb, uint8_t* predPartLXCr);

	uint8_t Luma_sample_interpolation_process(int xIntL, int yIntL, int xFracL, int yFracL, Picture* refPic);
	uint8_t Chroma_sample_interpolation_process(int xIntC, int yIntC, int xFracC, int yFracC, Picture* refPic, bool isChromaCb);

	void Weighted_sample_prediction_process(
		int logWDL, int w0L, int w1L, int o0L, int o1L,
		int logWDCb, int w0Cb, int w1Cb, int o0Cb, int o1Cb,
		int logWDCr, int w0Cr, int w1Cr, int o0Cr, int o1Cr,
		int xP, int xS, int yP, int yS,
		int mbPartIdx, int subMbPartIdx,
		int partWidth, int partHeight, int partWidthC, int partHeightC, int predFlagL0, int predFlagL1,
		uint8_t* predPartL0L, uint8_t* predPartL0Cb, uint8_t* predPartL0Cr, uint8_t* predPartL1L, uint8_t* predPartL1Cb, uint8_t* predPartL1Cr,
		uint8_t predPartL[16][16], uint8_t predPartCb[16][16], uint8_t predPartCr[16][16]);

	void Default_weighted_sample_prediction_process(
		int xP, int xS, int yP, int yS,
		int partWidth, int partHeight, int partWidthC, int partHeightC, int predFlagL0, int predFlagL1,
		uint8_t* predPartL0L, uint8_t* predPartL0Cb, uint8_t* predPartL0Cr, uint8_t* predPartL1L, uint8_t* predPartL1Cb, uint8_t* predPartL1Cr,
		uint8_t predPartL[16][16], uint8_t predPartCb[16][16], uint8_t predPartCr[16][16]);

	void Weighted_sample_prediction_process_next(
		int xP, int xS, int yP, int yS,
		int logWDL, int w0L, int w1L, int o0L, int o1L,
		int logWDCb, int w0Cb, int w1Cb, int o0Cb, int o1Cb,
		int logWDCr, int w0Cr, int w1Cr, int o0Cr, int o1Cr,
		int partWidth, int partHeight, int partWidthC, int partHeightC, int predFlagL0, int predFlagL1,
		uint8_t* predPartL0L, uint8_t* predPartL0Cb, uint8_t* predPartL0Cr, uint8_t* predPartL1L, uint8_t* predPartL1Cb, uint8_t* predPartL1Cr,
		uint8_t predPartL[16][16], uint8_t predPartCb[16][16], uint8_t predPartCr[16][16]);

	void Derivation_process_for_motion_vector_components_and_reference_indices(DPB& dpb, int mbPartIdx, int subMbPartIdx,
		int mvL0[2], int mvL1[2], int mvCL0[2], int mvCL1[2], int& refIdxL0, int& refIdxL1, int& predFlagL0, int& predFlagL1, int& subMvCnt);

	void Derivation_process_for_luma_motion_vector_prediction(int mbPartIdx, int subMbPartIdx, H264_MB_TYPE currSubMbType, bool listSuffixFlag, int refIdxL0, int mvpLX[2]);

	void Derivation_process_for_chroma_motion_vectors(int mvLX[2], int mvCLX[2]);
	void Derivation_process_for_luma_motion_vectors_for_B_Skip_or_B_Direct_16x16_or_B_Direct_8x8(DPB& dpb, int mbPartIdx, int subMbPartIdx,
		int& refIdxL0, int& refIdxL1, int mvL0[2], int mvL1[2], int& predFlagL0, int& predFlagL1, int& subMvCnt);

	void Derivation_process_for_the_co_located_4x4_sub_macroblock_partitions(DPB& dpb, int mbPartIdx, int subMbPartIdx,
		Picture*& colPic, int& mbAddrCol, int mvCol[2], int& refIdxCol);

	void Derivation_process_for_spatial_direct_luma_motion_vector_and_reference_index_prediction_mode(DPB& dpb, int mbPartIdx, int subMbPartIdx,
		int& refIdxL0, int& refIdxL1, int mvL0[2], int mvL1[2], int& predFlagL0, int& predFlagL1, int& subMvCnt);
	void Derivation_process_for_temporal_direct_luma_motion_vector_and_reference_index_prediction_mode(DPB& dpb, int mbPartIdx, int subMbPartIdx,
		int& refIdxL0, int& refIdxL1, int mvL0[2], int mvL1[2], int& predFlagL0, int& predFlagL1);



	void Derivation_process_for_motion_data_of_neighbouring_partitions(int mbPartIdx, int subMbPartIdx, H264_MB_TYPE currSubMbType, bool listSuffixFlag,
		int& mbAddrA, int& mbAddrB, int& mbAddrC, int mvLXA[2], int mvLXB[2], int mvLXC[2], int& refIdxLXA, int& refIdxLXB, int& refIdxLXC);
	void Derivation_process_for_neighbouring_partitions(int mbPartIdx, int subMbPartIdx, H264_MB_TYPE currSubMbType,
		int& mbAddrA, int& mbAddrB, int& mbAddrC, int& mbAddrD,
		int& mbPartIdxA, int& mbPartIdxB, int& mbPartIdxC, int& mbPartIdxD,
		int& subMbPartIdxA, int& subMbPartIdxB, int& subMbPartIdxC, int& subMbPartIdxD);

	static int DiffPicOrderCnt(int PicOrderCntA, int PicOrderCntB);


	void Derivation_process_for_prediction_weights(DPB& dpb, int refIdxL0, int refIdxL1, int predFlagL0, int predFlagL1,
		int& logWDL, int& w0L, int& w1L, int& o0L, int& o1L, int& logWDCb, int& w0Cb, int& w1Cb, int& o0Cb, int& o1Cb, int& logWDCr, int& w0Cr, int& w1Cr, int& o0Cr, int& o1Cr);

};

