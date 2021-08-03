#pragma once
#include "Common.h"
#include "SliceHeader.h"
#include "SliceData.h"
#include "ParsePPS.h"
#include "ParseSPS.h"
#include "ParseNalu.h"
#include "Macroblock.h"



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

	uint8_t** lumaData;//�洢�����ͼƬ��Y��������
	uint8_t** chromaCbData;//�洢�����ͼƬ��cb��������
	uint8_t** chromaCrData;//�洢�����ͼƬ��cr��������

	uint32_t	CurrMbAddr;  //��ǰ����λ��
	uint32_t	mbX;        //��ǰ���x��λ�ã������ͼƬ���Ͻ�λ�ã�
	uint32_t	mbY;		//��ǰ���y��λ�ã������ͼƬ���Ͻ�λ�ã�


	int LevelScale4x4[6][4][4];
	int LevelScale8x8[6][8][8];




	uint32_t PicWidthInSamplesL;
	uint32_t PicHeightInSamplesL;
	uint32_t PicWidthInSamplesC;
	uint32_t PicHeightInSamplesC;
	uint32_t PicSizeInMbs;




	uint32_t  sliceNumber; //��ǰ֡�ڼ���slice

	uint32_t  mbCount; //����ĺ��ļ���

public:
	ParseSlice(ParseNalu& nalu, SliceHeader* sHeader);

	bool parse();

	void init();

	~ParseSlice();






	void transformDecode4x4LuamResidualProcess();

	void transformDecode8x8LuamResidualProcess();

	void transformDecode16x16LuamResidualProcess(const int i16x16DClevel[16], const int i16x16AClevel[16][16], bool isLuam, bool isChromaCb);

	void transformDecodeChromaResidualProcess(bool isChromaCb);











	void getChromaQuantisationParameters(bool isChromaCb);
	void getIntra4x4PredMode(size_t luma4x4BlkIdx, bool isLuam);

	void getMbAddrNAndLuma4x4BlkIdxN(int& mbAddrN, const int xN, const int yN, const int maxW, const int maxH, int& xW, int& yW);


	void Picture_construction_process_prior_to_deblocking_filter_process(int* u, const char* type, const size_t BlkIdx, const bool isLuam, bool isChromaCb = true);

	void scaling(bool isLuam, bool isChromaCb);
	void free();

private:
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
};

