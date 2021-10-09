#pragma once

#include "Common.h"
class ParseSEI
{
public:
	uint8_t last_payload_type_byte;
	uint8_t last_payload_size_byte;
public:
	ParseSEI();
	void sei_rbsp(BitStream& bs);
	void sei_message(BitStream& bs);
	void sei_payload(BitStream& bs, int payloadType, int payloadSize);
};

