#pragma once
#include "Common.h"
#include "BitStream.h"


class ParseSlice;
class Cabac
{
private:
	//当前的状态在表格的位置
	int stateIdx[1024];

	//LPS的概率取值在0-0.5之间被划分64个值

	//0和1哪个出现的概率比较小 假设0出现的概率只有30%，1出现的有70%，在这个状态下LPS就是0，MPS就是1
	//（LPS和MPS并不是恒定的有可能互换，通过一个预定义的表格来查询和转移）
	//当前的状态下这个大概率模型所取的值（LPS是MPS的相反）
	int MPSValue[1024];


	//为了提高编码效果，实时对上下文模型进行更新
	int codIRange;
	int codIOffset;
public:
	Cabac();
	~Cabac();
	void Initialisation_process_for_context_variables(SLIECETYPE slice_type, int SliceQPY, int cabac_init_idc);


	void Initialisation_process_for_the_arithmetic_decoding_engine(BitStream& bs);

	void getMN(const int ctxIdx, const SLIECETYPE slice_type, const int cabac_init_idc, int m, int n);


	int decode_mb_skip_flag(BitStream& bs, ParseSlice* Slice, int _CurrMbAddr);
	int Derivation_process_of_ctxIdxInc_for_the_syntax_element_mb_skip_flag(ParseSlice* Slice, int _CurrMbAddr);
};

