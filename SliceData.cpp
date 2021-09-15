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

//跳过宏块
int SliceData::NextMbAddress(const SliceHeader& sHeader, uint32_t n)
{
	int i = n + 1;


	if (i >= sHeader.PicSizeInMbs)
	{
		printError("i >= slice_header.PicSizeInMbs(%d);");
		return -1;
	}

	//i有效且其所属片组与n不同 i=i+1,因为如果有多条带组，宏块的顺序就是乱的
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

	bool isAe = sHeader.pps.entropy_coding_mode_flag;  //ae(v)表示CABAC编码

	if (isAe)
	{
		while (!byte_aligned(bs))
		{
			/*当熵编码模式是CABAC时,
			  此时要求数据字节对齐, 即数据从下一个字节的第一个比特开始,
			  如果还没有字节对齐将出现若干个 cabac_alignment_one_bit 作为填充。*/
			cabac_alignment_one_bit = bs.readBit(); //2 f(1)
		}

		//初始化cabac
		cabac.Initialisation_process_for_context_variables((SLIECETYPE)sHeader.slice_type, sHeader.SliceQPY, sHeader.cabac_init_idc);

		cabac.Initialisation_process_for_the_arithmetic_decoding_engine(bs);
	}


	//slice只有3种编码方式：帧编码、场编码或MBAFF
	//MbaffFrameFlag表示帧场自适应

	//当前解码的宏块在图片中的坐标位置
	CurrMbAddr = sHeader.first_mb_in_slice;
	Slice->CurrMbAddr = CurrMbAddr;

	if (!sHeader.MbaffFrameFlag)
	{
		mb_field_decoding_flag = sHeader.field_pic_flag;
	}

	if (Slice->sliceCount == 0)
	{
		Slice->Decoding_process_for_picture_order_count(dpb);

		//参考图像列表  在解码每一个P,SP或B条带开始时
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

			//等于0，那么采用语法表中左边的描述符所指定的方法
			//等于1，就采用语法表中右边的描述符所指定的方法
			/*当entropy_coding_mode_flag为1，即熵编码为CABAC时 是每个“ 跳 跃 ”块都会有句法元素mb_skip_flag指明，
			而entropy_coding_mode_flag 等于 0，即熵编码为CAVLC时，
			  用一种行程的方法给出紧连着的“跳跃”块的数目，即句法元素 mb_skip_run。
			  mb_skip_run 值的范围 0 to PicSizeInMbs – CurrMbAddr  。*/
			if (!sHeader.pps.entropy_coding_mode_flag)
			{
				/*当图像采用帧间预测编码时，H.264 允许在图像平坦的区域使用“跳跃”块，“跳跃”块本身不携带任何数据，
				  解码器通过周围已重建的宏块的数据来恢复“跳跃”块。当熵编码为 CAVLC 或 CABAC 时，“跳跃”块的表示方法不同。*/
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
					//这个是循环读这个数据，读到最后那个填充字节之前没有数据了就终止这个循环。。
					//如果在rbsp_trailing_bits( )之前的RBSP中有更多数据，more_rbsp_data( ) 的返回值为TRUE,否则返回false
					//moreDataFlag = more_rbsp_data(bs);
					moreDataFlag = bs.more_rbsp_data();
				}
			}
			else  //ae(v)表示CABAC编码
			{
				Slice->mbX = (CurrMbAddr % Slice->sHeader.sps.PicWidthInMbs);
				Slice->mbY = (CurrMbAddr / Slice->sHeader.sps.PicWidthInMbs);

				Slice->CurrMbAddr = CurrMbAddr;


				mb_skip_flag = cabac.decode_mb_skip_flag(bs, Slice);


				if (mb_skip_flag)
				{
					Slice->mbCount++;
					//表示本宏块没有残差数据，相应的像素值只需要利用之前已经解码的I/P帧来预测获得

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
				printError("帧场自适应 sHeader.MbaffFrameFlag && (CurrMbAddr % 2 == 0 || (CurrMbAddr % 2 == 1 && prevMbSkipped))");
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
			else  //帧间预测
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
