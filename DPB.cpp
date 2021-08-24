#include "DPB.h"

DPB::DPB() :dpb(16)
{
}

void DPB::Decoded_reference_picture_marking_process(ParseSlice* slice)
{
	Picture* pic = new Picture(slice);
	/*dpb[0] = new Picture(slice);*/
	dpb.push(new Picture(slice));
}
