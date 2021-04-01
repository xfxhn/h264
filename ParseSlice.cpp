#include "ParseSlice.h"







/// <summary>
/// ,
/// </summary>
/// <param name="naluType"></param>
ParseSlice::ParseSlice( uint32_t naluType)
{

	memset(this, 0, sizeof(ParseSlice));
	/*this->ppsGather = pps;
	this->spsGather = sps;*/
	this->naluType = naluType;
}
//
//bool ParseSlice::getppsAndSps()
//{
//	for (size_t i = 0; i < ppsGather.size(); i++)
//	{
//		if (ppsGather[i]->pic_parameter_set_id == pic_parameter_set_id)
//		{
//			pps = ppsGather[i];
//			break;
//		}
//	}
//
//	for (size_t i = 0; i < spsGather.size(); i++)
//	{
//		if (spsGather[i]->seq_parameter_set_id == pps->seq_parameter_set_id)
//		{
//			sps = spsGather[i];
//			break;
//		}
//	}
//
//	return false;
//}



bool ParseSlice::slice_header(BitStream& bs, const ParsePPS ppsCache[256], const ParseSPS spsCache[32])
{


	//这个属性表示的是在这个 Slice 中第一个宏块的序号
	first_mb_in_slice = bs.readUE(); //2 ue(v)
	//slice的类型
	slice_type = bs.readUE(); //2 ue(v)
	//依赖的pps id
	pic_parameter_set_id = bs.readUE(); //2 ue(v)
	
	//bool flag = getppsAndSps();

	pps = ppsCache[pic_parameter_set_id];
	sps = spsCache[pps.seq_parameter_set_id];
	return false;
}

ParseSlice::~ParseSlice()
{
}
