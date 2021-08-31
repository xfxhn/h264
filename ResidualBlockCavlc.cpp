#include "ResidualBlockCavlc.h"
#include "ParseSlice.h"
#include "CalvcTable.h"


ResidualBlockCavlc::ResidualBlockCavlc(ParseSlice* slice)
{
	sliceBase = slice;
	trailing_ones_sign_flag = 0;
	/*level_suffix = 0;
	memset(&levelVal, 0, sizeof(int32_t) * 16);*/
	//memset(&runVal, 0, sizeof(int32_t) * 16);
}

bool ResidualBlockCavlc::residual_block_cavlc(
	BitStream& bs, int* coeffLevel, int startIdx, int endIdx, uint32_t maxNumCoeff, int& TotalCoeff, RESIDUAL_LEVEL residualLevel, size_t BlkIdx
)
{

	memset(coeffLevel, 0, sizeof(uint32_t) * maxNumCoeff);

	int coeff_token_length = 0;
	int TrailingOnes = 0;

	int numberCurrent = getNumberCurrent(residualLevel, BlkIdx);

	//获取16bit因为token最长只有16bit
	uint16_t coeff_token = bs.getMultiBit(16);


	getNumCoeffAndTrailingOnes(numberCurrent, coeff_token, coeff_token_length, TrailingOnes, TotalCoeff);

	//读取真正的token长度
	int a = bs.readMultiBit(coeff_token_length);




	int suffixLength = 0;
	int levelSuffixSize = 0;
	int	levelCode = 0;

	int levelVal[16] = { 0 }; //非零系数,带符号



	int	level_suffix = 0;
	int	level_prefix = 0;
	if (TotalCoeff > 0)
	{
		if (TotalCoeff > 10 && TrailingOnes < 3)
		{
			suffixLength = 1;
		}

		for (size_t i = 0; i < TotalCoeff; i++)
		{
			if (i < TrailingOnes)
			{
				trailing_ones_sign_flag = bs.readBit();
				levelVal[i] = 1 - 2 * trailing_ones_sign_flag; //3个拖尾系数，只能是1或-1   //1代表-1,0代表+1
			}
			else
			{
				//普通非零系数的幅值（Levels）由两个部分组成：前缀（level_prefix）和后缀（level_suffix）


				int leadingZeroBits = -1;

				for (size_t b = 0; !b; leadingZeroBits++)
				{
					b = bs.readBit();
				}

				level_prefix = leadingZeroBits;


				if (level_prefix == 14 && suffixLength == 0)
				{
					levelSuffixSize = 4;
				}
				else if (level_prefix >= 15)
				{
					levelSuffixSize = level_prefix - 3;
				}
				else
				{
					levelSuffixSize = suffixLength;
				}

				levelCode = (min(15, level_prefix) << suffixLength);
				if (suffixLength > 0 || level_prefix >= 14)
				{
					//level_suffix
					if (levelSuffixSize > 0)
					{
						level_suffix = bs.readMultiBit(levelSuffixSize);
					}
					else
					{
						levelSuffixSize = 0;
					}
					levelCode += level_suffix;
				}


				if (level_prefix >= 15 && suffixLength == 0)
				{
					levelCode += 15;
				}

				if (level_prefix >= 16)
				{
					levelCode += (1 << (level_prefix - 3)) - 4096;
				}

				if (i == TrailingOnes && TrailingOnes < 3)
				{
					levelCode += 2;
				}


				if (levelCode % 2 == 0)
				{
					levelVal[i] = (levelCode + 2) >> 1;
				}
				else
				{
					levelVal[i] = (-levelCode - 1) >> 1;
				}

				if (suffixLength == 0)
				{
					suffixLength = 1;
				}


				if (abs(levelVal[i]) > (3 << (suffixLength - 1)) && suffixLength < 6)
				{
					++suffixLength;
				}
			}

		}


		//最后一个非0系数之前的0的个数  TotalZeros  0, 15(4x4)
		int TotalZeros = 0;
		if (TotalCoeff < endIdx - startIdx + 1)
		{
			//解码total_zeros
			TotalZeros = getTotalZeros(bs, TotalCoeff - 1, maxNumCoeff);

		}
		else
		{
			TotalZeros = 0;
		}


		int runVal[16] = { 0 };
		//每个非0系数之前0的个数  zerosLeft
		//获取每个非0系数之前到下一个非0之间0的个数  RunBefore
		//初始化zerosLeft为TotalZeros
		int zerosLeft = TotalZeros;
		for (size_t i = 0; i < TotalCoeff - 1; i++)
		{
			if (zerosLeft > 0)
			{
				int runbeforeVlcIdx = (zerosLeft <= 6 ? zerosLeft - 1 : 6);

				runVal[i] = getRunbefore(bs, runbeforeVlcIdx);
			}
			else
			{
				runVal[i] = 0;
			}

			zerosLeft -= runVal[i];
		}

		runVal[TotalCoeff - 1] = zerosLeft;

		int coeffNum = -1;
		for (int i = TotalCoeff - 1; i >= 0; i--) {
			coeffNum += runVal[i] + 1;
			coeffLevel[startIdx + coeffNum] = levelVal[i];
		}
	}


	return false;
}

