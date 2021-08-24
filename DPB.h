#pragma once
#include "ParseSlice.h"
#include "Picture.h"
#include "Array.hpp"
class DPB
{
public:
	Array<Picture*> dpb;
	/*Picture* dpb[16];*/
	Picture* fs_ref[16];  //短期参考图像列表(当然，在这里是未经排序的)
	Picture* fs_ltref[16]; //长期参考图像列表
	uint8_t	 size; //dpb最大容量，能容得下多少帧,最大不超过16
	uint8_t	 used_size; //dpb实际存了多少帧,以帧为单位,如果只有其中一场,也被看做一帧

public:
	DPB();
	void Decoded_reference_picture_marking_process(ParseSlice* slice);
};

