#include "ParseSlice.h"








ParseSlice::ParseSlice( uint32_t naluType)
{

	memset(this, 0, sizeof(ParseSlice));
	/*this->pps = pps;
	this->sps = sps;*/
	this->naluType = naluType;
}

ParsePPS* ParseSlice::getpps(vector<ParsePPS*> pps)
{
	return nullptr;
}

ParseSPS* ParseSlice::getsps(vector<ParseSPS*> sps)
{
	return nullptr;
}


bool ParseSlice::slice_header(BitStream& bs, vector<ParsePPS*> pps, vector<ParseSPS*> sps)
{



	//������Ա�ʾ��������� Slice �е�һ���������
	first_mb_in_slice = bs.readUE(); //2 ue(v)
	//slice������
	slice_type = bs.readUE(); //2 ue(v)
	//������pps id
	pic_parameter_set_id = bs.readUE(); //2 ue(v)


	return false;
}

ParseSlice::~ParseSlice()
{
}
