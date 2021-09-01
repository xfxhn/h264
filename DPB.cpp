#include "DPB.h"
#include "ParseSlice.h"


//���ڲο��ͳ��ڲο�����ΪDPB�������������޴洢��ȥ������Ҫ���ĳЩ֡Ϊ���ڻ��Ƕ��ڣ����ڳ��������ض�ָ��Ż��Ƴ������ڵ���������ʱ���Ƴ��������ض�ָ��
DPB::DPB() :dpb(16), RefPicList0(16), RefPicList1(16)
{
	previous = nullptr;
}

void DPB::Decoding_process_for_picture_numbers(const SliceHeader& sHeader)
{

	for (size_t i = 0; i < dpb.length; i++)
	{
		//��ÿһ�����ڲο�ͼ��
		if (dpb[i]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE)
		{
			//FrameNum ���ó���Ӧ���ڲο�ͼ���Ƭͷ�н������õ��﷨Ԫ��frame_num  
			if (dpb[i]->FrameNum > sHeader.frame_num)
			{
				dpb[i]->FrameNumWrap = dpb[i]->FrameNum - dpb[i]->MaxFrameNum;
			}
			else
			{
				dpb[i]->FrameNumWrap = dpb[i]->FrameNum;
			}
		}

		//LongTermFrameIdx  ���ڲο�ͼ���Ѿ���ͼ�������ɵ�ʱ��ָ��
	}


	//field_pic_flag==0 �ο�֡�򻥲��ο�����
	for (size_t i = 0; i < dpb.length; i++)
	{
		if (dpb[i]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE)
		{
			//������ʹ��ͼ����� PicNum ���һ�����ڲο�ͼ��
			dpb[i]->PicNum = dpb[i]->FrameNumWrap;
		}

		if (dpb[i]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE)
		{
			//ʹ�ó���ͼ����� LongTermPicNum ���һ�����ڲο�ͼ��
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


	//num_ref_idx_l0_active_minus1��ǰ��ο������ο�����
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

	//�����еĶ��ڲο�֡��ǰ�棬���ڲο��ź���
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
		else //�������ο���
		{

		}
	}


	if (shortTermlength > 0)
	{
		for (size_t i = 0; i < shortTermlength - 1; i++)
		{
			for (size_t j = 0; j < shortTermlength - i - 1; j++)
			{
				if (shortTerm[j]->PicNum < shortTerm[j + 1]->PicNum)//��������
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
				if (longTerm[j]->LongTermPicNum > longTerm[j + 1]->LongTermPicNum)//��������
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

	//�����еĶ��ڲο�֡��ǰ�棬���ڲο��ź���
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
			//�������ο�
		}
	}

	if (shortTermLeftLength > 0)
	{
		for (size_t i = 0; i < shortTermLeftLength - 1; i++)
		{
			for (size_t j = 0; j < shortTermLeftLength - i - 1; j++)
			{
				if (shortTermLeft[j]->PicOrderCnt < shortTermLeft[j + 1]->PicOrderCnt)//��������
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
				if (shortTermRight[j]->PicOrderCnt > shortTermRight[j + 1]->PicOrderCnt)//��������
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
				if (longTerm[j]->LongTermPicNum > longTerm[j + 1]->LongTermPicNum)//��������
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
			//�������ο�
		}
	}


	if (shortTermLeftLength > 0)
	{
		for (size_t i = 0; i < shortTermLeftLength - 1; i++)
		{
			for (size_t j = 0; j < shortTermLeftLength - i - 1; j++)
			{
				if (shortTermLeft[j]->PicOrderCnt > shortTermLeft[j + 1]->PicOrderCnt)//��������
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
				if (shortTermRight[j]->PicOrderCnt < shortTermRight[j + 1]->PicOrderCnt)//��������
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
				if (longTerm[j]->LongTermPicNum > longTerm[j + 1]->LongTermPicNum)//��������
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
	//ͼ����ŵļ���
	Decoding_process_for_picture_numbers(sHeader);

	//�ο�ͼ����ͨ�� 8.4.2.1 ���ж���Ĳο���������λ��
	//�ο�ͼ���б�ĳ�ʼ������
	Initialisation_process_for_reference_picture_lists(slice);
}

void DPB::Decoded_reference_picture_marking_process(Picture* pic, const SliceHeader& sHeader)
{

	if (sHeader.nalu.IdrPicFlag)
	{
		//���вο�ͼ����Ҫ�����Ϊ"�����ڲο�" 
		for (size_t i = 0; i < dpb.length; i++)
		{
			dpb[i]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
		}

		if (sHeader.long_term_reference_flag)
		{
			//��IDRͼ����Ҫ �����Ϊ"���ڳ��ڲο�"
			pic->reference_marked_type = PICTURE_MARKING::LONG_TERM_REFERENCE;
			pic->LongTermFrameIdx = 0;
			pic->MaxLongTermFrameIdx = 0;

		}
		else//sHeader.long_term_reference_flag=0
		{
			//��IDRͼ�񽫱����Ϊ"���ڶ��ڲο�"
			pic->reference_marked_type = PICTURE_MARKING::SHORT_TERM_REFERENCE;
			pic->MaxLongTermFrameIdx = NA; //����Ϊû�г�������
		}
	}
	else
	{
		// =0 �����ȳ���FIFO����ʹ�û������Ļ��ƣ������ȳ���������ģʽ��û�а취�Գ��ڲο�֡���в�����
		// =1 ����Ӧ��ǣ�marking�������������л���һϵ�о䷨Ԫ����ʽָ�������Ĳ��衣����Ӧ��ָ�������ɸ��������������������ߡ�
		if (sHeader.adaptive_ref_pic_marking_mode_flag)
		{
			//����Ӧ��ǹ���
			Adaptive_memory_control_decoded_reference_picture_marking_process(sHeader, pic);
		}
		else
		{
			//�������ڽ���ο�ͼ��ı�ʶ����
			Sliding_window_decoded_reference_picture_marking_process(sHeader.sps.max_num_ref_frames);
		}
	}

}


void DPB::Decoding_to_complete(ParseSlice* slice, const  SliceHeader& sHeader)
{
	Picture* pic = new Picture(slice, sHeader);

	if (sHeader.nalu.nal_ref_idc != 0)
	{
		//��ǵ�ǰ������ɵ�֡��ʲô���ͣ�Ȼ��ŵ�DBP��ȥ����
		Decoded_reference_picture_marking_process(pic, sHeader);
	}
	previous = pic;

	dpb.push(pic);
}



void DPB::Sliding_window_decoded_reference_picture_marking_process(uint8_t max_num_ref_frames)
{
	//�����ǰ���볡��һ�������ο����Եĵڶ����������ҵ�һ�����Ѿ������Ϊ�����ڶ��ڲο���ʱ����ǰͼ��ҲӦ�ñ����Ϊ�����ڶ��ڲο���

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
		//������С FrameNumWrap ֵ�Ķ��ڲο�֡������Ϊ�������ڲο���
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
		//������һ��֡�򳡶ԣ���ô������������������Ϊ�������ڲο�����
	}
}

void DPB::Adaptive_memory_control_decoded_reference_picture_marking_process(const SliceHeader& sHeader, Picture* pic)
{

	for (size_t i = 0; i < sHeader.dec_ref_pic_markings_size; i++)
	{
		//��һ�����ڲο�ͼ����Ϊ�ǲο�ͼ��Ҳ ����һ�����ڲο�ͼ���Ƴ��ο�֡����
		if (sHeader.dec_ref_pic_markings[i].memory_management_control_operation == 1)
		{
			int picNumX = sHeader.CurrPicNum - (sHeader.dec_ref_pic_markings[i].difference_of_pic_nums_minus1 + 1);
			//���field_pic_flag����0, ��picNumXָ���Ķ��ڲο�֡�����Ϊ��unused for reference����

			for (size_t j = 0; j < dpb.length; j++)
			{
				if (dpb[j]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE && dpb[j]->PicNum == picNumX)
				{
					dpb[j]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
				}
			}
		}

		//��һ�����ڲο�ͼ����Ϊ�ǲο�ͼ��Ҳ ����һ�����ڲο�ͼ���Ƴ��ο�֡����
		if (sHeader.dec_ref_pic_markings[i].memory_management_control_operation == 2)
		{
			//���field_pic_flagΪ0����LongTermPicNum����long_term_pic_num�ĳ��ڲο�֡�����Ϊ�������ڲο���
			for (size_t j = 0; j < dpb.length; j++)
			{
				if (dpb[j]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE && dpb[j]->LongTermPicNum == sHeader.dec_ref_pic_markings[i].long_term_pic_num)
				{
					dpb[j]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
				}
			}
		}

		//��һ�����ڲο�ͼ��תΪ���ڲο�ͼ��
		if (sHeader.dec_ref_pic_markings[i].memory_management_control_operation == 3)
		{
			int picNumX = sHeader.CurrPicNum - (sHeader.dec_ref_pic_markings[i].difference_of_pic_nums_minus1 + 1);
			//��LongTermFrameIdx����long_term_frame_idx�Ѿ��������һ�����ڲο�֡,��֡�����Ϊ��δʹ�õĲο���
			for (size_t j = 0; j < dpb.length; j++)
			{
				if (dpb[j]->reference_marked_type == PICTURE_MARKING::LONG_TERM_REFERENCE && dpb[j]->LongTermFrameIdx == sHeader.dec_ref_pic_markings[i].long_term_frame_idx)
				{
					dpb[j]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
				}
			}

			//���field_pic_flag����0����PicNumX��ȷ���Ķ��ڲο�֡���ӡ����ڶ��ڲο���ת��Ϊ�����ڳ��ڲο���
			for (size_t j = 0; j < dpb.length; j++)
			{
				if (dpb[j]->reference_marked_type == PICTURE_MARKING::SHORT_TERM_REFERENCE && dpb[j]->PicNum == picNumX)
				{
					dpb[j]->reference_marked_type = PICTURE_MARKING::LONG_TERM_REFERENCE;
				}
			}

		}

		//ָ�����ڲο�֡�������Ŀ��
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
				pic->MaxLongTermFrameIdx = NA;//�ǳ���֡����
			}
			else
			{
				pic->MaxLongTermFrameIdx = sHeader.dec_ref_pic_markings[i].max_long_term_frame_idx_plus1 - 1;
			}
		}

		//��ղο�֡���У������вο�ͼ���Ƴ��ο�֡���У������ó��ڲο�����
		if (sHeader.dec_ref_pic_markings[i].memory_management_control_operation == 5)
		{
			for (size_t j = 0; j < dpb.length; j++)
			{
				dpb[j]->reference_marked_type = PICTURE_MARKING::UNUSED_FOR_REFERENCE;
			}
			pic->MaxLongTermFrameIdx = NA;
			pic->memory_management_control_operation_5_flag = true;
		}

		//����ǰͼ���Ϊһ�����ڲο�֡
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
