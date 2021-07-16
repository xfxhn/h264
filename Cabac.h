#pragma once
#include "Common.h"
#include "BitStream.h"


class ParseSlice;
class Cabac
{
private:
	//��ǰ��״̬�ڱ���λ��
	int stateIdx[1024];

	//LPS�ĸ���ȡֵ��0-0.5֮�䱻����64��ֵ

	//0��1�ĸ����ֵĸ��ʱȽ�С ����0���ֵĸ���ֻ��30%��1���ֵ���70%�������״̬��LPS����0��MPS����1
	//��LPS��MPS�����Ǻ㶨���п��ܻ�����ͨ��һ��Ԥ����ı������ѯ��ת�ƣ�
	//��ǰ��״̬����������ģ����ȡ��ֵ��LPS��MPS���෴��
	int MPSValue[1024];


	//Ϊ����߱���Ч����ʵʱ��������ģ�ͽ��и���
	int codIRange;
	int codIOffset;
public:
	Cabac();
	~Cabac();
	void Initialisation_process_for_context_variables(SLIECETYPE slice_type, int SliceQPY, int cabac_init_idc);


	void Initialisation_process_for_the_arithmetic_decoding_engine(BitStream& bs);




	int decode_mb_skip_flag(BitStream& bs, ParseSlice* Slice, int _CurrMbAddr);
	int decode_mb_type(BitStream& bs, ParseSlice* Slice);

private:
	int Derivation_process_of_ctxIdxInc_for_the_syntax_element_mb_skip_flag(ParseSlice* Slice, int _CurrMbAddr);
	int Derivation_process_of_ctxIdxInc_for_the_syntax_element_mb_type(ParseSlice* Slice, int ctxIdxOffset);

	int decode_mb_type_in_I_slices(ParseSlice* Slice, BitStream& bs, int ctxIdxOffset);
	int decode_mb_type_in_SI_slices(ParseSlice* Slice, BitStream& bs, int ctxIdxOffset);
	int decode_mb_type_in_P_SP_slices(ParseSlice* Slice, BitStream& bs, int ctxIdxOffset);
	int decode_mb_type_in_B_slices(ParseSlice* Slice, BitStream& bs, int ctxIdxOffset);


	void getMN(const int ctxIdx, const SLIECETYPE slice_type, const int cabac_init_idc, int m, int n);
	int DecodeBin(BitStream& bs, int bypassFlag, int ctxIdx);
	int DecodeDecision(BitStream& bs, int ctxIdx);
	int DecodeBypass(BitStream& bs);
	int DecodeTerminate(BitStream& bs);
	void RenormD(BitStream& bs);
};

