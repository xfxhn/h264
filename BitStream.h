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

	//构造函数可以传入一个 buffer，这里可以直接把 nalu 的 buffer 传入
	BitStream(uint8_t* buf, int size);

	//读取1bit
	uint16_t readBit();

	//读取n个bit
	int readMultiBit(int n);
	//获取n个bit
	int getMultiBit(int n);
	//读取一个无符号指数哥伦布熵编码的数据
	int readUE();
	int readSE();
	int readME(int ChromaArrayType, H264_MB_PART_PRED_MODE mode);

	

	bool more_rbsp_data();

	~BitStream();

public:
	//指向开始的位置
	uint8_t* start = nullptr;

	// buffer 的长度（单位 Byte）
	size_t size = 0;

	// 当前读取到了哪个字节
	uint8_t* currentPtr = nullptr;

	// 当前读取到了字节中的第几位
	size_t bitsLeft = 8;

	uint8_t postion;
};
