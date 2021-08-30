#include "DPB.h"
#include "ParseSlice.h"

DPB::DPB() :dpb(16)
{
	previous = nullptr;
}

void DPB::Decoded_reference_picture_marking_process(Picture* pic, const SliceHeader& sHeader)
{

	if (sHeader.nalu.IdrPicFlag)
	{
		//所有参考图像需要被标记为"不用于参考" 
		for (size_t i = 0; i < dpb.length; i++)
		{
			dpb[i]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
		}

		if (sHeader.long_term_reference_flag)
		{
			//该IDR图像需要 被标记为"用于长期参考"
			pic->reference_marked_type = PICTURE_MARKING::LONG_TERM_REFERENCE;
			pic->LongTermFrameIdx = 0;
			pic->MaxLongTermFrameIdx = 0;

		}
		else//sHeader.long_term_reference_flag=0
		{
			//该IDR图像将被标记为"用于短期参考"
			pic->reference_marked_type = PICTURE_MARKING::SHORT_TERM_REFERENCE;
			pic->MaxLongTermFrameIdx = NA; //设置为没有长期索引
		}
	}
	else
	{
		// =0 先入先出（FIFO）：使用滑动窗的机制，先入先出，在这种模式下没有办法对长期参考帧进行操作。
		// =1 自适应标记（marking）：后续码流中会有一系列句法元素显式指明操作的步骤。自适应是指编码器可根据情况随机灵活地作出决策。
		if (sHeader.adaptive_ref_pic_marking_mode_flag)
		{
			//自适应标记过程
			Adaptive_memory_control_decoded_reference_picture_marking_process(sHeader, pic);
		}
		else
		{
			//滑动窗口解码参考图像的标识过程
			Sliding_window_decoded_reference_picture_marking_process(sHeader.sps.max_num_ref_frames);
		}
	}

}


void DPB::Decoding_to_complete(ParseSlice* slice, const  SliceHeader& sHeader)
{
	Picture* pic = new Picture(slice, sHeader);

	if (sHeader.nalu.nal_ref_idc != 0)
	{
		//标记当前解码完成的帧是什么类型，然后放到DBP里去管理
		Decoded_reference_picture_marking_process(pic, sHeader);
	}
	previous = pic;

	dpb.push(pic);
}



void DPB::Sliding_window_decoded_reference_picture_marking_process(uint8_t max_num_ref_frames)
{
	//如果当前编码场是一个互补参考场对的第二个场，并且第一个场已经被标记为“用于短期参考”时，当前图像也应该被标记为“用于短期参考”

	int numShortTerm = 0;
	int numLongTerm = 0;

	for (size_t i = 0; i < dpb.length; i++)
	{
		if (dpb[i]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE)
		{
			numShortTerm++;
		}

		if (dpb[i]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE)
		{
			numLongTerm++;
		}
	}

	if (numShortTerm + numLongTerm == std::max(max_num_ref_frames, (uint8_t)1) && numShortTerm > 0)
	{
		//有着最小 FrameNumWrap 值的短期参考帧必须标记为“不用于参考”
		int FrameNumWrap = -1;
		Picture* pic = nullptr;
		for (size_t i = 0; i < dpb.length; i++)
		{
			if (dpb[i]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE)
			{
				if (FrameNumWrap == -1)
				{
					FrameNumWrap = dpb[i]->FrameNumWrap;
				}

				if (dpb[i]->FrameNumWrap < FrameNumWrap)
				{
					pic = dpb[i];
					FrameNumWrap = dpb[i]->FrameNumWrap;
				}
			}
		}


		if (pic != nullptr)
		{
			pic->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
		}
		//如它是一个帧或场对，那么它的两个场必须均标记为“不用于参考”。
	}
}

