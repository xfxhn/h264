#pragma once
#include "Common.h"
#include "SliceHeader.h"
#include "SliceData.h"
#include "ParsePPS.h"
#include "ParseSPS.h"
#include "ParseNalu.h"
#include "Macroblock.h"

class ParseSlice
{
public:
    uint32_t mbNum;
    ParseNalu &nalu;
    Macroblock** macroblock;

public:
    ParseSlice(ParseNalu& nalu);

    bool parse(BitStream& bs, const ParsePPS ppsCache[256], const ParseSPS spsCache[32]);

	~ParseSlice();
};

