#include "ParseSEI.h"

ParseSEI::ParseSEI()
{
	last_payload_type_byte = 0;
	last_payload_size_byte = 0;

}

void ParseSEI::sei_rbsp(BitStream& bs)
{
	do
	{
		sei_message(bs);
	} while (bs.more_rbsp_data());

	//把剩下的填充字节读取
	rbsp_trailing_bits(bs);
}

void ParseSEI::sei_message(BitStream& bs)
{
	int payloadType = 0;

	while (bs.getMultiBit(8) == 0xFF)
	{
		int ff_byte = bs.readMultiBit(8); // /* equal to 0xFF */ 5 f(8)
		payloadType += 255;
	}
	last_payload_type_byte = bs.readMultiBit(8);

	payloadType += last_payload_type_byte;

	int payloadSize = 0;
	while (bs.getMultiBit(8) == 0xFF)
	{
		int ff_byte = bs.readMultiBit(8); // /* equal to 0xFF */ 5 f(8)
		payloadSize += 255;
	}

	last_payload_size_byte = bs.readMultiBit(8);

	payloadSize += last_payload_size_byte;

}

void ParseSEI::sei_payload(BitStream& bs, int payloadType, int payloadSize)
{
	if (!byte_aligned(bs))
	{
		int bit_equal_to_one = bs.readBit(); // /* equal to 1 */ f(1)
		while (!byte_aligned(bs))
			int bit_equal_to_zero = bs.readBit(); // /* equal to 0 */ f(1)
	}
}