int ResidualBlockCavlc::getNumberCurrent(RESIDUAL_LEVEL residualLevel, size_t BlkIdx)
{

	int nC = 0;


	if (residualLevel == RESIDUAL_LEVEL::ChromaDCLevel)
	{
		//色度DC固定码表
		if (sliceBase->sHeader.sps.ChromaArrayType == 1)
		{
			nC = -1;
		}
		else
		{
			nC = -2;
		}
	}
	else
	{
		int nA = 0; //左侧块
		int nB = 0;	//上测块


		int luma4x4BlkIdx = BlkIdx;
		int cb4x4BlkIdx = BlkIdx;
		int cr4x4BlkIdx = BlkIdx;

		int BlkIdxA = BlkIdx;
		int BlkIdxB = BlkIdx;

		int mbAddrA = NA;
		int mbAddrB = NA;


		int chroma4x4BlkIdx = BlkIdx;
		if (residualLevel == RESIDUAL_LEVEL::Intra16x16DCLevel)
		{
			luma4x4BlkIdx = 0;
		}
		if (residualLevel == RESIDUAL_LEVEL::CbIntra16x16DCLevel)
		{
			cb4x4BlkIdx = 0;
		}
		if (residualLevel == RESIDUAL_LEVEL::CrIntra16x16DCLevel)
		{
			cr4x4BlkIdx = 0;
		}


		//亮度
		if (residualLevel == RESIDUAL_LEVEL::Intra16x16DCLevel || residualLevel == RESIDUAL_LEVEL::Intra16x16ACLevel || residualLevel == RESIDUAL_LEVEL::LumaLevel4x4)
		{

			//获取距离宏块左上角的样点位置
			int x = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 0);
			int y = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 1);



			//	int mbAddrA = NA;
			int luma4x4BlkIdxA = NA;


			int xW = NA;
			int yW = NA;

			sliceBase->getMbAddrNAndLuma4x4BlkIdxN(mbAddrA, x + (-1), y + 0, 16, 16, xW, yW);

			if (mbAddrA != NA)
			{
				//左侧宏块子块索引
				luma4x4BlkIdxA = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
			}

			//		int mbAddrB = NA;
			int luma4x4BlkIdxB = NA;
			sliceBase->getMbAddrNAndLuma4x4BlkIdxN(mbAddrB, x + 0, y + (-1), 16, 16, xW, yW);

			if (mbAddrB != NA)
			{
				//上侧宏块子块索引
				luma4x4BlkIdxB = 8 * (yW / 8) + 4 * (xW / 8) + 2 * ((yW % 8) / 4) + ((xW % 8) / 4);
			}
			BlkIdxA = luma4x4BlkIdxA;
			BlkIdxB = luma4x4BlkIdxB;
		}
		else if (residualLevel == RESIDUAL_LEVEL::CbIntra16x16DCLevel || residualLevel == RESIDUAL_LEVEL::CbIntra16x16ACLevel || residualLevel == RESIDUAL_LEVEL::CbLevel4x4)
		{
			//对于ChromaArrayType等于3的相邻4x4色度块的推导过程
			//这个过程只在ChromaArrayType等于3时被调用

			int x = InverseRasterScan(cb4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(cb4x4BlkIdx % 4, 4, 4, 8, 0);
			int y = InverseRasterScan(cb4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(cb4x4BlkIdx % 4, 4, 4, 8, 1);

			int maxW = sliceBase->sHeader.sps.MbWidthC;
			int maxH = sliceBase->sHeader.sps.MbHeightC;

			//		int mbAddrA = NA;
			int chroma4x4BlkIdxA = NA;

			int xW = NA;
			int yW = NA;

			sliceBase->getMbAddrNAndLuma4x4BlkIdxN(mbAddrA, x + (-1), y + 0, maxW, maxH, xW, yW);

			if (mbAddrA != NA)
			{
				//左侧宏块子块索引
				chroma4x4BlkIdxA = 2 * (yW / 4) + (xW / 4);
			}

			//			int mbAddrB = NA;
			int chroma4x4BlkIdxB = NA;
			sliceBase->getMbAddrNAndLuma4x4BlkIdxN(mbAddrB, x + 0, y + (-1), maxW, maxH, xW, yW);

			if (mbAddrB != NA)
			{
				//上侧宏块子块索引
				chroma4x4BlkIdxB = 2 * (yW / 4) + (xW / 4);
			}
			BlkIdxA = chroma4x4BlkIdxA;
			BlkIdxB = chroma4x4BlkIdxB;

		}
		else if (residualLevel == RESIDUAL_LEVEL::CrIntra16x16DCLevel || residualLevel == RESIDUAL_LEVEL::CrIntra16x16ACLevel || residualLevel == RESIDUAL_LEVEL::CrLevel4x4)
		{
			//对于ChromaArrayType等于3的相邻4x4色度块的推导过程
			//这个过程只在ChromaArrayType等于3时被调用

			int x = InverseRasterScan(cb4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(cb4x4BlkIdx % 4, 4, 4, 8, 0);
			int y = InverseRasterScan(cb4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(cb4x4BlkIdx % 4, 4, 4, 8, 1);

			int maxW = sliceBase->sHeader.sps.MbWidthC;
			int maxH = sliceBase->sHeader.sps.MbHeightC;

			//	int mbAddrA = NA;
			int chroma4x4BlkIdxA = NA;

			int xW = NA;
			int yW = NA;

			sliceBase->getMbAddrNAndLuma4x4BlkIdxN(mbAddrA, x + (-1), y + 0, maxW, maxH, xW, yW);

			if (mbAddrA != NA)
			{
				//左侧宏块子块索引
				chroma4x4BlkIdxA = 2 * (yW / 4) + (xW / 4);
			}

			//		int mbAddrB = NA;
			int chroma4x4BlkIdxB = NA;
			sliceBase->getMbAddrNAndLuma4x4BlkIdxN(mbAddrB, x + 0, y + (-1), maxW, maxH, xW, yW);

			if (mbAddrB != NA)
			{
				//上侧宏块子块索引
				chroma4x4BlkIdxB = 2 * (yW / 4) + (xW / 4);
			}
			BlkIdxA = chroma4x4BlkIdxA;
			BlkIdxB = chroma4x4BlkIdxB;
		}
		else if (residualLevel == RESIDUAL_LEVEL::ChromaACLevelCb || residualLevel == RESIDUAL_LEVEL::ChromaACLevelCr)
		{
			//ChromaACLevel
			//相邻4x4色度块的推导过程
			//这个子句只在ChromaArrayType等于1或2时被调用

			//相对于宏块的左上色度样本的索引为chroma4x4BlkIdx的4x4色度块的左上色度样本的位置(x, y)。
			int x = InverseRasterScan(chroma4x4BlkIdx, 4, 4, 8, 0);
			int y = InverseRasterScan(chroma4x4BlkIdx, 4, 4, 8, 1);



			int maxW = sliceBase->sHeader.sps.MbWidthC;
			int maxH = sliceBase->sHeader.sps.MbHeightC;

			//		int mbAddrA = NA;
			int chroma4x4BlkIdxA = NA;

			int xW = NA;
			int yW = NA;

			sliceBase->getMbAddrNAndLuma4x4BlkIdxN(mbAddrA, x + (-1), y + 0, maxW, maxH, xW, yW);

			if (mbAddrA != NA)
			{
				//左侧宏块子块索引
				chroma4x4BlkIdxA = 2 * (yW / 4) + (xW / 4);
			}

			//		int mbAddrB = NA;
			int chroma4x4BlkIdxB = NA;
			sliceBase->getMbAddrNAndLuma4x4BlkIdxN(mbAddrB, x + 0, y + (-1), maxW, maxH, xW, yW);

			if (mbAddrB != NA)
			{
				//上侧宏块子块索引
				chroma4x4BlkIdxB = 2 * (yW / 4) + (xW / 4);
			}
			BlkIdxA = chroma4x4BlkIdxA;
			BlkIdxB = chroma4x4BlkIdxB;
		}


		bool availableFlagA = false;
		bool availableFlagB = false;
		if (mbAddrA == NA ||
			(isInterMode(sliceBase->macroblock[sliceBase->CurrMbAddr]->mode) && sliceBase->sHeader.pps.constrained_intra_pred_flag)
			&& isInterframe(sliceBase->macroblock[mbAddrA]->mode)
			&& sliceBase->sHeader.nalu.nal_unit_type >= 2 && sliceBase->sHeader.nalu.nal_unit_type <= 4)
		{
			availableFlagA = false;
		}
		else
		{
			availableFlagA = true;
		}

		if (mbAddrB == NA ||
			(isInterMode(sliceBase->macroblock[sliceBase->CurrMbAddr]->mode) && sliceBase->sHeader.pps.constrained_intra_pred_flag)
			&& isInterframe(sliceBase->macroblock[mbAddrB]->mode)
			&& sliceBase->sHeader.nalu.nal_unit_type >= 2 && sliceBase->sHeader.nalu.nal_unit_type <= 4)
		{
			availableFlagB = false;
		}
		else
		{
			availableFlagB = true;
		}


		if (availableFlagA)
		{
			if (sliceBase->macroblock[mbAddrA]->mbType == H264_MB_TYPE::P_Skip || sliceBase->macroblock[mbAddrA]->mbType == H264_MB_TYPE::B_Skip)
			{
				nA = 0;
			}
			else if (sliceBase->macroblock[mbAddrA]->mbType == H264_MB_TYPE::I_PCM) //if mbAddrN is an I_PCM macroblock, nN is set equal to 16.
			{
				nA = 16;
			}
			else
			{
				if (residualLevel == RESIDUAL_LEVEL::Intra16x16DCLevel || residualLevel == RESIDUAL_LEVEL::Intra16x16ACLevel || residualLevel == RESIDUAL_LEVEL::LumaLevel4x4)
				{
					nA = sliceBase->macroblock[mbAddrA]->mb_luma_4x4_non_zero_count_coeff[BlkIdxA];
				}
				else if (residualLevel == RESIDUAL_LEVEL::CbIntra16x16DCLevel || residualLevel == RESIDUAL_LEVEL::CbIntra16x16ACLevel || residualLevel == RESIDUAL_LEVEL::CbLevel4x4)
				{
					nA = sliceBase->macroblock[mbAddrA]->mb_chroma_4x4_non_zero_count_coeff[0][BlkIdxA];
				}
				else if (residualLevel == RESIDUAL_LEVEL::CrIntra16x16DCLevel || residualLevel == RESIDUAL_LEVEL::CrIntra16x16ACLevel || residualLevel == RESIDUAL_LEVEL::CrLevel4x4)
				{
					nA = sliceBase->macroblock[mbAddrA]->mb_chroma_4x4_non_zero_count_coeff[1][BlkIdxA];
				}
				else if (residualLevel == RESIDUAL_LEVEL::ChromaACLevelCb)
				{
					nA = sliceBase->macroblock[mbAddrA]->mb_chroma_4x4_non_zero_count_coeff[0][BlkIdxA];
				}
				else if (residualLevel == RESIDUAL_LEVEL::ChromaACLevelCr)
				{
					nA = sliceBase->macroblock[mbAddrA]->mb_chroma_4x4_non_zero_count_coeff[1][BlkIdxA];
				}
			}
		}



		if (availableFlagB)
		{
			if (sliceBase->macroblock[mbAddrB]->mbType == H264_MB_TYPE::P_Skip || sliceBase->macroblock[mbAddrB]->mbType == H264_MB_TYPE::B_Skip)
			{
				nB = 0;
			}
			else if (sliceBase->macroblock[mbAddrB]->mbType == H264_MB_TYPE::I_PCM) //if mbAddrN is an I_PCM macroblock, nN is set equal to 16.
			{
				nB = 16;
			}
			else
			{
				if (residualLevel == RESIDUAL_LEVEL::Intra16x16DCLevel || residualLevel == RESIDUAL_LEVEL::Intra16x16ACLevel || residualLevel == RESIDUAL_LEVEL::LumaLevel4x4)
				{
					nB = sliceBase->macroblock[mbAddrB]->mb_luma_4x4_non_zero_count_coeff[BlkIdxB];
				}
				else if (residualLevel == RESIDUAL_LEVEL::CbIntra16x16DCLevel || residualLevel == RESIDUAL_LEVEL::CbIntra16x16ACLevel || residualLevel == RESIDUAL_LEVEL::CbLevel4x4)
				{
					nB = sliceBase->macroblock[mbAddrB]->mb_chroma_4x4_non_zero_count_coeff[0][BlkIdxB];
				}
				else if (residualLevel == RESIDUAL_LEVEL::CrIntra16x16DCLevel || residualLevel == RESIDUAL_LEVEL::CrIntra16x16ACLevel || residualLevel == RESIDUAL_LEVEL::CrLevel4x4)
				{
					nB = sliceBase->macroblock[mbAddrB]->mb_chroma_4x4_non_zero_count_coeff[1][BlkIdxB];
				}
				else if (residualLevel == RESIDUAL_LEVEL::ChromaACLevelCb)
				{
					nB = sliceBase->macroblock[mbAddrB]->mb_chroma_4x4_non_zero_count_coeff[0][BlkIdxB];
				}
				else if (residualLevel == RESIDUAL_LEVEL::ChromaACLevelCr)
				{
					nB = sliceBase->macroblock[mbAddrB]->mb_chroma_4x4_non_zero_count_coeff[1][BlkIdxB];
				}
			}
		}

		if (availableFlagA && availableFlagB)
		{
			nC = (nA + nB + 1) >> 1;
		}
		else if (availableFlagA && !availableFlagB)
		{
			nC = nA;
		}
		else if (availableFlagB && !availableFlagA)
		{
			nC = nB;
		}
		else
		{
			nC = 0;
		}
	}

	return nC;
}

int ResidualBlockCavlc::getNumCoeffAndTrailingOnes(int nC, uint16_t coeff_token, int& coeff_token_length, int& TrailingOnes, int& TotalCoeff)
{

	if (0 <= nC && nC < 2)
	{
		if ((coeff_token >> 15) == 1) //(1)b
		{
			coeff_token_length = 1;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 10) == 5) //(0001 01)b ==> (00 0101)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 14) == 1) //(01)b
		{
			coeff_token_length = 2;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 8) == 7) //(0000 0111)b
		{
			coeff_token_length = 8;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 4) //(0001 00)b ==> (00 0100)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 13) == 1) //(001)b
		{
			coeff_token_length = 3;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 7) == 7) //(0000 0011 1)b ==> (0 0000 0111)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 8) == 6) //(0000 0110)b
		{
			coeff_token_length = 8;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 5) //(0000 101)b ==> (000 0101)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 11) == 3) //(0001 1)b ==> (0 0011)b
		{
			coeff_token_length = 5;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 6) == 7) //(0000 0001 11)b ==> (00 0000 0111)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 7) == 6) //(0000 0011 0)b ==> (0 0000 0110)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 8) == 5) //(0000 0101)b ==> (0000 0101)b
		{
			coeff_token_length = 8;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 3) //(0000 11)b ==> (00 0011)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 5) == 7) //(0000 0000 111)b ==> (000 0000 0111)b
		{
			coeff_token_length = 11;
			TrailingOnes = 0;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 6) == 6) //(0000 0001 10)b ==> (00 0000 0110)b
		{
			coeff_token_length = 10;
			TrailingOnes = 1;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 7) == 5) //(0000 0010 1)b ==> (0 0000 0101)b
		{
			coeff_token_length = 9;
			TrailingOnes = 2;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 9) == 4) //(0000 100)b ==> (000 0100)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 3) == 15) //(0000 0000 0111 1)b ==> (0 0000 0000 1111)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 5) == 6) //(0000 0000 110)b ==> (000 0000 0110)b
		{
			coeff_token_length = 11;
			TrailingOnes = 1;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 6) == 5) //(0000 0001 01)b ==> (00 0000 0101)b
		{
			coeff_token_length = 10;
			TrailingOnes = 2;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 8) == 4) //(0000 0100)b ==> (0000 0100)b
		{
			coeff_token_length = 8;
			TrailingOnes = 3;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 3) == 11) //(0000 0000 0101 1)b ==> (0 0000 0000 1011)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 3) == 14) //(0000 0000 0111 0)b ==> (0 0000 0000 1110)b
		{
			coeff_token_length = 13;
			TrailingOnes = 1;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 5) == 5) //(0000 0000 101)b ==> (000 0000 0101)b
		{
			coeff_token_length = 11;
			TrailingOnes = 2;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 7) == 4) //(0000 0010 0)b ==> (0 0000 0100)b
		{
			coeff_token_length = 9;
			TrailingOnes = 3;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 3) == 8) //(0000 0000 0100 0)b ==> (0 0000 0000 1000)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 3) == 10) //(0000 0000 0101 0)b ==> (0 0000 0000 1010)b
		{
			coeff_token_length = 13;
			TrailingOnes = 1;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 3) == 13) //(0000 0000 0110 1)b ==> (0 0000 0000 1101)b
		{
			coeff_token_length = 13;
			TrailingOnes = 2;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 6) == 4) //(0000 0001 00)b ==> (00 0000 0100)b
		{
			coeff_token_length = 10;
			TrailingOnes = 3;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 2) == 15) //(0000 0000 0011 11)b ==> (00 0000 0000 1111)b
		{
			coeff_token_length = 14;
			TrailingOnes = 0;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 2) == 14) //(0000 0000 0011 10)b ==> (00 0000 0000 1110)b
		{
			coeff_token_length = 14;
			TrailingOnes = 1;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 3) == 9) //(0000 0000 0100 1)b ==> (0 0000 0000 1001)b
		{
			coeff_token_length = 13;
			TrailingOnes = 2;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 5) == 4) //(0000 0000 100)b ==> (000 0000 0100)b
		{
			coeff_token_length = 11;
			TrailingOnes = 3;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 2) == 11) //(0000 0000 0010 11)b ==> (00 0000 0000 1011)b
		{
			coeff_token_length = 14;
			TrailingOnes = 0;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 2) == 10) //(0000 0000 0010 10)b ==> (00 0000 0000 1010)b
		{
			coeff_token_length = 14;
			TrailingOnes = 1;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 2) == 13) //(0000 0000 0011 01)b ==> (00 0000 0000 1101)b
		{
			coeff_token_length = 14;
			TrailingOnes = 2;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 3) == 12) //(0000 0000 0110 0)b ==> (0 0000 0000 1100)b
		{
			coeff_token_length = 13;
			TrailingOnes = 3;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 1) == 15) //(0000 0000 0001 111)b ==> (000 0000 0000 1111)b
		{
			coeff_token_length = 15;
			TrailingOnes = 0;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 1) == 14) //(0000 0000 0001 110)b ==> (000 0000 0000 1110)b
		{
			coeff_token_length = 15;
			TrailingOnes = 1;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 2) == 9) //(0000 0000 0010 01)b ==> (00 0000 0000 1001)b
		{
			coeff_token_length = 14;
			TrailingOnes = 2;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 2) == 12) //(0000 0000 0011 00)b ==> (00 0000 0000 1100)b
		{
			coeff_token_length = 14;
			TrailingOnes = 3;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 1) == 11) //(0000 0000 0001 011)b ==> (000 0000 0000 1011)b
		{
			coeff_token_length = 15;
			TrailingOnes = 0;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 1) == 10) //(0000 0000 0001 010)b ==> (000 0000 0000 1010)b
		{
			coeff_token_length = 15;
			TrailingOnes = 1;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 1) == 13) //(0000 0000 0001 101)b ==> (000 0000 0000 1101)b
		{
			coeff_token_length = 15;
			TrailingOnes = 2;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 2) == 8) //(0000 0000 0010 00)b ==> (00 0000 0000 1000)b
		{
			coeff_token_length = 14;
			TrailingOnes = 3;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 0) == 15) //(0000 0000 0000 1111)b ==> (0000 0000 0000 1111)b
		{
			coeff_token_length = 16;
			TrailingOnes = 0;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 1) == 1) //(0000 0000 0000 001)b ==> (000 0000 0000 0001)b
		{
			coeff_token_length = 15;
			TrailingOnes = 1;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 1) == 9) //(0000 0000 0001 001)b ==> (000 0000 0000 1001)b
		{
			coeff_token_length = 15;
			TrailingOnes = 2;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 1) == 12) //(0000 0000 0001 100)b ==> (000 0000 0000 1100)b
		{
			coeff_token_length = 15;
			TrailingOnes = 3;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 0) == 11) //(0000 0000 0000 1011)b ==> (0000 0000 0000 1011)b
		{
			coeff_token_length = 16;
			TrailingOnes = 0;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 0) == 14) //(0000 0000 0000 1110)b ==> (0000 0000 0000 1110)b
		{
			coeff_token_length = 16;
			TrailingOnes = 1;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 0) == 13) //(0000 0000 0000 1101)b ==> (0000 0000 0000 1101)b
		{
			coeff_token_length = 16;
			TrailingOnes = 2;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 1) == 8) //(0000 0000 0001 000)b ==> (000 0000 0000 1000)b
		{
			coeff_token_length = 15;
			TrailingOnes = 3;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 0) == 7) //(0000 0000 0000 0111)b ==> (0000 0000 0000 0111)b
		{
			coeff_token_length = 16;
			TrailingOnes = 0;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 0) == 10) //(0000 0000 0000 1010)b ==> (0000 0000 0000 1010)b
		{
			coeff_token_length = 16;
			TrailingOnes = 1;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 0) == 9) //(0000 0000 0000 1001)b ==> (0000 0000 0000 1001)b
		{
			coeff_token_length = 16;
			TrailingOnes = 2;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 0) == 12) //(0000 0000 0000 1100)b ==> (0000 0000 0000 1100)b
		{
			coeff_token_length = 16;
			TrailingOnes = 3;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 0) == 4) //(0000 0000 0000 0100)b ==> (0000 0000 0000 0100)b
		{
			coeff_token_length = 16;
			TrailingOnes = 0;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 0) == 6) //(0000 0000 0000 0110)b ==> (0000 0000 0000 0110)b
		{
			coeff_token_length = 16;
			TrailingOnes = 1;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 0) == 5) //(0000 0000 0000 0101)b ==> (0000 0000 0000 0101)b
		{
			coeff_token_length = 16;
			TrailingOnes = 2;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 0) == 8) //(0000 0000 0000 1000)b ==> (0000 0000 0000 1000)b
		{
			coeff_token_length = 16;
			TrailingOnes = 3;
			TotalCoeff = 16;
		}
	}
	else if (2 <= nC && nC < 4)
	{
		if ((coeff_token >> 14) == 0x0003) // (11)b
		{
			coeff_token_length = 2;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 10) == 0x000B) // (0010 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 14) == 0x0002) // (10)b
		{
			coeff_token_length = 2;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 10) == 0x0007) // (0001 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 11) == 0x0007) // (0011 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 13) == 0x0003) // (011)b
		{
			coeff_token_length = 3;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 9) == 0x0007) // (0000 111)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x000A) // (0010 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x0009) // (0010 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 12) == 0x0005) // (0101)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 8) == 0x0007) // (0000 0111)b
		{
			coeff_token_length = 8;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x0006) // (0001 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x0005) // (0001 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 12) == 0x0004) // (0100)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 8) == 0x0004) // (0000 0100)b
		{
			coeff_token_length = 8;
			TrailingOnes = 0;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 9) == 0x0006) // (0000 110)b
		{
			coeff_token_length = 7;
			TrailingOnes = 1;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 9) == 0x0005) // (0000 101)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 11) == 0x0006) // (0011 0)b
		{
			coeff_token_length = 5;
			TrailingOnes = 3;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 7) == 0x0007) // (0000 0011 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 8) == 0x0006) // (0000 0110)b
		{
			coeff_token_length = 8;
			TrailingOnes = 1;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 8) == 0x0005) // (0000 0101)b
		{
			coeff_token_length = 8;
			TrailingOnes = 2;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x0008) // (0010 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 5) == 0x000F) // (0000 0001 111)b
		{
			coeff_token_length = 11;
			TrailingOnes = 0;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 7) == 0x0006) // (0000 0011 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 7) == 0x0005) // (0000 0010 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 2;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x0004) // (0001 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 5) == 0x000B) // (0000 0001 011)b
		{
			coeff_token_length = 11;
			TrailingOnes = 0;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 5) == 0x000E) // (0000 0001 110)b
		{
			coeff_token_length = 11;
			TrailingOnes = 1;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 5) == 0x000D) // (0000 0001 101)b
		{
			coeff_token_length = 11;
			TrailingOnes = 2;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 9) == 0x0004) // (0000 100)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 4) == 0x000F) // (0000 0000 1111)b
		{
			coeff_token_length = 12;
			TrailingOnes = 0;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 5) == 0x000A) // (0000 0001 010)b
		{
			coeff_token_length = 11;
			TrailingOnes = 1;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 5) == 0x0009) // (0000 0001 001)b
		{
			coeff_token_length = 11;
			TrailingOnes = 2;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 7) == 0x0004) // (0000 0010 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 3;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 4) == 0x000B) // (0000 0000 1011)b
		{
			coeff_token_length = 12;
			TrailingOnes = 0;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 4) == 0x000E) // (0000 0000 1110)b
		{
			coeff_token_length = 12;
			TrailingOnes = 1;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 4) == 0x000D) // (0000 0000 1101)b
		{
			coeff_token_length = 12;
			TrailingOnes = 2;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 5) == 0x000C) // (0000 0001 100)b
		{
			coeff_token_length = 11;
			TrailingOnes = 3;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 4) == 0x0008) // (0000 0000 1000)b
		{
			coeff_token_length = 12;
			TrailingOnes = 0;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 4) == 0x000A) // (0000 0000 1010)b
		{
			coeff_token_length = 12;
			TrailingOnes = 1;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 4) == 0x0009) // (0000 0000 1001)b
		{
			coeff_token_length = 12;
			TrailingOnes = 2;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 5) == 0x0008) // (0000 0001 000)b
		{
			coeff_token_length = 11;
			TrailingOnes = 3;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 3) == 0x000F) // (0000 0000 0111 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 3) == 0x000E) // (0000 0000 0111 0)b
		{
			coeff_token_length = 13;
			TrailingOnes = 1;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 3) == 0x000D) // (0000 0000 0110 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 2;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 4) == 0x000C) // (0000 0000 1100)b
		{
			coeff_token_length = 12;
			TrailingOnes = 3;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 3) == 0x000B) // (0000 0000 0101 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 3) == 0x000A) // (0000 0000 0101 0)b
		{
			coeff_token_length = 13;
			TrailingOnes = 1;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 3) == 0x0009) // (0000 0000 0100 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 2;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 3) == 0x000C) // (0000 0000 0110 0)b
		{
			coeff_token_length = 13;
			TrailingOnes = 3;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 3) == 0x0007) // (0000 0000 0011 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 2) == 0x000B) // (0000 0000 0010 11)b
		{
			coeff_token_length = 14;
			TrailingOnes = 1;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 3) == 0x0006) // (0000 0000 0011 0)b
		{
			coeff_token_length = 13;
			TrailingOnes = 2;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 3) == 0x0008) // (0000 0000 0100 0)b
		{
			coeff_token_length = 13;
			TrailingOnes = 3;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 2) == 0x0009) // (0000 0000 0010 01)b
		{
			coeff_token_length = 14;
			TrailingOnes = 0;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 2) == 0x0008) // (0000 0000 0010 00)b
		{
			coeff_token_length = 14;
			TrailingOnes = 1;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 2) == 0x000A) // (0000 0000 0010 10)b
		{
			coeff_token_length = 14;
			TrailingOnes = 2;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 3) == 0x0001) // (0000 0000 0000 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 3;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 2) == 0x0007) // (0000 0000 0001 11)b
		{
			coeff_token_length = 14;
			TrailingOnes = 0;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 2) == 0x0006) // (0000 0000 0001 10)b
		{
			coeff_token_length = 14;
			TrailingOnes = 1;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 2) == 0x0005) // (0000 0000 0001 01)b
		{
			coeff_token_length = 14;
			TrailingOnes = 2;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 2) == 0x0004) // (0000 0000 0001 00)b
		{
			coeff_token_length = 14;
			TrailingOnes = 3;
			TotalCoeff = 16;
		}
	}
	else if (4 <= nC && nC < 8)
	{
		if ((coeff_token >> 12) == 0x000F) // (1111)b
		{
			coeff_token_length = 4;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 10) == 0x000F) // (0011 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 12) == 0x000E) // (1110)b
		{
			coeff_token_length = 4;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 10) == 0x000B) // (0010 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 11) == 0x000F) // (0111 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 12) == 0x000D) // (1101)b
		{
			coeff_token_length = 4;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0008) // (0010 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 11) == 0x000C) // (0110 0)b
		{
			coeff_token_length = 5;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 11) == 0x000E) // (0111 0)b
		{
			coeff_token_length = 5;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 12) == 0x000C) // (1100)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 0x000F) // (0001 111)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 11) == 0x000A) // (0101 0)b
		{
			coeff_token_length = 5;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 11) == 0x000B) // (0101 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 12) == 0x000B) // (1011)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 9) == 0x000B) // (0001 011)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 11) == 0x0008) // (0100 0)b
		{
			coeff_token_length = 5;
			TrailingOnes = 1;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 11) == 0x0009) // (0100 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 2;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 12) == 0x000A) // (1010)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 9) == 0x0009) // (0001 001)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x000E) // (0011 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x000D) // (0011 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 12) == 0x0009) // (1001)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 9) == 0x0008) // (0001 000)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x000A) // (0010 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x0009) // (0010 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 12) == 0x0008) // (1000)b
		{
			coeff_token_length = 4;
			TrailingOnes = 3;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 8) == 0x000F) // (0000 1111)b
		{
			coeff_token_length = 8;
			TrailingOnes = 0;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 9) == 0x000E) // (0001 110)b
		{
			coeff_token_length = 7;
			TrailingOnes = 1;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 9) == 0x000D) // (0001 101)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 11) == 0x000D) // (0110 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 3;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 8) == 0x000B) // (0000 1011)b
		{
			coeff_token_length = 8;
			TrailingOnes = 0;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 8) == 0x000E) // (0000 1110)b
		{
			coeff_token_length = 8;
			TrailingOnes = 1;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 9) == 0x000A) // (0001 010)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 10) == 0x000C) // (0011 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 7) == 0x000F) // (0000 0111 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 8) == 0x000A) // (0000 1010)b
		{
			coeff_token_length = 8;
			TrailingOnes = 1;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 8) == 0x000D) // (0000 1101)b
		{
			coeff_token_length = 8;
			TrailingOnes = 2;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 9) == 0x000C) // (0001 100)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 7) == 0x000B) // (0000 0101 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 7) == 0x000E) // (0000 0111 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 8) == 0x0009) // (0000 1001)b
		{
			coeff_token_length = 8;
			TrailingOnes = 2;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 8) == 0x000C) // (0000 1100)b
		{
			coeff_token_length = 8;
			TrailingOnes = 3;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 7) == 0x0008) // (0000 0100 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 7) == 0x000A) // (0000 0101 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 7) == 0x000D) // (0000 0110 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 2;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 8) == 0x0008) // (0000 1000)b
		{
			coeff_token_length = 8;
			TrailingOnes = 3;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 6) == 0x000D) // (0000 0011 01)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 7) == 0x0007) // (0000 0011 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 7) == 0x0009) // (0000 0100 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 2;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 7) == 0x000C) // (0000 0110 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 3;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 6) == 0x0009) // (0000 0010 01)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 6) == 0x000C) // (0000 0011 00)b
		{
			coeff_token_length = 10;
			TrailingOnes = 1;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 6) == 0x000B) // (0000 0010 11)b
		{
			coeff_token_length = 10;
			TrailingOnes = 2;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 6) == 0x000A) // (0000 0010 10)b
		{
			coeff_token_length = 10;
			TrailingOnes = 3;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 6) == 0x0005) // (0000 0001 01)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 6) == 0x0008) // (0000 0010 00)b
		{
			coeff_token_length = 10;
			TrailingOnes = 1;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 6) == 0x0007) // (0000 0001 11)b
		{
			coeff_token_length = 10;
			TrailingOnes = 2;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 6) == 0x0006) // (0000 0001 10)b
		{
			coeff_token_length = 10;
			TrailingOnes = 3;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 6) == 0x0001) // (0000 0000 01)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 6) == 0x0004) // (0000 0001 00)b
		{
			coeff_token_length = 10;
			TrailingOnes = 1;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 6) == 0x0003) // (0000 0000 11)b
		{
			coeff_token_length = 10;
			TrailingOnes = 2;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 6) == 0x0002) // (0000 0000 10)b
		{
			coeff_token_length = 10;
			TrailingOnes = 3;
			TotalCoeff = 16;
		}
	}
	else if (8 <= nC)
	{
		if ((coeff_token >> 10) == 0x0003) // (0000 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 10) == 0x0000) // (0000 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 10) == 0x0001) // (0000 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 10) == 0x0004) // (0001 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0005) // (0001 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0006) // (0001 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0008) // (0010 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x0009) // (0010 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x000A) // (0010 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x000B) // (0010 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x000C) // (0011 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x000D) // (0011 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x000E) // (0011 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x000F) // (0011 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x0010) // (0100 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 10) == 0x0011) // (0100 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 10) == 0x0012) // (0100 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 10) == 0x0013) // (0100 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 10) == 0x0014) // (0101 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x0015) // (0101 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x0016) // (0101 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x0017) // (0101 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 10) == 0x0018) // (0110 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x0019) // (0110 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x001A) // (0110 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x001B) // (0110 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 10) == 0x001C) // (0111 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 10) == 0x001D) // (0111 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 10) == 0x001E) // (0111 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 10) == 0x001F) // (0111 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 10) == 0x0020) // (1000 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 10) == 0x0021) // (1000 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 10) == 0x0022) // (1000 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 10) == 0x0023) // (1000 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 9;
		}
		else if ((coeff_token >> 10) == 0x0024) // (1001 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 10) == 0x0025) // (1001 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 10) == 0x0026) // (1001 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 10) == 0x0027) // (1001 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 10;
		}
		else if ((coeff_token >> 10) == 0x0028) // (1010 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 10) == 0x0029) // (1010 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 10) == 0x002A) // (1010 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 10) == 0x002B) // (1010 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 11;
		}
		else if ((coeff_token >> 10) == 0x002C) // (1011 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 10) == 0x002D) // (1011 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 10) == 0x002E) // (1011 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 10) == 0x002F) // (1011 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 12;
		}
		else if ((coeff_token >> 10) == 0x0030) // (1100 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 10) == 0x0031) // (1100 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 10) == 0x0032) // (1100 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 10) == 0x0033) // (1100 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 13;
		}
		else if ((coeff_token >> 10) == 0x0034) // (1101 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 10) == 0x0035) // (1101 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 10) == 0x0036) // (1101 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 10) == 0x0037) // (1101 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 14;
		}
		else if ((coeff_token >> 10) == 0x0038) // (1110 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 10) == 0x0039) // (1110 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 10) == 0x003A) // (1110 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 10) == 0x003B) // (1110 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 15;
		}
		else if ((coeff_token >> 10) == 0x003C) // (1111 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 10) == 0x003D) // (1111 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 10) == 0x003E) // (1111 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 2;
			TotalCoeff = 16;
		}
		else if ((coeff_token >> 10) == 0x003F) // (1111 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 16;
		}
	}
	else if (-1 == nC)
	{
		if ((coeff_token >> 14) == 0x0001) // (01)b
		{
			coeff_token_length = 2;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 10) == 0x0007) // (0001 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 15) == 0x0001) // (1)b
		{
			coeff_token_length = 1;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 10) == 0x0004) // (0001 00)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0006) // (0001 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 13) == 0x0001) // (001)b
		{
			coeff_token_length = 3;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 10) == 0x0003) // (0000 11)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 0x0003) // (0000 011)b
		{
			coeff_token_length = 7;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 0x0002) // (0000 010)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x0005) // (0001 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 10) == 0x0002) // (0000 10)b
		{
			coeff_token_length = 6;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 8) == 0x0003) // (0000 0011)b
		{
			coeff_token_length = 8;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 8) == 0x0002) // (0000 0010)b
		{
			coeff_token_length = 8;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 9) == 0x0000) // (0000 000)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
	}
	else if (-2 == nC)
	{
		if ((coeff_token >> 15) == 0x0001) // (1)b
		{
			coeff_token_length = 1;
			TrailingOnes = 0;
			TotalCoeff = 0;
		}
		else if ((coeff_token >> 9) == 0x000F) // (0001 111)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 14) == 0x0001) // (01)b
		{
			coeff_token_length = 2;
			TrailingOnes = 1;
			TotalCoeff = 1;
		}
		else if ((coeff_token >> 9) == 0x000E) // (0001 110)b
		{
			coeff_token_length = 7;
			TrailingOnes = 0;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 9) == 0x000D) // (0001 101)b
		{
			coeff_token_length = 7;
			TrailingOnes = 1;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 13) == 0x0001) // (001)b
		{
			coeff_token_length = 3;
			TrailingOnes = 2;
			TotalCoeff = 2;
		}
		else if ((coeff_token >> 7) == 0x0007) // (0000 0011 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 0x000C) // (0001 100)b
		{
			coeff_token_length = 7;
			TrailingOnes = 1;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 9) == 0x000B) // (0001 011)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 11) == 0x0001) // (0000 1)b
		{
			coeff_token_length = 5;
			TrailingOnes = 3;
			TotalCoeff = 3;
		}
		else if ((coeff_token >> 7) == 0x0006) // (0000 0011 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 0;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 7) == 0x0005) // (0000 0010 1)b
		{
			coeff_token_length = 9;
			TrailingOnes = 1;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 9) == 0x000A) // (0001 010)b
		{
			coeff_token_length = 7;
			TrailingOnes = 2;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 10) == 0x0001) // (0000 01)b
		{
			coeff_token_length = 6;
			TrailingOnes = 3;
			TotalCoeff = 4;
		}
		else if ((coeff_token >> 6) == 0x0007) // (0000 0001 11)b
		{
			coeff_token_length = 10;
			TrailingOnes = 0;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 6) == 0x0006) // (0000 0001 10)b
		{
			coeff_token_length = 10;
			TrailingOnes = 1;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 7) == 0x0004) // (0000 0010 0)b
		{
			coeff_token_length = 9;
			TrailingOnes = 2;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 9) == 0x0009) // (0001 001)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 5;
		}
		else if ((coeff_token >> 5) == 0x0007) // (0000 0000 111)b
		{
			coeff_token_length = 11;
			TrailingOnes = 0;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 5) == 0x0006) // (0000 0000 110)b
		{
			coeff_token_length = 11;
			TrailingOnes = 1;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 6) == 0x0005) // (0000 0001 01)b
		{
			coeff_token_length = 10;
			TrailingOnes = 2;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 9) == 0x0008) // (0001 000)b
		{
			coeff_token_length = 7;
			TrailingOnes = 3;
			TotalCoeff = 6;
		}
		else if ((coeff_token >> 4) == 0x0007) // (0000 0000 0111)b
		{
			coeff_token_length = 12;
			TrailingOnes = 0;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 4) == 0x0006) // (0000 0000 0110)b
		{
			coeff_token_length = 12;
			TrailingOnes = 1;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 5) == 0x0005) // (0000 0000 101)b
		{
			coeff_token_length = 11;
			TrailingOnes = 2;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 6) == 0x0004) // (0000 0001 00)b
		{
			coeff_token_length = 10;
			TrailingOnes = 3;
			TotalCoeff = 7;
		}
		else if ((coeff_token >> 3) == 0x0007) // (0000 0000 0011 1)b
		{
			coeff_token_length = 13;
			TrailingOnes = 0;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 4) == 0x0005) // (0000 0000 0101)b
		{
			coeff_token_length = 12;
			TrailingOnes = 1;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 4) == 0x0004) // (0000 0000 0100)b
		{
			coeff_token_length = 12;
			TrailingOnes = 2;
			TotalCoeff = 8;
		}
		else if ((coeff_token >> 5) == 0x0004) // (0000 0000 100)b
		{
			coeff_token_length = 11;
			TrailingOnes = 3;
			TotalCoeff = 8;
		}
	}
	return 0;
}

