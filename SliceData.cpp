#include "SliceData.h"

SliceData::SliceData()
{
	mb_field_decoding_flag = 0;
	mb_skip_flag = 0;
}

//跳过宏块
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
	}


	//slice只有3种编码方式：帧编码、场编码或MBAFF
	//MbaffFrameFlag表示帧场自适应

	//当前解码的宏块在图片中的坐标位置
	CurrMbAddr = sHeader.first_mb_in_slice * (1 + sHeader.MbaffFrameFlag);

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
					CurrMbAddr = NextMbAddress(sHeader, CurrMbAddr);
				}

				if (mb_skip_run > 0)
				{
					//这个是循环读这个数据，读到最后那个填充字节之前没有数据了就终止这个循环。。
					//如果在rbsp_trailing_bits( )之前的RBSP中有更多数据，more_rbsp_data( ) 的返回值为TRUE,否则返回false
					//moreDataFlag = more_rbsp_data(bs);
					moreDataFlag = bs.isEmpty();
				}
			}
			else  //ae(v)表示CABAC编码
			{
				//cabac编码  暂时不做
				printError("cabac编码");
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
