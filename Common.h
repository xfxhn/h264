#pragma once
#include <cstdint>
#include <iostream>
#include "BitStream.h"
#include <cassert>
#include <vector>
#include "NaluType.h" 
using namespace std;



inline void printError(const char* err) {
    cout << err << endl;
}

string printHex(unsigned char* data,int len);



int h264_log2(int32_t value);


int scaling_list(BitStream& bs, int32_t* scalingList, int sizeOfScalingList, int32_t& useDefaultScalingMatrixFlag);

//Моід
bool byte_aligned(BitStream& bs);

bool more_rbsp_data(BitStream& bs);

