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

public:
	ParseSlice(ParseNalu& nalu);

	bool parse(BitStream& bs, const ParsePPS* ppsCache, const ParseSPS* spsCache);

	~ParseSlice();

	



	void transformDecode4x4LuamResidualProcess();
};

