#pragma once
#include "Common.h"
#include "SliceHeader.h"
class SliceData
{
public:
	uint32_t  cabac_alignment_one_bit;
	uint32_t  mb_skip_run;
public:

	SliceData();
	bool slice_data(BitStream& bs, SliceHeader& sHeader);
	~SliceData();
};

