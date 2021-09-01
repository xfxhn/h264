#include "DPB.h"
#include "ParseSlice.h"


//短期参考和长期参考，因为DPB容量不可能无限存储下去，所以要标记某些帧为长期还是短期，长期除非遇到特定指令才会移除，短期当容量满的时候移除，或者特定指令
DPB::DPB() :dpb(16), RefPicList0(16), RefPicList1(16)
{
	previous = nullptr;
}

void DPB::Decoding_process_for_picture_numbers(const SliceHeader& sHeader)
{

	for (size_t i = 0; i < dpb.length; i++)
	{
		//对每一个短期参考图像
		if (dpb[i]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE)
		{
			//FrameNum 设置成相应短期参考图像的片头中解码所得得语法元素frame_num  
			if (dpb[i]->FrameNum > sHeader.frame_num)
			{
				dpb[i]->FrameNumWrap = dpb[i]->FrameNum - dpb[i]->MaxFrameNum;
			}
			else
			{
				dpb[i]->FrameNumWrap = dpb[i]->FrameNum;
			}
		}

		//LongTermFrameIdx  长期参考图像已经在图像解码完成的时候指定
	}


	//field_pic_flag==0 参考帧或互补参考场对
	for (size_t i = 0; i < dpb.length; i++)
	{
		if (dpb[i]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE)
		{
			//解码器使用图像序号 PicNum 标记一个短期参考图像
			dpb[i]->PicNum = dpb[i]->FrameNumWrap;
		}

		if (dpb[i]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE)
		{
			//使用长期图像序号 LongTermPicNum 标记一个长期参考图像
			dpb[i]->LongTermPicNum = dpb[i]->LongTermFrameIdx;
		}
	}

}

void DPB::Initialisation_process_for_reference_picture_lists(const ParseSlice* slice)
{
	SliceHeader& sHeader = slice->sHeader;
	SLIECETYPE slice_type = (SLIECETYPE)sHeader.slice_type;

	if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_P || slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP)
	{
		Initialisation_process_for_the_reference_picture_list_for_P_and_SP_slices_in_frames();
	}
	else if (slice_type == SLIECETYPE::H264_SLIECE_TYPE_B)
	{
		Initialisation_process_for_reference_picture_lists_for_B_slices_in_frames(slice->PicOrderCnt);
	}


	//num_ref_idx_l0_active_minus1是前向参考的最大参考索引
	if (RefPicList0.length > sHeader.num_ref_idx_l0_active_minus1 + 1)
	{
		RefPicList0.splice(sHeader.num_ref_idx_l0_active_minus1 + 1, RefPicList0.length);
	}

	if (RefPicList1.length > sHeader.num_ref_idx_l1_active_minus1 + 1)
	{
		RefPicList1.splice(sHeader.num_ref_idx_l1_active_minus1 + 1, RefPicList1.length);
	}
}

void DPB::Initialisation_process_for_the_reference_picture_list_for_P_and_SP_slices_in_frames()
{
	size_t shortTermlength = 0;
	Picture* shortTerm[16] = { 0 };

	size_t longTermLength = 0;
	Picture* longTerm[16] = { 0 };

	//把所有的短期参考帧放前面，长期参考放后面
	for (size_t i = 0; i < dpb.length; i++)
	{
		if (dpb[i]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE)
		{
			shortTerm[shortTermlength] = dpb[i];
			shortTermlength++;
		}
		else if (dpb[i]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE)
		{
			longTerm[longTermLength] = dpb[i];
			longTermLength++;
		}
		else //不用做参考的
		{

		}
	}


	if (shortTermlength > 0)
	{
		for (size_t i = 0; i < shortTermlength - 1; i++)
		{
			for (size_t j = 0; j < shortTermlength - i - 1; j++)
			{
				if (shortTerm[j]->PicNum < shortTerm[j + 1]->PicNum)//降序排列
				{
					Picture* temp = shortTerm[j];
					shortTerm[j] = shortTerm[j + 1];
					shortTerm[j + 1] = temp;
				}
			}
		}
	}

	if (longTermLength > 0)
	{
		for (size_t i = 0; i < longTermLength - 1; i++)
		{
			for (size_t j = 0; j < longTermLength - i - 1; j++)
			{
				if (longTerm[j]->LongTermPicNum > longTerm[j + 1]->LongTermPicNum)//升序排列
				{
					Picture* temp = longTerm[j];
					longTerm[j] = longTerm[j + 1];
					longTerm[j + 1] = temp;
				}
			}
		}
	}

	for (size_t i = 0; i < shortTermlength; i++)
	{
		RefPicList0.push(shortTerm[i]);
	}

	for (size_t i = 0; i < longTermLength; i++)
	{
		RefPicList0.push(longTerm[i]);
	}

}

