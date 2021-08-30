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
