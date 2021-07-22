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




	int decode_mb_skip_flag(BitStream& bs, ParseSlice* Slice);
	int decode_mb_type(BitStream& bs, ParseSlice* Slice);
	int decode_transform_size_8x8_flag(BitStream& bs, ParseSlice* Slice);
	int decode_coded_block_pattern(BitStream& bs, ParseSlice* Slice);
	int decode_mb_qp_delta(BitStream& bs, ParseSlice* Slice);
	int decode_prev_intra4x4_pred_mode_flag_or_prev_intra8x8_pred_mode_flag(BitStream& bs);
	int decode_rem_intra4x4_pred_mode_or_rem_intra8x8_pred_mode(BitStream& bs);
	int decode_intra_chroma_pred_mode(BitStream& bs, ParseSlice* Slice);
	int residual_block_cabac(BitStream& bs, ParseSlice* Slice, int* coeffLevel, int startIdx, int endIdx, uint32_t maxNumCoeff, RESIDUAL_LEVEL residualLevel, int& TotalCoeff, int BlkIdx, int iCbCr = -1);

private:
	int Derivation_process_of_ctxIdxInc_for_the_syntax_element_mb_skip_flag(ParseSlice* Slice);
	int Derivation_process_of_ctxIdxInc_for_the_syntax_element_mb_type(ParseSlice* Slice, int ctxIdxOffset);
	int Derivation_process_of_ctxIdxInc_for_the_syntax_element_transform_size_8x8_flag(ParseSlice* Slice);
	int Derivation_process_of_ctxIdxInc_for_the_syntax_element_coded_block_pattern(ParseSlice* Slice, int ctxIdxOffset, int binIdx, int binVal);
	int Derivation_process_of_ctxIdxInc_for_the_syntax_element_mb_qp_delta(ParseSlice* Slice);
	int Derivation_process_of_ctxIdxInc_for_the_syntax_element_intra_chroma_pred_mode(ParseSlice* Slice);
	int Derivation_process_of_ctxIdxInc_for_the_syntax_element_coded_block_flag(ParseSlice* Slice, int ctxBlockCat, int BlkIdx, int iCbCr);

	int decode_mb_type_in_I_slices(ParseSlice* Slice, BitStream& bs, int ctxIdxOffset);
	int decode_mb_type_in_SI_slices(ParseSlice* Slice, BitStream& bs, int ctxIdxOffset);
	int decode_mb_type_in_P_SP_slices(ParseSlice* Slice, BitStream& bs, int ctxIdxOffset);
	int decode_mb_type_in_B_slices(ParseSlice* Slice, BitStream& bs, int ctxIdxOffset);


	bool decode_coded_block_flag(ParseSlice* Slice, BitStream& bs, RESIDUAL_LEVEL residualLevel, int BlkIdx, int iCbCr);
	bool decode_significant_coeff_flag_and_last_significant_coeff_flag(ParseSlice* Slice, BitStream& bs, RESIDUAL_LEVEL residualLevel, int levelListIdx, bool lastFlag);
	int decode_coeff_abs_level_minus1(ParseSlice* Slice, BitStream& bs, RESIDUAL_LEVEL residualLevel, int numDecodAbsLevelEq1, int numDecodAbsLevelGt1);
	int decode_coeff_sign_flag(ParseSlice* Slice, BitStream& bs);

	void getMN(const int ctxIdx, const SLIECETYPE slice_type, const int cabac_init_idc, int m, int n);
	int DecodeBin(BitStream& bs, int bypassFlag, int ctxIdx);
	int DecodeDecision(BitStream& bs, int ctxIdx);
	int DecodeBypass(BitStream& bs);
	int DecodeTerminate(BitStream& bs);
	void RenormD(BitStream& bs);


	int getCtxBlockCat(int NumC8x8, RESIDUAL_LEVEL residualLevel);
};


