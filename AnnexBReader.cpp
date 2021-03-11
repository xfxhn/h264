#include "AnnexBReader.h"



//����IDR֡��˵����IDR֮֡�������֡�����������κ�IDR֮֡ǰ��֡�����ݣ�
//����෴��������ͨ��I - ֡��˵��λ����֮���B - ��P - ֡��������λ����ͨI - ֮֡ǰ��I - ֡��
//�������ȡ����Ƶ���У���������Զ���Դ�һ��IDR֡���ţ���Ϊ����֮��û���κ�֡����֮ǰ��֡��
//���ǣ�������һ��û��IDR֡����Ƶ�д�����㿪ʼ���ţ���Ϊ�����֡���ǻ�����ǰ���֡��

//nal_unit_type	NALU ����
//0	δ����
//1	��� nalu ����֮Ϊ Slice��������ǳ��� IDR Slice ����     
//2	�� IDR SLICE������ A �����ݻ���Ƭ��	slice_data_partition_a_layer_rbsp()
//3	�� IDR SLICE������ B �����ݻ���Ƭ��	slice_data_partition_b_layer_rbsp()
//4	�� IDR SLICE������ C �����ݻ���Ƭ��	slice_data_partition_c_layer_rbsp()

//5	IDR SLICE	nalu ����֮Ϊ IDR ֡��Ҳ�����ؼ�֡   ��ʱ����ˢ�� I��IDR֡����ʹ��֡��Ԥ��ġ�
//5 ���Ƕ���ͬһ����������,�ڱ���ͽ�����Ϊ�˷��㣬Ҫ�׸�I֡������I֡���𿪣����ԲŰѵ�һ���׸�I֡��IDR,IDR֡������������ˢ��,ʹ�����´���

//6	������ǿ��Ϣ SEI	sei_rbsp()
//7	���в����� SPS	seq_parameter_set_rbsp()
//8	ͼ������� PPS	pic_parameter_set_rbsp()
//9	�ָ���	access_unit_delimiter_rbsp()
//10	���н�����	end_of_seq_rbsp()
//11	����������	end_of_stream_rbsp()
//12	�������	filler_data_rbsp()
//13	���в�����չ��	seq_parameter_set_extension_rbsp()
//14~18	����
//19	δ�ָ�ĸ�������ͼ��ı�������	slice_layer_without_partitioning_rbsp()
//20~23	����
//24~31	δָ��



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
    //    //���ֽ��ַ���ǰn���ֽ�Ϊ���Ұ�����\0����
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
    
    //��ǰ���ĸ��ֽ�
    unsigned int slice = 0;

   
    //��ǰstartcode�ĳ���
    int currentCodeLen = 0;

    int start = 0;
    while (slice <= dataLen)
    {
        //��ȡstartCodeռȡ�����ֽ�
        bool flag = CheckStartCode(currentCodeLen, buffer + slice, dataLen, slice);
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
        //�����ļ���β
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

//"0x00000001"�ָ�����ı���������NAL unit��
//��ʼ������ĵ�һ���ֽ���NAL header
//NALU��һ���ֽڵ�Header��RBSP���.
void AnnexBReader::getNaluHeader(uint8_t byte)
{
    //����7λ��Ȼ��&  0000 0001���ܵó���һλ�Ƕ���
    std::cout << ((byte >> 7) & 1) << std::endl;
    //����5λ��Ȼ��&  0000 0011���ܵó���2��3���Ƕ���
    std::cout << ((byte >> 5) & 3) << std::endl;
    std::cout <<"nal_unit_type"<< (byte & 31) << std::endl;
}
AnnexBReader::~AnnexBReader()
{
}

