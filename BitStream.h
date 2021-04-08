#pragma once
#include <cstdint>
#include <cmath>
//#include "Common.h"

class BitStream
{
public:

	//���캯�����Դ���һ�� buffer���������ֱ�Ӱ� nalu �� buffer ����
	BitStream(uint8_t* buf, int size);

	//��ȡ1bit
	uint16_t readBit();

	//��ȡn��bit
	int readMultiBit(int n);
	//��ȡһ���޷���ָ�����ײ��ر��������
	int readUE();
	int readSE();

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
