#pragma once
#include "SliceHeader.h"
#include "Picture.h"
#include "Array.hpp"



class ParseSlice;
class DPB
{
public:
	Array<Picture*> dpb;
	//֡������ ���16
	Array<Picture*> RefPicList0;
	Array<Picture*> RefPicList1; //����B����
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


	void Decoding_process_for_reference_picture_lists_construction(ParseSlice* slice);
private:
	void Decoding_process_for_picture_numbers(const SliceHeader& sHeader);
	void Initialisation_process_for_reference_picture_lists(const ParseSlice* slice);

	void Initialisation_process_for_the_reference_picture_list_for_P_and_SP_slices_in_frames();

	void Initialisation_process_for_reference_picture_lists_for_B_slices_in_frames(int POC);

	void Modification_process_for_reference_picture_lists(const SliceHeader& sHeader);


	void Modification_process_of_reference_picture_lists_for_short_term_reference_pictures(int& refIdxLX, int& picNumL0Pred, uint16_t abs_diff_pic_num_minus1,
		uint16_t modification_of_pic_nums_idc, uint8_t num_ref_idx_lX_active_minus1, Array<Picture*>& RefPicListX, const SliceHeader& sHeader);
	void Modification_process_of_reference_picture_lists_for_long_term_reference_pictures(int& refIdxLX, int& picNumL0Pred, uint16_t long_term_pic_num,
		uint8_t num_ref_idx_lX_active_minus1, Array<Picture*>& RefPicListX, const SliceHeader& sHeader);
};

