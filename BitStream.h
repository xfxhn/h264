#pragma once
#include <cstdint>
#include <cmath>
#include "NaluType.h"


struct maping_coded_block_pattern
{
	int32_t code_num;
	int32_t Intra_4x4_or_Intra_8x8;
	int32_t Inter;
};


struct maping_coded_block_pattern_equal0
{
	int32_t code_num;
	int32_t Intra_4x4_or_Intra_8x8;
	int32_t Inter;
};

class BitStream
{
public:

	//���캯�����Դ���һ�� buffer���������ֱ�Ӱ� nalu �� buffer ����
	BitStream(uint8_t* buf, int size);

	//��ȡ1bit
	uint16_t readBit();

	//��ȡn��bit
	int readMultiBit(int n);
	//��ȡn��bit
	int getMultiBit(int n);
	//��ȡһ���޷���ָ�����ײ��ر��������
	int readUE();
	int readSE();
	int readME(int ChromaArrayType, H264_MB_PART_PRED_MODE mode);

	

	bool more_rbsp_data();

	~BitStream();

public:
	//ָ��ʼ��λ��
	uint8_t* start = nullptr;

	// buffer �ĳ��ȣ���λ Byte��
	size_t size = 0;

	// ��ǰ��ȡ�����ĸ��ֽ�
	uint8_t* currentPtr = nullptr;

	// ��ǰ��ȡ�����ֽ��еĵڼ�λ
	size_t bitsLeft = 8;

	uint8_t postion;
};
