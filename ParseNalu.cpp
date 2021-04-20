#include "ParseNalu.h"

ParseNalu::ParseNalu()
{
    forbidden_zero_bit = 0;
    nal_ref_idc = 0;
    nal_unit_type = 0;
    svc_extension_flag = 0;
    avc_3d_extension_flag = 0;
    emulation_prevention_three_byte = 0;
   /* NumBytesInNALunit = numBytesInNALunit;
    NumBytesInRBSP = 0;*/
    rbsp_byte = NULL;
    IdrPicFlag = false;
    //m_is_malloc_mem_self = 0;

   /* if (NumBytesInNALunit > 0)
    {
        rbsp_byte = (uint8_t*)my_malloc(sizeof(uint8_t) * NumBytesInNALunit);
        RETURN_IF_FAILED(rbsp_byte == NULL, -1);
        memset(rbsp_byte, 0, sizeof(uint8_t) * NumBytesInNALunit);
        m_is_malloc_mem_self = 1;
    }*/
}

bool ParseNalu::getH264RbspFromNalUnit(BitStream& bs)
{
    forbidden_zero_bit = bs.readMultiBit(1);
    nal_ref_idc = bs.readMultiBit(2);
    nal_unit_type = bs.readMultiBit(5);

    if (nal_unit_type == 5)
    {
        IdrPicFlag = true;
    }


    return false;
}

ParseNalu::~ParseNalu()
{
}
