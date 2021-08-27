#pragma once
#include "Common.h"
#include "FileReader.h"
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

	
private:


	void getNaluHeader(uint8_t* buffer, int size);

};




