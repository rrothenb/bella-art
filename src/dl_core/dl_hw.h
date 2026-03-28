// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_HW_H
#define DL_CORE_HW_H

#include "dl_string.h"

// Get info about the current hardware.
//
namespace dl {
namespace hw {

DL_API String       vendorString();
DL_API String       osInfo();
DL_API String       hwInfo();

DL_API bool         canUseAVX();
DL_API bool         canUseAVX512();

DL_API bool         isAMD();
DL_API bool         isIntel();

// OS Features
DL_API bool         osX64();
DL_API bool         osAVX();
DL_API bool         osAVX512();

// Misc.
DL_API bool         hwMMX();
DL_API bool         hwIsX64();
DL_API bool         hwABM();
DL_API bool         hwRDRAND();
DL_API bool         hwBMI1();
DL_API bool         hwBMI2();
DL_API bool         hwADX();
DL_API bool         hwPREFETCHWT1();
DL_API bool         hwMPX();

// SIMD: 128-bit
DL_API bool         hwSSE();
DL_API bool         hwSSE2();
DL_API bool         hwSSE3();
DL_API bool         hwSSSE3();
DL_API bool         hwSSE41();
DL_API bool         hwSSE42();
DL_API bool         hwSSE4a();
DL_API bool         hwAES();
DL_API bool         hwSHA();

// SIMD: 256-bit
DL_API bool         hwAVX();
DL_API bool         hwXOP();
DL_API bool         hwFMA3();
DL_API bool         hwFMA4();
DL_API bool         hwAVX2();

// SIMD: 512-bit
DL_API bool         hwAVX512_F();
DL_API bool         hwAVX512_PF();
DL_API bool         hwAVX512_ER();
DL_API bool         hwAVX512_CD();
DL_API bool         hwAVX512_VL();
DL_API bool         hwAVX512_BW();
DL_API bool         hwAVX512_DQ();
DL_API bool         hwAVX512_IFMA();
DL_API bool         hwAVX512_VBMI();

} // hw
} // dl

#endif // DL_CORE_HW_H

