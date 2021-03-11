#pragma once
#include <cstring>
#include <cstdint>
#include <iostream>
#pragma warning(disable:4996)


class AnnexBReader
{
public:
	AnnexBReader();

	bool open(const char* filePath);
	bool close();
	int ReadNalu(uint8_t* buffer, rsize_t& dataLen);
	~AnnexBReader();
	
private:
	static const int MAX_BUFFER_SIZE;
	bool CheckStartCode(int& startCodeLen, uint8_t* bufPtr, int bufLen,int slice);
	int ReadFromFile();

	void getNaluHeader(uint8_t byte);


	FILE* f = nullptr;

	typedef struct _NaluUnit
	{
		int type;
		int size;
		uint8_t* data;
	}NaluUnit;
	//使用typedef定义结构体同时，给_NaluUnit结构体别名NaluUnit，后续定义可不用使用struct data，直接使用DATA即可
	



};




