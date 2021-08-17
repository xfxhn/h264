#pragma once
#include "Common.h"
#include "ResidualBlockCavlc.h"
#include "Cabac.h"


class SliceData;
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

struct SUB_MB_TYPE_MBS_P
{
	uint8_t					sub_mb_type;
	H264_MB_TYPE			name;
	uint8_t					NumSubMbPart;
	H264_MB_PART_PRED_MODE  SubMbPredMode;
	uint8_t                 SubMbPartWidth;
	uint8_t                 SubMbPartHeight;
};


struct SUB_MB_TYPE_MBS_B
{
	uint8_t					sub_mb_type;
	H264_MB_TYPE			name;
	uint8_t					NumSubMbPart;
	H264_MB_PART_PRED_MODE  SubMbPredMode;
	uint8_t                 SubMbPartWidth;
	uint8_t                 SubMbPartHeight;
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
	H264_MB_TYPE    subMbType[4];




	uint8_t			MbPartWidth;
	uint8_t			MbPartHeight;
	uint8_t			SubMbPartWidth[4];
	uint8_t			SubMbPartHeight[4];

	uint32_t		pcm_alignment_zero_bit; // 3 f(1)
	uint32_t		pcm_sample_luma[256]; //3 u(v)
	uint32_t* pcm_sample_chroma;
	bool			transform_size_8x8_flag;
	uint8_t			mb_type;


	uint8_t     ref_idx_l0[4];
	uint8_t     ref_idx_l1[4];
	uint8_t     mvd_l0[4][4][2];
	uint8_t     mvd_l1[4][4][2];



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
	uint8_t		 fix_mb_type;

	bool TransformBypassModeFlag;


	uint8_t Intra4x4PredMode[16];//�洢��ǰ����16��4x4�Ӻ��Ԥ��ģʽ��ֵ����Χ[0,8]
	uint8_t Intra8x8PredMode[4];//�洢��ǰ����4��8x8�Ӻ��Ԥ��ģʽ��ֵ����Χ[0,8]
	uint8_t Intra16x16PredMode;//�洢��ǰ����1��16x16���Ԥ��ģʽ��ֵ����Χ[0,4]

	uint8_t lumaPredSamples[16][4][4]; //�洢16*16���ȿ�Ԥ������ֵ
	uint8_t luma8x8PredSamples[4][8][8]; //�洢8*8���ȿ�Ԥ������ֵ
	uint8_t luma16x16PredSamples[16][16]; //�洢16*16���ȿ�Ԥ������ֵ
	uint8_t chromaPredSamples[8][16]; //�洢ɫ�ȿ�Ԥ������ֵ  //���422 8*16

	//uint8_t predL[16][16];  //�洢����Ԥ������ֵ



	uint32_t sliceNumber;

	int8_t     FilterOffsetA;
	int8_t     FilterOffsetB;


	int  mb_skip_flag;

	uint8_t     coded_block_flag_DC_pattern; //3��bitλ��ʾCABAC�в���Ӧ4x4�Ӻ���е�DCֱ��ϵ��block��coded_block_flagֵ��(b7,...,b2,b1,b0)=(x,...,cr,cb,luma)
	uint16_t    coded_block_flag_AC_pattern[3]; //16��bitλ��ʾCABAC�в���Ӧ4x4�Ӻ���е�AC����ϵ��block��coded_block_flagֵ(0��1)(ȫ��Ĭ��Ϊ1)��[0]-luma,[1]-cb,[2]-cr

public:
	Macroblock();
	bool macroblock_layer(BitStream& bs, ParseSlice* Slice, SliceData* slice_data, Cabac& cabac);



	static std::pair<int, int> getMbPartWidthAndHeight(H264_MB_TYPE mbType);
	static H264_MB_PART_PRED_MODE getSubMbPredMode(H264_MB_TYPE subMbType);
	static H264_MB_PART_PRED_MODE getMbPartPredMode(SLIECETYPE fix_slice_type, uint8_t fix_mb_type, bool transform_size_8x8_flag, uint8_t mbPartIdx);
	~Macroblock();
private:

	H264_MB_PART_PRED_MODE MbPartPredMode(uint8_t mbPartIdx);

	int fixed_mb_type(uint32_t slice_type, uint8_t& fix_mb_type, SLIECETYPE& fix_slice_type);

	bool mb_pred(BitStream& bs, uint32_t numMbPart, ParseSlice* Slice, Cabac& cabac);

	bool residual(BitStream& bs, int startIdx, int endIdx, Cabac& cabac);
	bool sub_mb_pred(uint32_t mb_type);
	uint8_t NumMbPart(uint32_t mb_type, SLIECETYPE slice_type);


	int residual_luma(BitStream& bs, int i16x16DClevel[16], int i16x16AClevel[16][16], int level4x4[16][16],
		int level8x8[4][64], int startIdx, int endIdx, Cabac& cabac);

	//void getChromaQuantisationParameters();
private:
	bool isAe;


	ParseSlice* sliceBase;
};

