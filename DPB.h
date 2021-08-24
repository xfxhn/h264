#pragma once
#include "ParseSlice.h"
#include "Picture.h"
#include "Array.hpp"
class DPB
{
public:
	Array<Picture*> dpb;
	/*Picture* dpb[16];*/
	Picture* fs_ref[16];  //���ڲο�ͼ���б�(��Ȼ����������δ�������)
	Picture* fs_ltref[16]; //���ڲο�ͼ���б�
	uint8_t	 size; //dpb������������ݵ��¶���֡,��󲻳���16
	uint8_t	 used_size; //dpbʵ�ʴ��˶���֡,��֡Ϊ��λ,���ֻ������һ��,Ҳ������һ֡

public:
	DPB();
	void Decoded_reference_picture_marking_process(ParseSlice* slice);
};