int ResidualBlockCavlc::getTotalZeros(BitStream& bs, uint32_t TotalCoeff, uint32_t maxNumCoeff)
{
	int TotalZeros = 0;
	if (maxNumCoeff == 4)
	{
		//表9-9 (a)
		int* lengthTable = totalZerosTableChromaDCLength_420[TotalCoeff];
		int* codeTable = totalZerosTableChromaDCCode_420[TotalCoeff];
		TotalZeros = findTable(bs, 4, lengthTable, codeTable);

	}
	else if (maxNumCoeff == 8)
	{
		//表9 - 9 (b)
		int* lengthTable = totalZerosTableChromaDCLength_422[TotalCoeff];
		int* codeTable = totalZerosTableChromaDCCode_422[TotalCoeff];
		TotalZeros = findTable(bs, 8, lengthTable, codeTable);

	}
	else
	{
		//表9-7和表9-8
		int* lengthTable = totalZerosTableLength[TotalCoeff];
		int* codeTable = totalZerosTableCode[TotalCoeff];

		TotalZeros = findTable(bs, 16, lengthTable, codeTable);

	}


	return TotalZeros;
}

int ResidualBlockCavlc::getRunbefore(BitStream& bs, uint32_t zerosLeft)
{

	int* tableLength = runBeforeTableLength[zerosLeft];
	int* tableCode = runBeforeTableCode[zerosLeft];

	int runbefore = findTable(bs, 15, tableLength, tableCode);

	return runbefore;
}

