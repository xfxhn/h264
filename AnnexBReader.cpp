#include "AnnexBReader.h"



//对于IDR帧来说，在IDR帧之后的所有帧都不能引用任何IDR帧之前的帧的内容，
//与此相反，对于普通的I - 帧来说，位于其之后的B - 和P - 帧可以引用位于普通I - 帧之前的I - 帧。
//从随机存取的视频流中，播放器永远可以从一个IDR帧播放，因为在它之后没有任何帧引用之前的帧。
//但是，不能在一个没有IDR帧的视频中从任意点开始播放，因为后面的帧总是会引用前面的帧。

//nal_unit_type	NALU 类型
//0	未定义
//1	这个 nalu 被称之为 Slice，这里就是除了 IDR Slice 以外     
//2	非 IDR SLICE，采用 A 类数据划分片段	slice_data_partition_a_layer_rbsp()
//3	非 IDR SLICE，采用 B 类数据划分片段	slice_data_partition_b_layer_rbsp()
//4	非 IDR SLICE，采用 C 类数据划分片段	slice_data_partition_c_layer_rbsp()

//5	IDR SLICE	nalu 被称之为 IDR 帧，也叫做关键帧   即时解码刷新 I和IDR帧都是使用帧内预测的。
//5 它们都是同一个东西而已,在编码和解码中为了方便，要首个I帧和其他I帧区别开，所以才把第一个首个I帧叫IDR,IDR帧的作用是立刻刷新,使错误不致传播

//6	补充增强信息 SEI	sei_rbsp()
//7	序列参数集 SPS	seq_parameter_set_rbsp()
//8	图像参数集 PPS	pic_parameter_set_rbsp()
//9	分隔符	access_unit_delimiter_rbsp()
//10	序列结束符	end_of_seq_rbsp()
//11	码流结束符	end_of_stream_rbsp()
//12	填充数据	filler_data_rbsp()
//13	序列参数扩展集	seq_parameter_set_extension_rbsp()
//14~18	保留
//19	未分割的辅助编码图像的编码条带	slice_layer_without_partitioning_rbsp()
//20~23	保留
//24~31	未指定



const int AnnexBReader::MAX_BUFFER_SIZE = 50 * 1024 * 1024;
AnnexBReader::AnnexBReader()
{
}

bool AnnexBReader::open(const char *filePath)
{
	f = fopen(filePath, "rb");
    if (f == nullptr) {
        return false;
    }

    //uint8_t* buffer = new uint8_t[AnnexBReader::MAX_BUFFER_SIZE];
    //uint32_t count;
    //while ((count = fread(buffer, 1, AnnexBReader::MAX_BUFFER_SIZE, f)) > 0)
    //{
    //    if (count < AnnexBReader::MAX_BUFFER_SIZE)
    //        break;
    //    //置字节字符串前n个字节为零且包括‘\0’。
    //    //bzero(buffer, AnnexBReader::MAX_BUFFER_SIZE);
    //    
    //}



    

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

   // NaluUnit naluArr[1024];
    
    //当前在哪个字节
    unsigned int slice = 0;

   
    //当前startcode的长度
    int currentCodeLen = 0;

    int start = 0;
    while (slice <= dataLen)
    {
        //获取startCode占取几个字节
        bool flag = CheckStartCode(currentCodeLen, buffer + slice, dataLen, slice);
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
                if (slice<100000)
                {
                    getNaluHeader(data[0]);
                }
                
                NaluUnit nalu;
                nalu.data = data;
                nalu.size = blockSize;
                
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

bool AnnexBReader::CheckStartCode(int& startCodeLen, uint8_t* bufPtr, int bufLen,int slice)
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

int AnnexBReader::ReadFromFile()
{
    return 0;
    /*const int tempBufferLen = 1024;
    uint8_t* buf = new uint8_t[tempBufferLen];
    int readedLen = fread(buf, 1, tempBufferLen, f);

    if (readedLen > 0) {


        uint8_t* _buffer = (uint8_t*)malloc(bufferLen + tempBufferLen);
        memcpy(_buffer, buffer, bufferLen);
        memcpy(_buffer + bufferLen, buf, tempBufferLen);
        bufferLen = bufferLen + tempBufferLen;

        if (buffer != nullptr) {
            free(buffer);
            buffer = nullptr;
        }

        buffer = _buffer;
    }

    delete[] buf;

    return readedLen;*/
}

//"0x00000001"分割出来的比特流即是NAL unit，
//起始码紧跟的第一个字节是NAL header
//NALU由一个字节的Header和RBSP组成.
void AnnexBReader::getNaluHeader(uint8_t byte)
{
    //右移7位，然后&  0000 0001就能得出第一位是多少
    std::cout << ((byte >> 7) & 1) << std::endl;
    //右移5位，然后&  0000 0011就能得出第2和3和是多少
    std::cout << ((byte >> 5) & 3) << std::endl;
    std::cout <<"nal_unit_type"<< (byte & 31) << std::endl;
}
AnnexBReader::~AnnexBReader()
{
}

