// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_HASH_H
#define DL_CORE_HASH_H

#include "dl_defines.h"
#include "dl_vector.h"

// Functions here, and their implementations, should never be changed, since we rely on these
// hashes for various things like quickly verifying that one file cannot be used with another.
//
namespace dl {

// Hash some data. Uses SpookyHash::Hash64 internally. If len = -1 we will use a strlen function
// to find the length so in this case, buf MUST point to an actual null-terminated string, or you
// will crash.
//
DL_API UInt64 hash( const char* buf, Size len = ( Size )-1 );

// Combine two hashes. This does not have to be fancy, it just has to never change.
//
static DL_FI UInt64 hashCombine( UInt64 lhs, UInt64 rhs )
{
    return lhs ^ ( rhs + 0x9e3779b9 + ( lhs << 6 ) + ( lhs >> 2 ) );
}

// Get MD5 hash for string.
//
DL_API ds::Vector<Byte> md5( String input );

// Get MD5 hash hex-encoded to string.
//
DL_API String md5Hex( String input );

} // dl

#endif // DL_CORE_HASH_H
