#include "AnnexBReader.h"


AnnexBReader::AnnexBReader()
{

	ppsCache = new ParsePPS[256];
	spsCache = new ParseSPS[32];

	slice = nullptr;
}

bool AnnexBReader::open(const char* filePath)
{

	FileReader file(filePath);

	size_t size = 0;
	uint8_t* data = nullptr;
	bool isStopLoop = true;
	ParseNalu nalu;
	//这样只是生命一个函数！
	/*SliceHeader lastHeader();
	SliceHeader lastHeader(ParseNalu());*/

	SliceHeader lastHeader(nalu);
	int nal_cnt = 0;

	DPB dpb;
	while (isStopLoop) {

		file.readNalUint(data, size, isStopLoop);

		BitStream bs(data, size);

		nalu.getH264RbspFromNalUnit(bs);


		switch ((NaluType)nalu.nal_unit_type)
		{
		case NaluType::H264_NAL_SLICE: //1
		{

			SliceHeader sHeader(nalu);
			sHeader.slice_header(bs, ppsCache, spsCache);


			if (this->slice && this->slice->decodeFinished)
			{
				//环路滤波器
				slice->Deblocking_filter_process(lastHeader);

				dpb.Decoding_to_complete(slice, lastHeader);

				char buf[50];
				sprintf(buf, "./output/xf-%d-%d.bmp", nal_cnt, lastHeader.slice_type);

				slice->saveBmpFile(buf);
				nal_cnt++;

				if (this->slice)
				{
					delete this->slice;
					this->slice = nullptr;
				}


				this->slice = new ParseSlice(sHeader);
				this->slice->parse();
				cout << "解码完这一帧" << endl;
			}
			else
			{
				//是否是第一帧的第一个slice
				if (sHeader.first_mb_in_slice == 0)
				{
					if (slice)
					{
						delete slice;
						slice = nullptr;
					}

					this->slice = new ParseSlice(sHeader);
					this->slice->parse();
				}
			}

			if (this->slice)
			{
				this->slice->sliceNumber = sHeader.first_mb_in_slice;
			}
			lastHeader = sHeader;
			SliceData sData;

			sData.slice_data(bs, slice, dpb, nal_cnt);

			break;
		}
		case NaluType::H264_NAL_DPA: //2
		{
			int a = 1;
			break;
		}
		case NaluType::H264_NAL_DPB: //3
		{
			int a = 1;
			break;
		}
		case NaluType::H264_NAL_DPC: //4
		{
			int a = 1;
			break;
		}
		case NaluType::H264_NAL_IDR_SLICE: //5 立即刷新帧（每个GOP的第一帧必须是I帧）
		{
			SliceHeader sHeader(nalu);
			sHeader.slice_header(bs, ppsCache, spsCache);

			if (this->slice && this->slice->decodeFinished)
			{
				//环路滤波器
				slice->Deblocking_filter_process(lastHeader);

				dpb.Decoding_to_complete(slice, lastHeader);

				slice->saveBmpFile("xf");

				if (slice)
				{
					delete slice;
					slice = nullptr;
				}


				this->slice = new ParseSlice(sHeader);
				this->slice->parse();
				cout << "解码完这一帧" << endl;
			}
			else
			{
				//是否是第一帧的第一个slice
				if (sHeader.first_mb_in_slice == 0)
				{
					if (slice)
					{
						delete slice;
						slice = nullptr;
					}

					this->slice = new ParseSlice(sHeader);
					this->slice->parse();
				}
			}

			if (this->slice)
			{
				this->slice->sliceNumber = sHeader.first_mb_in_slice;
			}

			lastHeader = sHeader;

			SliceData sData;
			sData.slice_data(bs, slice, dpb, nal_cnt);

			break;
		}
		case NaluType::H264_NAL_SEI: //6
		{
			ParseSEI sei;
			sei.sei_rbsp(bs);
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

	return true;
}




//"0x00000001"分割出来的比特流即是NAL unit，
//起始码紧跟的第一个字节是NAL header
//NALU由一个字节的Header和RBSP组成.
//forbidden_zero_bit    禁止位，正常情况下为 0。
//在某些情况下，如果 NALU 发生丢失数据的情况，可以将这一位置为 1，以便接收方纠错或丢掉该单元。
//nal_ref_idc   该元素表示这个 NALU 的重要性。可能的值有 4 个,越重要的 NALU 越不能丢弃。
//nal_unit_type  




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

