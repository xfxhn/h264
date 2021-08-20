#pragma once
#include "ParseSlice.h"

class Picture
{
public:
	ParseSlice* picturePreviousRef;  //前一个已经解码完成的帧


	Picture(ParseSlice* a);
};