void DPB::Initialisation_process_for_reference_picture_lists_for_B_slices_in_frames(int POC)
{
	size_t shortTermLeftLength = 0;
	Picture* shortTermLeft[16] = { 0 };
	size_t shortTermRightLength = 0;
	Picture* shortTermRight[16] = { 0 };


	size_t longTermLength = 0;
	Picture* longTerm[16] = { 0 };

	//把所有的短期参考帧放前面，长期参考放后面
	for (size_t i = 0; i < dpb.length; i++)
	{
		if (dpb[i]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE)
		{
			if (dpb[i]->PicOrderCnt < POC)
			{
				shortTermLeft[shortTermLeftLength] = dpb[i];
				shortTermLeftLength++;
			}
			else //if (dpb[i]->PicOrderCnt >= POC)
			{
				shortTermRight[shortTermRightLength] = dpb[i];
				shortTermRightLength++;
			}
		}
		else if (dpb[i]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE)
		{
			longTerm[longTermLength] = dpb[i];
			longTermLength++;
		}
		else
		{
			//不用做参考
		}
	}

	if (shortTermLeftLength > 0)
	{
		for (size_t i = 0; i < shortTermLeftLength - 1; i++)
		{
			for (size_t j = 0; j < shortTermLeftLength - i - 1; j++)
			{
				if (shortTermLeft[j]->PicOrderCnt < shortTermLeft[j + 1]->PicOrderCnt)//降序排列
				{
					Picture* temp = shortTermLeft[j];
					shortTermLeft[j] = shortTermLeft[j + 1];
					shortTermLeft[j + 1] = temp;
				}
			}
		}
	}

	if (shortTermRightLength > 0)
	{
		for (size_t i = 0; i < shortTermRightLength - 1; i++)
		{
			for (size_t j = 0; j < shortTermRightLength - i - 1; j++)
			{
				if (shortTermRight[j]->PicOrderCnt > shortTermRight[j + 1]->PicOrderCnt)//升序排列
				{
					Picture* temp = shortTermRight[j];
					shortTermRight[j] = shortTermRight[j + 1];
					shortTermRight[j + 1] = temp;
				}
			}
		}
	}

	if (longTermLength > 0)
	{
		for (size_t i = 0; i < longTermLength - 1; i++)
		{
			for (size_t j = 0; j < longTermLength - i - 1; j++)
			{
				if (longTerm[j]->LongTermPicNum > longTerm[j + 1]->LongTermPicNum)//升序排列
				{
					Picture* temp = longTerm[j];
					longTerm[j] = longTerm[j + 1];
					longTerm[j + 1] = temp;
				}
			}
		}
	}


	for (size_t i = 0; i < shortTermLeftLength; i++)
	{
		RefPicList0.push(shortTermLeft[i]);
	}

	for (size_t i = 0; i < shortTermRightLength; i++)
	{
		RefPicList0.push(shortTermRight[i]);
	}

	for (size_t i = 0; i < longTermLength; i++)
	{
		RefPicList0.push(longTerm[i]);
	}


	shortTermLeftLength = 0;
	memset(shortTermLeft, NULL, sizeof(Picture*) * 16);
	shortTermRightLength = 0;
	memset(shortTermRight, NULL, sizeof(Picture*) * 16);

	longTermLength = 0;
	memset(longTerm, NULL, sizeof(Picture*) * 16);


	for (size_t i = 0; i < dpb.length; i++)
	{
		if (dpb[i]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE)
		{
			if (dpb[i]->PicOrderCnt > POC)
			{
				shortTermLeft[shortTermLeftLength] = dpb[i];
				shortTermLeftLength++;
			}
			else //if (dpb[i]->PicOrderCnt <= POC)
			{
				shortTermRight[shortTermRightLength] = dpb[i];
				shortTermRightLength++;
			}
		}
		else if (dpb[i]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE)
		{
			longTerm[longTermLength] = dpb[i];
			longTermLength++;
		}
		else
		{
			//不用做参考
		}
	}


	if (shortTermLeftLength > 0)
	{
		for (size_t i = 0; i < shortTermLeftLength - 1; i++)
		{
			for (size_t j = 0; j < shortTermLeftLength - i - 1; j++)
			{
				if (shortTermLeft[j]->PicOrderCnt > shortTermLeft[j + 1]->PicOrderCnt)//升序排列
				{
					Picture* temp = shortTermLeft[j];
					shortTermLeft[j] = shortTermLeft[j + 1];
					shortTermLeft[j + 1] = temp;
				}
			}
		}
	}


	if (shortTermRightLength > 0)
	{
		for (size_t i = 0; i < shortTermRightLength - 1; i++)
		{
			for (size_t j = 0; j < shortTermRightLength - i - 1; j++)
			{
				if (shortTermRight[j]->PicOrderCnt < shortTermRight[j + 1]->PicOrderCnt)//降序排列
				{
					Picture* temp = shortTermRight[j];
					shortTermRight[j] = shortTermRight[j + 1];
					shortTermRight[j + 1] = temp;
				}
			}
		}
	}


	if (longTermLength > 0)
	{
		for (size_t i = 0; i < longTermLength - 1; i++)
		{
			for (size_t j = 0; j < longTermLength - i - 1; j++)
			{
				if (longTerm[j]->LongTermPicNum > longTerm[j + 1]->LongTermPicNum)//升序排列
				{
					Picture* temp = longTerm[j];
					longTerm[j] = longTerm[j + 1];
					longTerm[j + 1] = temp;
				}
			}
		}
	}


	for (size_t i = 0; i < shortTermLeftLength; i++)
	{
		RefPicList1.push(shortTermLeft[i]);
	}

	for (size_t i = 0; i < shortTermRightLength; i++)
	{
		RefPicList1.push(shortTermRight[i]);
	}

	for (size_t i = 0; i < longTermLength; i++)
	{
		RefPicList1.push(longTerm[i]);
	}


	bool flag = false;

	if (RefPicList1.length > 1 && RefPicList1.length == RefPicList0.length)
	{
		const size_t length = RefPicList1.length;

		for (size_t i = 0; i < length; i++)
		{
			if (RefPicList1[i] == RefPicList0[i])
			{
				flag = true;
			}
			else
			{
				flag = false;
				break;
			}
		}
	}


	if (flag)
	{
		Picture* tmp = RefPicList1[0];
		RefPicList1[0] = RefPicList1[1];
		RefPicList1[1] = tmp;
	}
}


void DPB::Decoding_process_for_reference_picture_lists_construction(ParseSlice* slice)
{
	const SliceHeader& sHeader = slice->sHeader;
	//图像序号的计算
	Decoding_process_for_picture_numbers(sHeader);

	//参考图像是通过 8.4.2.1 节中定义的参考索引来定位的
	//参考图像列表的初始化过程
	Initialisation_process_for_reference_picture_lists(slice);
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
