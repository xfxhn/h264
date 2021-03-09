#include "AnnexBReader.h"
//
//std::cout << slice << std::endl;
//std::cout << blockSize << std::endl;
//std::cout << (int)data[0] << std::endl;
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

    NaluUnit naluArr[1024];
    
    //当前在哪个字节
    unsigned int slice = 0;

   
    //当前startcode的长度
    int currentCodeLen = 0;

    unsigned int i = 0;
    int start = 0;
    while (slice <= dataLen)
    {
        //获取startCode占取几个字节
        bool flag = CheckStartCode(currentCodeLen, buffer + slice, dataLen, slice);
        uint8_t* data = nullptr;
        if (flag)
        {
            ++i;



            if (slice > 0)
            {
                
                //当前走到字节减去上个code码长度和上个走到的字节
                uint32_t blockSize = slice - start;

                data = new uint8_t[blockSize];
                //start是上个的位置加上上个的code码
                memcpy(data, buffer + start, blockSize);
       
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

    std::cout << i << std::endl;

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
AnnexBReader::~AnnexBReader()
{
}
