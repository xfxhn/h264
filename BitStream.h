#pragma once
#include <cstdint>
#include <cmath>
//#include "Common.h"

class BitStream
{
public:

	//构造函数可以传入一个 buffer，这里可以直接把 nalu 的 buffer 传入
	BitStream(uint8_t* buf, int size);

	//读取1bit
	uint16_t readBit();

	//读取n个bit
	int readMultiBit(int n);
	//读取一个无符号指数哥伦布熵编码的数据
	int readUE();
	int readSE();

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
