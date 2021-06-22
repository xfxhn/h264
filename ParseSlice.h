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
	ParseNalu& nalu;
	SliceHeader* sHeader;
	Macroblock** macroblock;
public:
	uint32_t	CurrMbAddr;  //��ǰ����λ��
	uint32_t	mbX;        //��ǰ���x��λ�ã������ͼƬ���Ͻ�λ�ã�
	uint32_t	mbY;		//��ǰ���y��λ�ã������ͼƬ���Ͻ�λ�ã�


	int LevelScale4x4[6][4][4];
	int LevelScale8x8[6][8][8];
	uint8_t** lumaData;//�洢�����ͼƬ��Y��������

public:
	ParseSlice(ParseNalu& nalu);

	bool parse(BitStream& bs, const ParsePPS* ppsCache, const ParseSPS* spsCache);

	~ParseSlice();



	void Intra_4x4_prediction(size_t luma4x4BlkIdx, bool isLuam);


	void transformDecode4x4LuamResidualProcess();

	void inverseScanner4x4Process(int value[16], int c[4][4]);

	void inverseScanner8x8Process(int value[64], int c[8][8]);

	void scalingTransformProcess(int c[4][4], int r[4][4], bool isLuam, bool isChromaCb);

	void getChromaQuantisationParameters(bool isChromaCb);
	void getIntra4x4PredMode(size_t luma4x4BlkIdx, bool isLuam);

	void getMbAddrNAndLuma4x4BlkIdxN(bool isLuam, int& mbAddrN, const int xN, const int yN, const int maxW, const int maxH, int& xW, int& yW);


	void Picture_construction_process_prior_to_deblocking_filter_process(int* u, const char* type, const size_t BlkIdx, const bool isLuam);

	void scaling(bool isLuam, bool isChromaCb);
};

