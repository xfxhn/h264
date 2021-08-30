#pragma once
#include "SliceHeader.h"
#include "Picture.h"
#include "Array.hpp"
class ParseSlice;
class DPB
{
public:
	Array<Picture*> dpb;
	Picture* previous;

	/*Picture* dpb[16];*/
	//Picture* fs_ref[16];  //���ڲο�ͼ���б�(��Ȼ����������δ�������)
	//Picture* fs_ltref[16]; //���ڲο�ͼ���б�
	//uint8_t	 size; //dpb������������ݵ��¶���֡,��󲻳���16
	//uint8_t	 used_size; //dpbʵ�ʴ��˶���֡,��֡Ϊ��λ,���ֻ������һ��,Ҳ������һ֡

public:
	DPB();
	void Decoding_to_complete(ParseSlice* slice, const  SliceHeader& sHeader);


	void Decoded_reference_picture_marking_process(Picture* pic, const  SliceHeader& sHeader);

	void Adaptive_memory_control_decoded_reference_picture_marking_process(const SliceHeader& sHeader, Picture* pic);

	void Sliding_window_decoded_reference_picture_marking_process(uint8_t max_num_ref_frames);
};

