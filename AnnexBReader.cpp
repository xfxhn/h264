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
	//����ֻ������һ��������
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
				//��·�˲���
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
				cout << "��������һ֡" << endl;
			}
			else
			{
				//�Ƿ��ǵ�һ֡�ĵ�һ��slice
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
		case NaluType::H264_NAL_IDR_SLICE: //5 ����ˢ��֡��ÿ��GOP�ĵ�һ֡������I֡��
		{
			SliceHeader sHeader(nalu);
			sHeader.slice_header(bs, ppsCache, spsCache);

			if (this->slice && this->slice->decodeFinished)
			{
				//��·�˲���
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
				cout << "��������һ֡" << endl;
			}
			else
			{
				//�Ƿ��ǵ�һ֡�ĵ�һ��slice
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




//"0x00000001"�ָ�����ı���������NAL unit��
//��ʼ������ĵ�һ���ֽ���NAL header
//NALU��һ���ֽڵ�Header��RBSP���.
//forbidden_zero_bit    ��ֹλ�����������Ϊ 0��
//��ĳЩ����£���� NALU ������ʧ���ݵ���������Խ���һλ��Ϊ 1���Ա���շ�����򶪵��õ�Ԫ��
//nal_ref_idc   ��Ԫ�ر�ʾ��� NALU ����Ҫ�ԡ����ܵ�ֵ�� 4 ��,Խ��Ҫ�� NALU Խ���ܶ�����
//nal_unit_type  




// profile_idc ��ʶ��ǰH.264������profile������֪����H.264�ж��������ֳ��õĵ���profile��
//��׼���Σ�baseline profile;
//��Ҫ���Σ�main profile;
//��չ���Σ�extended profile;
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

