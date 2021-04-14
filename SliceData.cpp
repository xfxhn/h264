#include "SliceData.h"

SliceData::SliceData()
{
	mb_field_decoding_flag = 0;
	mb_skip_flag = 0;
}

//�������
int SliceData::NextMbAddress(const SliceHeader& sHeader, uint32_t n)
{
	int32_t i = n + 1;
	if (sHeader.MbToSliceGroupMap == NULL)
	{
		printError("sHeader.MbToSliceGroupMap=null");
		return -1;
	}


	if (i >= sHeader.PicSizeInMbs)
	{
		printError("i >= slice_header.PicSizeInMbs(%d);");
		return -1;
	}

	while (i < sHeader.PicSizeInMbs && sHeader.MbToSliceGroupMap[i] != sHeader.MbToSliceGroupMap[n])
	{
		i++;
		if (i >= sHeader.PicSizeInMbs)
		{
			printError("i(%d) >= slice_header.PicSizeInMbs(%d);\n");
			return -3;
		}
	}

	return i;
}

bool SliceData::slice_data(BitStream& bs, SliceHeader& sHeader)
{


	bool isAe = sHeader.pps.entropy_coding_mode_flag;  //ae(v)��ʾCABAC����

	if (isAe)
	{
		while (!byte_aligned(bs))
		{
			/*���ر���ģʽ��CABACʱ,
			  ��ʱҪ�������ֽڶ���, �����ݴ���һ���ֽڵĵ�һ�����ؿ�ʼ,
			  �����û���ֽڶ��뽫�������ɸ� cabac_alignment_one_bit ��Ϊ��䡣*/
			cabac_alignment_one_bit = bs.readBit(); //2 f(1)
		}
	}


	//sliceֻ��3�ֱ��뷽ʽ��֡���롢�������MBAFF
	//MbaffFrameFlag��ʾ֡������Ӧ

	//��ǰ����ĺ����ͼƬ�е�����λ��
	CurrMbAddr = sHeader.first_mb_in_slice * (1 + sHeader.MbaffFrameFlag);

	bool moreDataFlag = true;
	bool prevMbSkipped = false;
	do
	{
		if ((SLIECETYPE)sHeader.slice_type != SLIECETYPE::H264_SLIECE_TYPE_I && (SLIECETYPE)sHeader.slice_type != SLIECETYPE::H264_SLIECE_TYPE_SI)
		{

			//����0����ô�����﷨������ߵ���������ָ���ķ���
			//����1���Ͳ����﷨�����ұߵ���������ָ���ķ���
			/*��entropy_coding_mode_flagΪ1�����ر���ΪCABACʱ ��ÿ���� �� Ծ ���鶼���о䷨Ԫ��mb_skip_flagָ����
			��entropy_coding_mode_flag ���� 0�����ر���ΪCAVLCʱ��
			  ��һ���г̵ķ������������ŵġ���Ծ�������Ŀ�����䷨Ԫ�� mb_skip_run��
			  mb_skip_run ֵ�ķ�Χ 0 to PicSizeInMbs �C CurrMbAddr  ��*/
			if (!sHeader.pps.entropy_coding_mode_flag)
			{
				/*��ͼ�����֡��Ԥ�����ʱ��H.264 ������ͼ��ƽ̹������ʹ�á���Ծ���飬����Ծ���鱾��Я���κ����ݣ�
				  ������ͨ����Χ���ؽ��ĺ����������ָ�����Ծ���顣���ر���Ϊ CAVLC �� CABAC ʱ������Ծ����ı�ʾ������ͬ��*/
				mb_skip_run = bs.readUE();
				prevMbSkipped = (mb_skip_run > 0);


				for (size_t i = 0; i < mb_skip_run; i++)
				{
					CurrMbAddr = NextMbAddress(sHeader, CurrMbAddr);
				}

				if (mb_skip_run > 0)
				{
					//�����ѭ����������ݣ���������Ǹ�����ֽ�֮ǰû�������˾���ֹ���ѭ������
					//�����rbsp_trailing_bits( )֮ǰ��RBSP���и������ݣ�more_rbsp_data( ) �ķ���ֵΪTRUE,���򷵻�false
					//moreDataFlag = more_rbsp_data(bs);
					moreDataFlag = bs.isEmpty();
				}
			}
			else  //ae(v)��ʾCABAC����
			{
				//cabac����  ��ʱ����
				printError("cabac����");
				return -1;
			}
		}

		if (moreDataFlag)
		{
			if (sHeader.MbaffFrameFlag && (CurrMbAddr % 2 == 0 || (CurrMbAddr % 2 == 1 && prevMbSkipped))) {
				return -1;
			}
			//macroblock_layer

			Macroblock mb;
			mb.macroblock_layer(bs, sHeader);


		}
		if (!sHeader.pps.entropy_coding_mode_flag)
		{
			moreDataFlag = bs.isEmpty();
		}
		else
		{
			if ((SLIECETYPE)sHeader.slice_type != SLIECETYPE::H264_SLIECE_TYPE_I && (SLIECETYPE)sHeader.slice_type != SLIECETYPE::H264_SLIECE_TYPE_SI)
			{
				prevMbSkipped = mb_skip_flag;
			}



			if (sHeader.MbaffFrameFlag && CurrMbAddr % 2 == 0)
			{
				moreDataFlag = true;
			}
			else
			{
				//ret = cabac.CABAC_decode_end_of_slice_flag(picture, bs, end_of_slice_flag); //2 ae(v)
				//RETURN_IF_FAILED(ret != 0, ret);

				//moreDataFlag = !end_of_slice_flag;


			}
		}


	} while (moreDataFlag);
	return false;
}

SliceData::~SliceData()
{
}
