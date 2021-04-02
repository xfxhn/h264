#pragma once
#include "Common.h"


class ParseNalu
{
public:
    bool     forbidden_zero_bit; // All f(1)
    uint32_t     nal_ref_idc; // All u(2)
    uint32_t     nal_unit_type; // All u(5)
    bool     svc_extension_flag; // All u(1)
    bool     avc_3d_extension_flag; // All u(1)
    uint32_t     emulation_prevention_three_byte; // /* equal to 0x03 */ All f(8)
    uint8_t* rbsp_byte; //my_malloc(NumBytesInNALunit);    Raw byte sequence payloads.
    /*int32_t     NumBytesInNALunit;
    int32_t     NumBytesInRBSP;*/
    bool     IdrPicFlag; //是否是立即刷新帧的标记
    //int32_t     m_is_malloc_mem_self;
public:
	ParseNalu();
    bool getH264RbspFromNalUnit(BitStream& bs);
	~ParseNalu();
};

