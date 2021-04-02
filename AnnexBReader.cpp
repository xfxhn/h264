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
        cout << "打开失败" << endl;
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
    
    //当前在哪个字节
    unsigned int slice = 0;
   
    //当前startcode的长度
    int currentCodeLen = 0;

    int start = 0;

    while (slice <= dataLen)
    {
        //获取startCode占取几个字节
        bool flag = CheckStartCode(currentCodeLen, buffer + slice, dataLen);

        uint8_t* data = nullptr;
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
    case NaluType::H264_NAL_IDR_SLICE: //5 立即刷新帧（每个GOP的第一帧必须是I帧）
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

//void AnnexBReader::parseSPS(uint8_t* bufPtr, int size)
//{
//    
//    int profile_idc = *(bufPtr+1);
//
//
//    const uint8_t byte = *(bufPtr + 2);
//    //当 constrained_set0_flag 值为 1 的时候，
//    //就说明码流应该遵循基线 profile(Baseline profile) 的所有约束 .constrained_set0_flag 值为 0 时，
//    //说明码流不一定要遵循基线 profile 的所有约束。
//    uint32_t constrained_set0_flag = (byte >> 7) & 1;
//    //当 constrained_set1_flag 值为 1 的时候，就说明码流应该遵循主 profile(Main profile) 的所有约束 .
//    //constrained_set1_flag 值为 0 时，说明码流不一定要遵循主 profile 的所有约束。
//    uint32_t constrained_set1_flag = (byte >> 6) & 1;
//    /*当 constrained_set2_flag 值为 1 的时候，就说明码流应该遵循扩展 profile(Extended profile) 的所有约束.
//    constrained_set2_flag 值为 0 时，说明码流不一定要遵循扩展 profile 的所有约束。
//    注意：当 constraint_set0_flag, constraint_set1_flag 或 constraint_set2_flag 中不只一个值为 1 的话，
//    那么码流必须满足所有相应指明的 profile 约束。*/
//    uint32_t constrained_set2_flag = (byte >> 5) & 1;
//    uint32_t constrained_set3_flag = (byte >> 4) & 1;
//    uint32_t constrained_set4_flag = (byte >> 3) & 1;
//    uint32_t constrained_set5_flag = (byte >> 2) & 1;
//    uint32_t reserved_zero_2bits = byte & 3;
//
//    //level_idc:最大支持码流范围，表示当前码流的level
//    //定义了某种条件下的最大视频分辨率，最大视频帧率等参数
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
//    //值为1的，而且level_idc小于等于指定level级的码流。
//    
//
//    BitStream decode(bufPtr+4,size-4);
//    //seq_parameter_set_id 指定了由图像参数集指明的序列参数集。 seq_parameter_set_id 值应该是从 0 到 31 ，包括 0 和 31
//    //注意： 当可用的情况下，编码器应该在 sps 值不同的情况下使用不同的 seq_parameter_set_id 值，
//    //而不是变化某一特定值的 seq_parameter_set_id 的参数集的语法结构中的值。
//    uint32_t seq_parameter_set_id = decode.readUE();
//    uint32_t chroma_format_idc = decode.readUE();
//    /*std::cout << code << std::endl;
//    std::cout << chroma_format_idc << std::endl;
//    std::cout << "---------" << std::endl;*/
//
//}






// profile_idc 标识当前H.264码流的profile。我们知道，H.264中定义了三种常用的档次profile：
//基准档次：baseline profile;
//主要档次：main profile;
//扩展档次：extended profile;
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

