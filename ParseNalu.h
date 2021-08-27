#pragma once
#include "Common.h"


class ParseNalu
{
public:
	bool        forbidden_zero_bit; // All f(1)
	uint32_t     nal_ref_idc; // All u(2)
	uint32_t     nal_unit_type; // All u(5)
	bool        svc_extension_flag; // All u(1)
	bool        avc_3d_extension_flag; // All u(1)
	/*int32_t     NumBytesInNALunit;
	int32_t     NumBytesInRBSP;*/
	bool     IdrPicFlag; //是否是立即刷新帧的标记
	//int32_t     m_is_malloc_mem_self;
public:
	ParseNalu();
	ParseNalu& operator=(const ParseNalu& src);
	bool getH264RbspFromNalUnit(BitStream& bs);
	~ParseNalu();
};

