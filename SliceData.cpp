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

				bool prevMbSkipped = (mb_skip_run > 0);

				for (size_t i = 0; i < mb_skip_run; i++)
				{
					//CurrMbAddr = NextMbAddress(CurrMbAddr)
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
			/*if (sHeader.MbaffFrameFlag && (CurrMbAddr % 2 == 0 || (CurrMbAddr % 2 == 1 && prevMbSkipped))) {

			}*/
		}



	} while (false);
	return false;
}

SliceData::~SliceData()
{
}
