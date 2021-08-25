#pragma once
#include "Common.h"
#include "ParseSPS.h"
#include "ParsePPS.h"
#include "SliceHeader.h"
#include "SliceData.h"
#include "ParseSlice.h"
#include "ParseNalu.h"
#include "ParseSEI.h"
#include "Picture.h"
#include "DPB.h"


class AnnexBReader
{
public:
	AnnexBReader();

	bool open(const char* filePath);
	bool close();
	int ReadNalu(uint8_t* buffer, rsize_t& dataLen);


	~AnnexBReader();
private:

	ParseSlice* slice;
	Picture* pic; //�洢ÿ��������ɵ�ͼ��

	//��ʶ������ͷ���ᵽ��ͼ������������� pic_parameter_set_id ��ֵӦ���� 0 ��  255 �� ��Χ�ڣ�����0 ��255����
	//ParsePPS ppsCache[256];
	ParsePPS* ppsCache;
	//����ʶ��ͼ���������ָ�����в�������seq_parameter_set_id ��ֵӦ�� 0-31 �ķ�Χ �ڣ�����0 ��31��
	//ParseSPS spsCache[32];
	ParseSPS* spsCache;

	DPB dpb;
private:
	static const int MAX_BUFFER_SIZE;
	bool CheckStartCode(int& startCodeLen, uint8_t* bufPtr, int bufLen);


	void getNaluHeader(uint8_t* buffer, int size);

	//ȥ���������ֽ�
	size_t unescape(uint8_t* src, uint32_t src_len);
	FILE* f = nullptr;

	//ʹ��typedef����ṹ��ͬʱ����_NaluUnit�ṹ�����NaluUnit����������ɲ���ʹ��struct data��ֱ��ʹ��DATA����
};




