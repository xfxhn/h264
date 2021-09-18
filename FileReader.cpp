#include "FileReader.h"


const size_t FileReader::MAX_BUFFER_SIZE = 1024 * 1024;

int FileReader::getNextStartCode(uint8_t* bufPtr, const uint8_t* end, uint8_t*& pos1, uint8_t*& pos2, int& startCodeLen1, int& startCodeLen2)
{
	uint8_t* pos = bufPtr;
	/* uint8_t *end = bufPtr + bufferSize;*/
	int type = 0;
	//查找开头的startCode
	while (pos < end) {
		if (pos[0] == 0) {
			if (pos[1] == 0) {
				if (pos[2] == 0) {
					if (pos[3] == 1) { //0 0 0 1
						startCodeLen1 = 4;
						pos1 = pos;
						pos += startCodeLen1;
						type = 1;
						break;

					}
				}
				if (pos[2] == 1) {  //0 0 1
					startCodeLen1 = 3;
					pos1 = pos;
					pos += startCodeLen1;
					type = 1;
					break;
				}
			}
		}
		pos++;
	}
	//查找结尾的startCode
	while (pos < end) {
		if (pos[0] == 0) {
			if (pos[1] == 0) {
				if (pos[2] == 0) {
					if (pos[3] == 1) { //0 0 0 1
						pos2 = pos;
						startCodeLen2 = 4;
						type = 2;
						break;
					}
				}
				if (pos[2] == 1) {  //0 0 1
					pos2 = pos;
					startCodeLen2 = 3;
					type = 2;
					break;
				}

				if (pos[2] == 3) {
					if (pos[3] <= 3) {

					}
				}
			}
		}
		pos++;
	}
	return type;
}

size_t FileReader::EBSP_TO_RBSP(uint8_t* src, size_t length)
{
	size_t size = length;
	for (size_t start = 0; start < length; start++) {
		if (src[start] == 0 && src[start + 1] == 0 && src[start + 2] == 3 && src[start + 3] <= 3) {
			--size;
			size_t index = start + 2;
			for (size_t j = index; j < length - 1; j++) {
				src[j] = src[j + 1];
			}
		}
	}
	return size;
}

FileReader::FileReader(const char* filename)
{
	if (file)
	{
		return;
	}
	file = fopen(filename, "rb");
	if (file == nullptr) {
		std::cout << "fopen打开失败" << std::endl;
		return;
	}

	bufferStart = new uint8_t[FileReader::MAX_BUFFER_SIZE];
	size_t bufferSize = fread(bufferStart, 1, FileReader::MAX_BUFFER_SIZE, file);

	bufferPosition = bufferStart;

	bufferEnd = bufferStart + bufferSize - 1;
}

void FileReader::readNalUint(uint8_t*& data, size_t& size, bool& isStopLoop)
{
	while (true) {
		uint8_t* pos1 = nullptr;
		uint8_t* pos2 = nullptr;
		int startCodeLen1 = 0;
		int startCodeLen2 = 0;

		const int type = FileReader::getNextStartCode(bufferPosition, bufferEnd, pos1, pos2, startCodeLen1,
			startCodeLen2);

		size_t residual = (bufferEnd - pos1 + 1);
		size_t readSize = FileReader::MAX_BUFFER_SIZE - residual;
		if (type == 1) { //表示找到了开头的startCode,没找到后面的
			for (int i = 0; i < residual; ++i) {
				bufferStart[i] = pos1[i];
			}
			//每次读File::MAX_BUFFER_SIZE个，这里读取的NALU必须要包含一整个slice,字节对齐
			size_t bufferSize = fread(bufferStart + residual, 1, readSize, file);
			if (bufferSize == 0) {
				//表示读完数据了
				/*data = bufferStart + startCodeLen1;
				size = residual - startCodeLen1;*/
				uint8_t* EBSP = bufferStart + startCodeLen1;
				size = FileReader::EBSP_TO_RBSP(EBSP, residual - startCodeLen1);
				data = EBSP;
				isStopLoop = false;
				break;
			}
			bufferPosition = bufferStart;
			bufferEnd = bufferStart + residual + bufferSize - 1;
		}
		else if (type == 2) {//都找到了
			/*data = pos1 + startCodeLen1;
			size = pos2 - data;
			bufferPosition = data + size;*/

			uint8_t* EBSP = pos1 + startCodeLen1;

			size_t EBSPSize = pos2 - EBSP;

			size = FileReader::EBSP_TO_RBSP(EBSP, EBSPSize);
			data = EBSP;
			bufferPosition = data + EBSPSize;
			break;
		}
		else {
			//错误

			std::cout << "没有找到开头startCode，也没有找到后面的startCode" << std::endl;
			isStopLoop = false;
			break;
		}
	}
}

FileReader::~FileReader()
{
	if (bufferStart) {
		delete[] bufferStart;
		bufferStart = nullptr;
	}

	if (file) {
		fclose(file);
		file = nullptr;
	}
}
