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
	Picture* pic; //存储每个解码完成的图像

	//标识在条带头中提到的图像参数集。变量 pic_parameter_set_id 的值应该在 0 到  255 的 范围内（包括0 和255）。
	//ParsePPS ppsCache[256];
	ParsePPS* ppsCache;
	//用于识别图像参数集所指的序列参数集。seq_parameter_set_id 的值应在 0-31 的范围 内，包括0 和31。
	//ParseSPS spsCache[32];
	ParseSPS* spsCache;

	DPB dpb;
private:
	static const int MAX_BUFFER_SIZE;
	bool CheckStartCode(int& startCodeLen, uint8_t* bufPtr, int bufLen);


	void getNaluHeader(uint8_t* buffer, int size);

	//去掉防竞争字节
	size_t unescape(uint8_t* src, uint32_t src_len);
	FILE* f = nullptr;

	//使用typedef定义结构体同时，给_NaluUnit结构体别名NaluUnit，后续定义可不用使用struct data，直接使用DATA即可
};