void DPB::Adaptive_memory_control_decoded_reference_picture_marking_process(const SliceHeader& sHeader, Picture* pic)
{

	for (size_t i = 0; i < sHeader.dec_ref_pic_markings_size; i++)
	{
		//将一个短期参考图像标记为非参考图像，也 即将一个短期参考图像移出参考帧队列
		if (sHeader.dec_ref_pic_markings[i].memory_management_control_operation == 1)
		{
			int picNumX = sHeader.CurrPicNum - (sHeader.dec_ref_pic_markings[i].difference_of_pic_nums_minus1 + 1);
			//如果field_pic_flag等于0, 则picNumX指定的短期参考帧被标记为“unused for reference”。

			for (size_t j = 0; j < dpb.length; j++)
			{
				if (dpb[j]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE && dpb[j]->PicNum == picNumX)
				{
					dpb[j]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
				}
			}
		}

		//将一个长期参考图像标记为非参考图像，也 即将一个长期参考图像移出参考帧队列
		if (sHeader.dec_ref_pic_markings[i].memory_management_control_operation == 2)
		{
			//如果field_pic_flag为0，则LongTermPicNum等于long_term_pic_num的长期参考帧被标记为“不用于参考”
			for (size_t j = 0; j < dpb.length; j++)
			{
				if (dpb[j]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE && dpb[j]->LongTermPicNum == sHeader.dec_ref_pic_markings[i].long_term_pic_num)
				{
					dpb[j]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
				}
			}
		}

		//将一个短期参考图像转为长期参考图像
		if (sHeader.dec_ref_pic_markings[i].memory_management_control_operation == 3)
		{
			int picNumX = sHeader.CurrPicNum - (sHeader.dec_ref_pic_markings[i].difference_of_pic_nums_minus1 + 1);
			//当LongTermFrameIdx等于long_term_frame_idx已经被分配给一个长期参考帧,该帧被标记为“未使用的参考”
			for (size_t j = 0; j < dpb.length; j++)
			{
				if (dpb[j]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE && dpb[j]->LongTermFrameIdx == sHeader.dec_ref_pic_markings[i].long_term_frame_idx)
				{
					dpb[j]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
				}
			}

			//如果field_pic_flag等于0，则PicNumX所确定的短期参考帧将从“用于短期参考”转化为“用于长期参考”
			for (size_t j = 0; j < dpb.length; j++)
			{
				if (dpb[j]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE && dpb[j]->PicNum == picNumX)
				{
					dpb[j]->reference_marked_type = PICTURE_MARKING::LONG_TERM_REFERENCE;
				}
			}

		}

		//指明长期参考帧的最大数目。
		if (sHeader.dec_ref_pic_markings[i].memory_management_control_operation == 4)
		{
			for (size_t j = 0; j < dpb.length; j++)
			{
				if ((dpb[j]->LongTermFrameIdx > sHeader.dec_ref_pic_markings[i].max_long_term_frame_idx_plus1 - 1) && dpb[j]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE)
				{
					dpb[j]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
				}
			}
			if (sHeader.dec_ref_pic_markings[i].max_long_term_frame_idx_plus1 == 0)
			{
				pic->MaxLongTermFrameIdx = NA;//非长期帧索引
			}
			else
			{
				pic->MaxLongTermFrameIdx = sHeader.dec_ref_pic_markings[i].max_long_term_frame_idx_plus1 - 1;
			}
		}

		//清空参考帧队列，将所有参考图像移出参考帧队列，并禁用长期参考机制
		if (sHeader.dec_ref_pic_markings[i].memory_management_control_operation == 5)
		{
			for (size_t j = 0; j < dpb.length; j++)
			{
				dpb[j]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
			}
			pic->MaxLongTermFrameIdx = NA;
			pic->memory_management_control_operation_5_flag = true;
		}

		//将当前图像存为一个长期参考帧
		if (sHeader.dec_ref_pic_markings[i].memory_management_control_operation == 6)
		{
			for (size_t j = 0; j < dpb.length; j++)
			{
				if (dpb[j]->LongTermFrameIdx == sHeader.dec_ref_pic_markings[i].long_term_frame_idx && dpb[j]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE)
				{
					dpb[j]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
				}
			}

			pic->reference_marked_type = PICTURE_MARKING::LONG_TERM_REFERENCE;
			pic->LongTermFrameIdx = sHeader.dec_ref_pic_markings[i].long_term_frame_idx;
		}
	}
}