int ResidualBlockCavlc::findTable(BitStream& bs, int maxNumber, int* lengthTable, int* codeTable)
{

	int ret = 0;
	for (size_t i = 0; i < maxNumber; i++)
	{
		const int codeLen = lengthTable[i];
		if (codeLen == 0)
		{
			break;
		}

		if (bs.getMultiBit(codeLen) == codeTable[i])
		{
			ret = i;
			bs.readMultiBit(codeLen);
			break;
		}
	}
	return ret;
}




int ResidualBlockCavlc::findChromaAC_nA_nB(const int CbCr, const int BlkIdx, int& nA, int& nB, bool& availableLeft, bool& availableTop)
{

	//const int MbWidthC = sliceBase->sHeader.sps.MbWidthC;    //色度宏块宽度 8
	//const int MbHeightC = sliceBase->sHeader.sps.MbHeightC;	//色度宏块高度 8

	if (sliceBase->sHeader.sps.ChromaArrayType == 1)//420
	{
		if (BlkIdx % 2 == 0)
		{
			if (sliceBase->mbX > 0)
			{
				const int leftMbIdx = sliceBase->CurrMbAddr - 1;
				const int leftIdx = BlkIdx + 1;

				nA = sliceBase->macroblock[leftMbIdx]->mb_chroma_4x4_non_zero_count_coeff[CbCr][leftIdx];
				availableLeft = true;
			}
		}
		else
		{
			nA = sliceBase->macroblock[sliceBase->CurrMbAddr]->mb_chroma_4x4_non_zero_count_coeff[CbCr][BlkIdx - 1];
			availableLeft = true;
		}


		if (BlkIdx < 2)
		{
			if (sliceBase->mbY > 0)
			{
				const int topMbIdx = sliceBase->CurrMbAddr - sliceBase->sHeader.sps.PicWidthInMbs;
				const int leftIdx = BlkIdx + 2;
				nB = sliceBase->macroblock[topMbIdx]->mb_chroma_4x4_non_zero_count_coeff[CbCr][leftIdx];
				availableTop = true;
			}
		}
		else
		{
			nB = sliceBase->macroblock[sliceBase->CurrMbAddr]->mb_chroma_4x4_non_zero_count_coeff[CbCr][BlkIdx - 2];
			availableTop = true;
		}
	}
	else if (sliceBase->sHeader.sps.ChromaArrayType == 2)//422
	{
		//2 1
		if (BlkIdx % 2 == 0)
		{
			if (sliceBase->mbX > 0)
			{
				const int leftMbIdx = sliceBase->CurrMbAddr - 1;
				const int leftIdx = BlkIdx + 1;
				nA = sliceBase->macroblock[leftMbIdx]->mb_chroma_4x4_non_zero_count_coeff[CbCr][leftIdx];
				availableLeft = true;
			}
		}
		else
		{
			nA = sliceBase->macroblock[sliceBase->CurrMbAddr]->mb_chroma_4x4_non_zero_count_coeff[CbCr][BlkIdx - 1];
			availableLeft = true;
		}



		if (BlkIdx < 2)
		{
			if (sliceBase->mbY > 0)
			{
				const int topMbIdx = sliceBase->CurrMbAddr - sliceBase->sHeader.sps.PicWidthInMbs;
				const int leftIdx = BlkIdx + 6;
				nB = sliceBase->macroblock[topMbIdx]->mb_chroma_4x4_non_zero_count_coeff[CbCr][leftIdx];
				availableTop = true;
			}
		}
		else
		{
			nB = sliceBase->macroblock[sliceBase->CurrMbAddr]->mb_chroma_4x4_non_zero_count_coeff[CbCr][BlkIdx - 2];
			availableTop = true;
		}

	}
	return 0;
}
