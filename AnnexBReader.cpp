#include "AnnexBReader.h"



const int AnnexBReader::MAX_BUFFER_SIZE = 50 * 1024 * 1024;
AnnexBReader::AnnexBReader()
{
    slice = nullptr;
}

bool AnnexBReader::open(const char *filePath)
{
	f = fopen(filePath, "rb");
    if (f == nullptr) {
        cout << "��ʧ��" << endl;
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
    size_t currentLenth=0;
    
    //��ǰ���ĸ��ֽ�
    unsigned int slice = 0;
   
    //��ǰstartcode�ĳ���
    int currentCodeLen = 0;

    int start = 0;

    while (slice <= dataLen)
    {
        //��ȡstartCodeռȡ�����ֽ�
        bool flag = CheckStartCode(currentCodeLen, buffer + slice, dataLen);

        uint8_t* data = nullptr;
        if (flag)
        {
            if (slice > 0)
            {
                
                //��ǰ�ߵ��ֽڼ�ȥ�ϸ�code�볤�Ⱥ��ϸ��ߵ����ֽ�
                uint32_t blockSize = slice - start;

                data = new uint8_t[blockSize];
                //start���ϸ���λ�ü����ϸ���code��
                memcpy(data, buffer + start, blockSize);
                size_t size = unescape(data, blockSize);
              
                
                getNaluHeader(data, size);
                
               /* NaluUnit nalu;
                nalu.data = data;
                nalu.size = blockSize;*/
                
            }
           
            
            start = currentCodeLen + slice;

            slice += currentCodeLen;

            if (data != nullptr)
            {
                delete[] data;
                data = nullptr;
            }
        }
        //�����ļ���β
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



//"0x00000001"�ָ�����ı���������NAL unit��
//��ʼ������ĵ�һ���ֽ���NAL header
//NALU��һ���ֽڵ�Header��RBSP���.
//forbidden_zero_bit    ��ֹλ�����������Ϊ 0��
//��ĳЩ����£���� NALU ������ʧ���ݵ���������Խ���һλ��Ϊ 1���Ա���շ�����򶪵��õ�Ԫ��
//nal_ref_idc   ��Ԫ�ر�ʾ��� NALU ����Ҫ�ԡ����ܵ�ֵ�� 4 ��,Խ��Ҫ�� NALU Խ���ܶ�����
//nal_unit_type  
void AnnexBReader::getNaluHeader(uint8_t* buffer,int size)
{


    
    BitStream bs(buffer, size);

    ParseNalu nalu;

    nalu.getH264RbspFromNalUnit(bs);
    /*int forbidden_zero_bit = bs.readMultiBit(1);
    int nal_ref_idc = bs.readMultiBit(2);
    int nal_unit_type = bs.readMultiBit(5);*/



    /*int forbidden_zero_bit = (*buffer >> 7) & 1;
    int nal_ref_idc = (*buffer >> 5) & 3;
    int nal_unit_type = *buffer & 31;*/
   
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
    case NaluType::H264_NAL_IDR_SLICE: //5 ����ˢ��֡��ÿ��GOP�ĵ�һ֡������I֡��
    {
        if (slice)
        {
            delete slice;
            slice = nullptr;
        }
        
        //BitStream bs(buffer + 1, size);

        slice = new ParseSlice(nalu);
        slice->slice_header(bs, ppsCache, spsCache);
       
        break;
    }
    case NaluType::H264_NAL_SEI: //6
    {
        break;
    }
    case NaluType::H264_NAL_SPS: //7
    {
        //ParseSPS sps;
        //BitStream bs(buffer + 1, size);
        ParseSPS sps;
        bool ret = sps.seq_parameter_set_data(bs);

        spsCache[sps.seq_parameter_set_id] = sps;
        //spsCache[pps->seq_parameter_set_id] = sps;
        //spsCache.push_back(sps);
        break;
    }
    case NaluType::H264_NAL_PPS: //8
    {
        ParsePPS pps;

        //BitStream bs(buffer + 1, size);

        bool ret = pps.pic_parameter_set_rbsp(bs);

        ppsCache[pps.pic_parameter_set_id] = pps;
        //ppsCache.push_back(pps);
        
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
//ȥ���������ֽ�
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

//void AnnexBReader::parseSPS(uint8_t* bufPtr, int size)
//{
//    
//    int profile_idc = *(bufPtr+1);
//
//
//    const uint8_t byte = *(bufPtr + 2);
//    //�� constrained_set0_flag ֵΪ 1 ��ʱ��
//    //��˵������Ӧ����ѭ���� profile(Baseline profile) ������Լ�� .constrained_set0_flag ֵΪ 0 ʱ��
//    //˵��������һ��Ҫ��ѭ���� profile ������Լ����
//    uint32_t constrained_set0_flag = (byte >> 7) & 1;
//    //�� constrained_set1_flag ֵΪ 1 ��ʱ�򣬾�˵������Ӧ����ѭ�� profile(Main profile) ������Լ�� .
//    //constrained_set1_flag ֵΪ 0 ʱ��˵��������һ��Ҫ��ѭ�� profile ������Լ����
//    uint32_t constrained_set1_flag = (byte >> 6) & 1;
//    /*�� constrained_set2_flag ֵΪ 1 ��ʱ�򣬾�˵������Ӧ����ѭ��չ profile(Extended profile) ������Լ��.
//    constrained_set2_flag ֵΪ 0 ʱ��˵��������һ��Ҫ��ѭ��չ profile ������Լ����
//    ע�⣺�� constraint_set0_flag, constraint_set1_flag �� constraint_set2_flag �в�ֻһ��ֵΪ 1 �Ļ���
//    ��ô������������������Ӧָ���� profile Լ����*/
//    uint32_t constrained_set2_flag = (byte >> 5) & 1;
//    uint32_t constrained_set3_flag = (byte >> 4) & 1;
//    uint32_t constrained_set4_flag = (byte >> 3) & 1;
//    uint32_t constrained_set5_flag = (byte >> 2) & 1;
//    uint32_t reserved_zero_2bits = byte & 3;
//
//    //level_idc:���֧��������Χ����ʾ��ǰ������level
//    //������ĳ�������µ������Ƶ�ֱ��ʣ������Ƶ֡�ʵȲ���
//    /*10	1       (supports only QCIF format and below with 380160 samples / sec)
//    11	1.1    (CIF and below. 768000 samples / sec)
//    12	1.2    (CIF and below. 1536000 samples / sec)
//    13	1.3    (CIF and below. 3041280 samples / sec)
//    20	2       (CIF and below. 3041280 samples / sec)
//    21	2.1    (Supports HHR formats.Enables Interlace support. 5068800 samples / sec)
//    22	2.2    (Supports SD / 4CIF formats.Enables Interlace support. 5184000 samples / sec)
//    30	3       (Supports SD / 4CIF formats.Enables Interlace support. 10368000 samples / sec)
//    31	3.1    (Supports 720p HD format.Enables Interlace support. 27648000 samples / sec)
//    32	3.2    (Supports SXGA format.Enables Interlace support. 55296000 samples / sec)
//    40	4       (Supports 2Kx1K format.Enables Interlace support. 62914560 samples / sec)
//    41	4.1    (Supports 2Kx1K format.Enables Interlace support. 62914560 samples / sec)
//    42	4.2    (Supports 2Kx1K format.Frame coding only. 125829120 samples / sec)
//    50	5       (Supports 3672x1536 format.Frame coding only. 150994944 samples / sec)
//    51	5.1    (Supports 4096x2304 format.Frame coding only. 251658240 samples / sec)*/
//    int level_idc = *(bufPtr+3);
//    /*std::cout << profile_idc << std::endl;
//    std::cout << level_idc << std::endl;*/
//    //ֵΪ1�ģ�����level_idcС�ڵ���ָ��level����������
//    
//
//    BitStream decode(bufPtr+4,size-4);
//    //seq_parameter_set_id ָ������ͼ�������ָ�������в������� seq_parameter_set_id ֵӦ���Ǵ� 0 �� 31 ������ 0 �� 31
//    //ע�⣺ �����õ�����£�������Ӧ���� sps ֵ��ͬ�������ʹ�ò�ͬ�� seq_parameter_set_id ֵ��
//    //�����Ǳ仯ĳһ�ض�ֵ�� seq_parameter_set_id �Ĳ��������﷨�ṹ�е�ֵ��
//    uint32_t seq_parameter_set_id = decode.readUE();
//    uint32_t chroma_format_idc = decode.readUE();
//    /*std::cout << code << std::endl;
//    std::cout << chroma_format_idc << std::endl;
//    std::cout << "---------" << std::endl;*/
//
//}






// profile_idc ��ʶ��ǰH.264������profile������֪����H.264�ж��������ֳ��õĵ���profile��
//��׼���Σ�baseline profile;
//��Ҫ���Σ�main profile;
//��չ���Σ�extended profile;
AnnexBReader::~AnnexBReader()
{
   /* if (sps)
    {
        delete sps;
        sps = nullptr;
    }*/
 

    if (slice)
    {
        delete slice;
        slice = nullptr;
    }
}

