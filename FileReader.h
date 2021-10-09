#pragma once
#include "Common.h"

class FileReader
{
private:
	static const size_t MAX_BUFFER_SIZE;
	FILE* file;
	uint8_t* bufferStart;
	uint8_t* bufferPosition;
	uint8_t* bufferEnd;

private:
	static int getNextStartCode(uint8_t* bufPtr, const uint8_t* end, uint8_t*& pos1, uint8_t*& pos2, int& startCodeLen1,
		int& startCodeLen2);

	static size_t EBSP_TO_RBSP(uint8_t* src, size_t length);
public:
	explicit FileReader(const char* filename);
	void readNalUint(uint8_t*& data, size_t& size, bool& isStopLoop);
	~FileReader();
};

