#pragma once
#include "SliceHeader.h"
#include "Picture.h"
#include "Array.hpp"



class ParseSlice;
class DPB
{
public:
	Array<Picture*> dpb;
	//帧编码下 最多16
	Array<Picture*> RefPicList0;
	Array<Picture*> RefPicList1; //用于B条带
	Picture* previous;



	/*Picture* dpb[16];*/
	//Picture* fs_ref[16];  //短期参考图像列表(当然，在这里是未经排序的)
	//Picture* fs_ltref[16]; //长期参考图像列表
	//uint8_t	 size; //dpb最大容量，能容得下多少帧,最大不超过16
	//uint8_t	 used_size; //dpb实际存了多少帧,以帧为单位,如果只有其中一场,也被看做一帧

public:
	DPB();
	~DPB();
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

