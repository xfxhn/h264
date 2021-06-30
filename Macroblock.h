#pragma once
#include "Common.h"
#include "ResidualBlockCavlc.h"



struct MB_TYPE_SLICES_I
{
	uint8_t					mb_type;
	H264_MB_TYPE			name;
	bool					transform_size_8x8_flag;
	H264_MB_PART_PRED_MODE  MbPartPredMode;
	uint8_t					Intra16x16PredMode;
	int32_t					CodedBlockPatternChroma;
	int32_t					CodedBlockPatternLuma;
};

struct MB_TYPE_SLICES_SP_P
{
	uint8_t					mb_type;
	H264_MB_TYPE			name;
	uint8_t					NumMbPart;
	H264_MB_PART_PRED_MODE  MbPartPredMode0;
	H264_MB_PART_PRED_MODE	MbPartPredMode1;
	uint8_t					MbPartWidth;
	uint8_t					MbPartHeight;
};

struct MB_TYPE_SLICES_B
{
	uint8_t					mb_type;
	H264_MB_TYPE			name;
	uint8_t					NumMbPart;
	H264_MB_PART_PRED_MODE  MbPartPredMode0;
	H264_MB_PART_PRED_MODE	MbPartPredMode1;
	uint8_t					MbPartWidth;
	uint8_t					MbPartHeight;
};

struct MB_TYPE_SLICES_SI
{
	uint8_t					mb_type;
	H264_MB_TYPE			name;
	H264_MB_PART_PRED_MODE  MbPartPredMode;
	uint8_t                 Intra16x16PredMode;
	uint8_t                 CodedBlockPatternChroma;
	uint8_t                 CodedBlockPatternLuma;
};




//�������mb_type
//������ͱ�ʾ���Ǻ�鲻ͬ�ķָ�ͱ��뷽ʽ����h.264���﷨�ṹ�У�
//��������ں��㣨macroblock_layer������mb_type��ʾ����ο�h.264�﷨�ṹ�����е�macroblock_layer����
//��mb_type���뵱ǰ������ڵ�slice��صģ���ͬ��ֵ��mb_type�ڲ�ͬ���͵�slice�б�ʾ�����ͻ᲻һ����
class ParseSlice;
class Macroblock
{
public:

	H264_MB_TYPE    mbType;
	uint32_t		pcm_alignment_zero_bit; // 3 f(1)
	uint32_t		pcm_sample_luma[256]; //3 u(v)
	uint32_t* pcm_sample_chroma;
	bool			transform_size_8x8_flag;
	uint32_t		mb_type;




	uint32_t		coded_block_pattern;
	uint32_t		CodedBlockPatternLuma;
	uint32_t		CodedBlockPatternChroma;

	uint32_t		mb_qp_delta;

	int             QPY;
	int				QP1Y;

	int QP1C;
	int QPC;


	int QSY;
	int QSC;



	/*int QPCb;
	int QP1Cb;
	int QPCr;
	int QP1Cr;*/

	uint32_t		prev_intra4x4_pred_mode_flag[16];
	uint32_t		rem_intra4x4_pred_mode[16];

	uint32_t		prev_intra8x8_pred_mode_flag[4];
	uint32_t		rem_intra8x8_pred_mode[4];

	uint32_t		intra_chroma_pred_mode;



	int     i16x16DClevel[16]; //DCT�任���ֱ��ϵ��
	int     i16x16AClevel[16][16]; //DCT�任��Ľ���ϵ��

	int     level4x4[16][16];     //�洢���ȵĲв�����
	int     level8x8[4][64];



	int     ChromaDCLevel[2][16]; //�洢DCɫ��u��v�Ĳв�����
	int     ChromaACLevel[2][16][15]; //�洢ACɫ��u��v�Ĳв�����


	uint8_t     mb_luma_4x4_non_zero_count_coeff[16];//�洢4x4���Ⱥ���0ϵ��
	uint8_t     mb_luma_8x8_non_zero_count_coeff[4];//�洢8x8���Ⱥ���0ϵ��
	uint8_t     mb_chroma_4x4_non_zero_count_coeff[2][16];//�洢ɫ�Ⱥ���0ϵ��


	H264_MB_PART_PRED_MODE mode;//��ǰ����Ԥ��ģʽ
	SLIECETYPE   fix_slice_type;


	bool TransformBypassModeFlag;


	uint8_t Intra4x4PredMode[16];//�洢��ǰ����16��4x4�Ӻ��Ԥ��ģʽ��ֵ����Χ[0,8]
	uint8_t Intra16x16PredMode;//�洢��ǰ����1��16x16���Ԥ��ģʽ��ֵ����Χ[0,4]

	uint8_t lumaPredSamples[16][4][4]; //�洢�ֿ�����Ԥ������ֵ
	uint8_t luma16x16PredSamples[16][16]; //�洢16*16���ȿ�Ԥ������ֵ
	uint8_t chromaPredSamples[8][16]; //�洢ɫ�ȿ�Ԥ������ֵ  //���422 8*16

	uint8_t predL[16][16];  //�洢����Ԥ������ֵ


public:
	Macroblock(ParseSlice& slice);
	bool macroblock_layer(BitStream& bs);





	~Macroblock();
private:
	H264_MB_PART_PRED_MODE MbPartPredMode(uint32_t mb_type, SLIECETYPE slice_type, uint32_t mbPartIdx);
	H264_MB_PART_PRED_MODE MbPartPredMode2(uint32_t mb_type, SLIECETYPE slice_type, uint32_t mbPartIdx);
	int fixed_mb_type(uint32_t slice_type, uint32_t& fix_mb_type, SLIECETYPE& fix_slice_type);
	bool is_I_NxN(uint32_t mb_type, SLIECETYPE slice_type);
	bool mb_pred(BitStream& bs, uint32_t mb_type, uint32_t numMbPart);
	bool residual(BitStream& bs, int startIdx, int endIdx);
	bool sub_mb_pred(uint32_t mb_type);
	uint32_t NumMbPart(uint32_t mb_type, SLIECETYPE slice_type);


	int residual_luma(BitStream& bs, int i16x16DClevel[16], int i16x16AClevel[16][16], int level4x4[16][16],
		int level8x8[4][64], int startIdx, int endIdx);

	//void getChromaQuantisationParameters();
private:
	bool isAe;


	ParseSlice& sliceBase;
};

