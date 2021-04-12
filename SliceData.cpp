#include "SliceData.h"

SliceData::SliceData()
{
}

int SliceData::NextMbAddress(const SliceHeader& sHeader, uint32_t n)
{
	int32_t i = n + 1;

	/*if (sHeader.MbToSliceGroupMap == NULL)
	{
		LOG_ERROR("slice_header.MbToSliceGroupMap == NULL\n");
		return -1;
	}

	if (i >= slice_header.PicSizeInMbs)
	{
		LOG_ERROR("i(%d) >= slice_header.PicSizeInMbs(%d);\n", i, slice_header.PicSizeInMbs);
		return -2;
	}

	while (i < slice_header.PicSizeInMbs && slice_header.MbToSliceGroupMap[i] != slice_header.MbToSliceGroupMap[n])
	{
		i++;
		if (i >= slice_header.PicSizeInMbs)
		{
			LOG_ERROR("i(%d) >= slice_header.PicSizeInMbs(%d);\n", i, slice_header.PicSizeInMbs);
			return -3;
		}
	}*/

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

				bool prevMbSkipped = (mb_skip_run > 0);

				for (size_t i = 0; i < mb_skip_run; i++)
				{
					//CurrMbAddr = NextMbAddress(CurrMbAddr)
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
			/*if (sHeader.MbaffFrameFlag && (CurrMbAddr % 2 == 0 || (CurrMbAddr % 2 == 1 && prevMbSkipped))) {

			}*/
		}



	} while (false);
	return false;
}

SliceData::~SliceData()
{
}
