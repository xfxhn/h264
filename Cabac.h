#pragma once
#include "Common.h"
class Cabac
{
public:
	Cabac();
	~Cabac();
	void Initialisation_process_for_context_variables(SLIECETYPE slice_type, int SliceQPY, int cabac_init_idc);
	void getMN(const int ctxIdx, const SLIECETYPE slice_type, const int cabac_init_idc, int m, int n);
};

