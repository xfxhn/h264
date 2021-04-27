#include "AnnexBReader.h"



const int AnnexBReader::MAX_BUFFER_SIZE = 5 * 1024 * 1024;
AnnexBReader::AnnexBReader()
{

	ppsCache = new ParsePPS[256];
	spsCache = new ParseSPS[32];


	slice = nullptr;
}

bool AnnexBReader::open(const char* filePath)
{
	f = fopen(filePath, "rb");
	if (f == nullptr) {
		cout << "fopen打开失败" << endl;
		return false;
	}

	uint8_t* buffer = new uint8_t[AnnexBReader::MAX_BUFFER_SIZE];

	rsize_t bufferSize = fread(buffer, 1, AnnexBReader::MAX_BUFFER_SIZE, f);

	ReadNalu(buffer, bufferSize);
	return true;
}
bool AnnexBReader::close()
{
	return false;
}

int AnnexBReader::ReadNalu(uint8_t* buffer, rsize_t& dataLen)
{
	size_t currentLenth = 0;

	//当前在哪个字节
	unsigned int slice = 0;

	//当前startcode的长度
	int currentCodeLen = 0;

	int start = 0;
	uint8_t* data = nullptr;


	while (slice <= dataLen)
	{
		//获取startCode占取几个字节
		bool flag = CheckStartCode(currentCodeLen, buffer + slice, dataLen);


		if (flag)
		{
			if (slice > 0)
			{

				//当前走到字节减去上个code码长度和上个走到的字节
				uint32_t blockSize = slice - start;

				data = new uint8_t[blockSize];
				//start是上个的位置加上上个的code码
				memcpy(data, buffer + start, blockSize);
				size_t size = unescape(data, blockSize);


				getNaluHeader(data, size);

			}


			start = currentCodeLen + slice;

			slice += currentCodeLen;

			if (data != nullptr)
			{
				delete[] data;
				data = nullptr;
			}
		}
		//读到文件结尾
		if (slice >= dataLen)
		{
			uint32_t blockSize = slice - start;

			data = new uint8_t[blockSize];

			memcpy(data, buffer + start, blockSize);
			size_t size = unescape(data, blockSize);
			printHex(data, size);
			//getNaluHeader(data, size);
			if (data != nullptr)
			{
				delete[] data;
				data = nullptr;
			}
		}
		++slice;
	}

	return 0;
}

bool AnnexBReader::CheckStartCode(int& startCodeLen, uint8_t* bufPtr, int bufLen)
{
	if (bufLen <= 2) {
		startCodeLen = 0;
		return false;
	}


	if (bufLen >= 4) {
		if (bufPtr[0] == 0) {
			if (bufPtr[1] == 0) {
				if (bufPtr[2] == 0) {
					if (bufPtr[3] == 1) {
						startCodeLen = 4;
						return true;
					}
				}
				if (bufPtr[2] == 1) {
					startCodeLen = 3;
					return true;
				}
			}
		}
	}
	if (bufLen <= 3) {
		if (bufPtr[0] == 0) {
			if (bufPtr[1] == 0) {
				if (bufPtr[2] == 1) {
					startCodeLen = 3;
					return true;
				}
			}
		}
	}

	startCodeLen = 0;
	return false;
}



//"0x00000001"分割出来的比特流即是NAL unit，
//起始码紧跟的第一个字节是NAL header
//NALU由一个字节的Header和RBSP组成.
//forbidden_zero_bit    禁止位，正常情况下为 0。
//在某些情况下，如果 NALU 发生丢失数据的情况，可以将这一位置为 1，以便接收方纠错或丢掉该单元。
//nal_ref_idc   该元素表示这个 NALU 的重要性。可能的值有 4 个,越重要的 NALU 越不能丢弃。
//nal_unit_type  
void AnnexBReader::getNaluHeader(uint8_t* buffer, int size)
{



	BitStream bs(buffer, size);

	ParseNalu nalu;

	nalu.getH264RbspFromNalUnit(bs);

	switch ((NaluType)nalu.nal_unit_type)
	{
	case NaluType::H264_NAL_SLICE: //1
	{
		// std::cout << NaluType::PPS << std::endl;
		break;
	}
	case NaluType::H264_NAL_DPA: //2
	{
		break;
	}
	case NaluType::H264_NAL_DPB: //3
	{
		break;
	}
	case NaluType::H264_NAL_DPC: //4
	{
		break;
	}
	case NaluType::H264_NAL_IDR_SLICE: //5 立即刷新帧（每个GOP的第一帧必须是I帧）
	{
		if (slice)
		{
			delete slice;
			slice = nullptr;
		}
		slice = new ParseSlice(nalu);
		slice->parse(bs, ppsCache, spsCache);

		break;
	}
	case NaluType::H264_NAL_SEI: //6
	{
		break;
	}
	case NaluType::H264_NAL_SPS: //7
	{

		ParseSPS sps;
		bool ret = sps.seq_parameter_set_data(bs);

		spsCache[sps.seq_parameter_set_id] = sps;

		break;
	}
	case NaluType::H264_NAL_PPS: //8
	{
		ParsePPS pps;

		bool ret = pps.pic_parameter_set_rbsp(bs, spsCache);

		ppsCache[pps.pic_parameter_set_id] = pps;

		break;
	}
	case NaluType::H264_NAL_SPS_EXT: //13
	{
		break;
	}
	default:

		break;
	}
}
//去掉防竞争字节
size_t AnnexBReader::unescape(uint8_t* src, uint32_t length)
{
	size_t size = length;

	for (size_t start = 0; start < length; start++)
	{
		if (src[start] == 0 && src[start + 1] == 0 && src[start + 2] == 3 && src[start + 3] <= 3)
		{
			--size;
			size_t index = start + 2;
			for (size_t j = index; j < length - 1; j++)
			{
				src[j] = src[j + 1];
			}
		}
	}

	return size;
}



// profile_idc 标识当前H.264码流的profile。我们知道，H.264中定义了三种常用的档次profile：
//基准档次：baseline profile;
//主要档次：main profile;
//扩展档次：extended profile;
AnnexBReader::~AnnexBReader()
{

	if (slice)
	{
		delete slice;
		slice = nullptr;
	}
	if (ppsCache)
	{
		delete[] ppsCache;
		ppsCache = nullptr;
	}
	if (spsCache)
	{
		delete[] spsCache;
		spsCache = nullptr;
	}
}

