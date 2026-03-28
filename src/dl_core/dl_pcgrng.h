// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_PCGRNG_H
#define DL_CORE_PCGRNG_H

#include "dl_defines.h"

namespace dl {

// Based on PCG basic C RNG from https://www.pcg-random.org.
//
// This class will not be modified, as it is provided to allow for implementation of plugins and
// nodes with need of pseudo-random behavior that remains stable over time.

class DL_API PcgRng
{
public:
    static const UInt64 DefaultSequence = 0xda3e39cb94b95bdbULL;
    static const UInt64 DefaultState    = 0x853c49e6748fea9bULL;

    // Seed RNG with sequence selection constant (a.k.a. stream id) and/or state. In general you
    // only need to provide your seed value as sequence, with the state being useful, for example,
    // to generate unique sequences tied to a given piece of hardware (e.g. setting state to the
    // cpuid or hash of the mac address, and so forth).

    PcgRng();
    PcgRng( UInt64 sequence );
    PcgRng( UInt64 sequence, UInt64 state );

    UInt32  uniformUInt();                  // Uniformly distributed random uint.
    UInt32  uniformUInt( UInt32 bound );    // Uniformly distributed uint where 0 <= N < bound.
    Double  uniformDouble();                // Uniformly distributed double in [0,1).
    Float   uniformFloat();                 // Uniformly distributed float in [0,1).

private:
    UInt64  _state;
    UInt64  _inc;
};

} // dl

#endif // DL_CORE_PCGRNG_H
