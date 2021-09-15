#include "SliceData.h"
#include "ParseSlice.h"
SliceData::SliceData()
{
	mb_field_decoding_flag = 0;
	mb_skip_flag = false;
	CurrMbAddr = 0;
	cabac_alignment_one_bit = 0;
	mb_skip_run = 0;
	end_of_slice_flag = false;
}

//�������
int SliceData::NextMbAddress(const SliceHeader& sHeader, uint32_t n)
{
	int i = n + 1;


	if (i >= sHeader.PicSizeInMbs)
	{
		printError("i >= slice_header.PicSizeInMbs(%d);");
		return -1;
	}

	//i��Ч��������Ƭ����n��ͬ i=i+1,��Ϊ����ж������飬����˳������ҵ�
	while (i < sHeader.PicSizeInMbs && sHeader.MbToSliceGroupMap[i] != sHeader.MbToSliceGroupMap[n])
	{
		i++;
	}

	return i;
}

bool SliceData::slice_data(BitStream& bs, ParseSlice* Slice, DPB& dpb)
{
	Cabac cabac;

	const SliceHeader& sHeader = Slice->sHeader;

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

		//��ʼ��cabac
		cabac.Initialisation_process_for_context_variables((SLIECETYPE)sHeader.slice_type, sHeader.SliceQPY, sHeader.cabac_init_idc);

		cabac.Initialisation_process_for_the_arithmetic_decoding_engine(bs);
	}


	//sliceֻ��3�ֱ��뷽ʽ��֡���롢�������MBAFF
	//MbaffFrameFlag��ʾ֡������Ӧ

	//��ǰ����ĺ����ͼƬ�е�����λ��
	CurrMbAddr = sHeader.first_mb_in_slice;
	Slice->CurrMbAddr = CurrMbAddr;

	if (!sHeader.MbaffFrameFlag)
	{
		mb_field_decoding_flag = sHeader.field_pic_flag;
	}

	if (Slice->sliceCount == 0)
	{
		Slice->Decoding_process_for_picture_order_count(dpb);

		//�ο�ͼ���б�  �ڽ���ÿһ��P,SP��B������ʼʱ
		if ((SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_P
			|| (SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_SP
			|| (SLIECETYPE)sHeader.slice_type == SLIECETYPE::H264_SLIECE_TYPE_B
			)
		{
			dpb.Decoding_process_for_reference_picture_lists_construction(Slice);
		}
	}

	Slice->sliceCount++;
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

					Slice->mbX = (CurrMbAddr % Slice->sHeader.sps.PicWidthInMbs);
					Slice->mbY = (CurrMbAddr / Slice->sHeader.sps.PicWidthInMbs);


					Slice->CurrMbAddr = CurrMbAddr;

					Slice->mbCount++;

					Slice->macroblock[Slice->CurrMbAddr]->macroblock_layer_skip(Slice, this);

					uint8_t predPartL[16][16] = { 0 };
					uint8_t predPartCb[16][16] = { 0 };
					uint8_t predPartCr[16][16] = { 0 };
					Slice->Inter_prediction_process(dpb, predPartL, predPartCb, predPartCr, true);

					CurrMbAddr = NextMbAddress(sHeader, CurrMbAddr);

				}

				if (mb_skip_run > 0)
				{
					//�����ѭ����������ݣ���������Ǹ�����ֽ�֮ǰû�������˾���ֹ���ѭ������
					//�����rbsp_trailing_bits( )֮ǰ��RBSP���и������ݣ�more_rbsp_data( ) �ķ���ֵΪTRUE,���򷵻�false
					//moreDataFlag = more_rbsp_data(bs);
					moreDataFlag = bs.more_rbsp_data();
				}
			}
			else  //ae(v)��ʾCABAC����
			{
				Slice->mbX = (CurrMbAddr % Slice->sHeader.sps.PicWidthInMbs);
				Slice->mbY = (CurrMbAddr / Slice->sHeader.sps.PicWidthInMbs);

				Slice->CurrMbAddr = CurrMbAddr;


				mb_skip_flag = cabac.decode_mb_skip_flag(bs, Slice);


				if (mb_skip_flag)
				{
					Slice->mbCount++;
					//��ʾ�����û�вв����ݣ���Ӧ������ֵֻ��Ҫ����֮ǰ�Ѿ������I/P֡��Ԥ����

					Slice->macroblock[Slice->CurrMbAddr]->macroblock_layer_skip(Slice, this);


					uint8_t predPartL[16][16] = { 0 };
					uint8_t predPartCb[16][16] = { 0 };
					uint8_t predPartCr[16][16] = { 0 };
					Slice->Inter_prediction_process(dpb, predPartL, predPartCb, predPartCr, true);


				}

				moreDataFlag = !mb_skip_flag;
			}
		}

		if (moreDataFlag)
		{
			if (sHeader.MbaffFrameFlag && (CurrMbAddr % 2 == 0 || (CurrMbAddr % 2 == 1 && prevMbSkipped))) {
				printError("֡������Ӧ sHeader.MbaffFrameFlag && (CurrMbAddr % 2 == 0 || (CurrMbAddr % 2 == 1 && prevMbSkipped))");
				exit(1);
			}


			Slice->mbX = (CurrMbAddr % Slice->sHeader.sps.PicWidthInMbs);
			Slice->mbY = (CurrMbAddr / Slice->sHeader.sps.PicWidthInMbs);

			Slice->CurrMbAddr = CurrMbAddr;
			Slice->mbCount++;

			Slice->macroblock[Slice->CurrMbAddr]->macroblock_layer(bs, Slice, this, cabac);

			bool isChromaCb = true;
			if (Slice->macroblock[Slice->CurrMbAddr]->mode == H264_MB_PART_PRED_MODE::Intra_4x4)
			{
				Slice->transformDecode4x4LuamResidualProcess();
				isChromaCb = true;
				Slice->transformDecodeChromaResidualProcess(isChromaCb);
				isChromaCb = false;
				Slice->transformDecodeChromaResidualProcess(isChromaCb);
			}
			else if (Slice->macroblock[Slice->CurrMbAddr]->mode == H264_MB_PART_PRED_MODE::Intra_8x8)
			{
				Slice->transformDecode8x8LuamResidualProcess();
				isChromaCb = true;
				Slice->transformDecodeChromaResidualProcess(isChromaCb);
				isChromaCb = false;
				Slice->transformDecodeChromaResidualProcess(isChromaCb);
			}
			else if (Slice->macroblock[Slice->CurrMbAddr]->mode == H264_MB_PART_PRED_MODE::Intra_16x16)
			{
				Slice->transformDecode16x16LuamResidualProcess(Slice->macroblock[Slice->CurrMbAddr]->i16x16DClevel, Slice->macroblock[Slice->CurrMbAddr]->i16x16AClevel, true, false);
				isChromaCb = true;
				Slice->transformDecodeChromaResidualProcess(isChromaCb);
				isChromaCb = false;
				Slice->transformDecodeChromaResidualProcess(isChromaCb);
			}
			else if (Slice->macroblock[Slice->CurrMbAddr]->mbType == H264_MB_TYPE::I_PCM)
			{

			}
			else  //֡��Ԥ��
			{

				uint8_t predPartL[16][16] = { 0 };
				uint8_t predPartCb[16][16] = { 0 };
				uint8_t predPartCr[16][16] = { 0 };
				Slice->Inter_prediction_process(dpb, predPartL, predPartCb, predPartCr);
				if (Slice->macroblock[Slice->CurrMbAddr]->transform_size_8x8_flag)
				{
					Slice->transformDecode8x8LuamResidualProcessInter(predPartL);
				}
				else
				{
					Slice->transformDecode4x4LuamResidualProcessInter(predPartL);
				}
				isChromaCb = true;
				Slice->transformDecodeChromaResidualProcessInter(predPartCb, isChromaCb);
				isChromaCb = false;
				Slice->transformDecodeChromaResidualProcessInter(predPartCr, isChromaCb);
			}

		}


		if (!sHeader.pps.entropy_coding_mode_flag)
		{
			moreDataFlag = bs.more_rbsp_data();
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
				end_of_slice_flag = cabac.decode_end_of_slice_flag(bs, Slice);
				moreDataFlag = !end_of_slice_flag;
			}
		}
		CurrMbAddr = NextMbAddress(sHeader, CurrMbAddr);
	} while (moreDataFlag);


	if (Slice->mbCount == Slice->PicSizeInMbs)
	{
		Slice->decodeFinished = true;
	}
	return false;
}

SliceData::~SliceData()
{
}
