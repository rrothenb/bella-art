// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_COMPRESS_H
#define DL_CORE_COMPRESS_H

#include "dl_vector.h"

namespace dl {

// Helpers for encoding/decoding hex & compressing buffers. Currently only supporting zlib.
//
enum Compression : Byte
{
    Compression_None = 0,
    Compression_Zlib = 1,
    Compression_Png  = 2, // Not supported yet.
    Compression_Jpeg = 3, // Not supported yet.

    Compression_Last
};

// Encode & decode hex buffers.
//
DL_API ds::Buffer   encodeHex( const Byte* buf, Size len );
DL_API ds::Buffer   encodeHex( const ds::Buffer& buf );

DL_API ds::Buffer   decodeHex( const Byte* buf, Size len );
DL_API ds::Buffer   decodeHex( const ds::Buffer& buf );

// Compress a buffer, optionally encoding as hex. Any compression but zlib will yield an error.
//
DL_API ds::Buffer   compress( const Byte* in, Size len, bool hexEncode = false, Compression compression = Compression_Zlib );
DL_API ds::Buffer   compress( const ds::Buffer& in, bool hexEncode = false, Compression compression = Compression_Zlib );

// Decompress a buffer, specifying whether it is hex encoded or not. If you know the decompressed
// length, you should provide it, otherwise there is some trial and error involved.
//
DL_API ds::Buffer   decompress( const Byte* in, Size len, bool hexDecode = false, Size decompressedLen = 0 );
DL_API ds::Buffer   decompress( const ds::Buffer& in, bool hexDecode = false, Size decompressedLen = 0 );

// Decompress known hex encoded buffers. If expectedLen is given then it will be an error if the
// buffer does not start with metadata matching that length. It will also be an error if the actual
// decompressed buffer length does not match the length recorded in the buffer metadata.
//
DL_API ds::Buffer   decompress( const Byte* hexEncoded, Size len, String& status, Size expectedLen = 0 );
DL_API ds::Buffer   decompress( String hexEncoded, String& status, Size expectedLen = 0 );

} // dl

#endif // DL_CORE_COMPRESS_H
