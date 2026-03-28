// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_FILE_H
#define DL_CORE_FILE_H

#include "dl_defines.h"
#include "dl_string.h"
#include "dl_stringio.h"
#include "dl_references.h"
#include "dl_types.h"
#include "dl_compress.h"

namespace dl {

// File is a refcounted, buffered file class for reading & writing DL types to text & binary files.
// It suppors per-element compression for writing buffers and blind data.
//
class DL_API File : public RefersTo<RefCounted>
{
public:

    enum Mode
    {
        Mode_Read,
        Mode_Write,
        Mode_Append
    };

    enum Format
    {
        Format_Text,
        Format_Binary
    };

    File();

    Bool            open( String path, Mode mode, Format format );
    Bool            close();
    Bool            flush();

    UInt64          len() const;
    UInt64          pos() const;
    Bool            eof() const;
    UInt64          seek( Int64 offset );
    UInt64          skip( Int64 offset );
    Byte            peek();

    Mode            mode() const;
    Format          format() const;

    Bool            ok() const;
    String          error() const;

    // Convenience.
    //
    Bool            openReadText( String path ) { return open( path, Mode_Read, Format_Text ); }
    Bool            openWriteText( String path ) { return open( path, Mode_Write, Format_Text ); }
    Bool            openAppendText( String path ) { return open( path, Mode_Append, Format_Text ); }

    Bool            openReadBinary( String path ) { return open( path, Mode_Read, Format_Binary ); }
    Bool            openWriteBinary( String path ) { return open( path, Mode_Write, Format_Binary ); }
    Bool            openAppendBinary( String path ) { return open( path, Mode_Append, Format_Binary ); }

    // NOTE: for numerics, use %s with String::format(number).buf(), unless you specifically want to
    // use the underlying runtime's formatting behavior.
    //
    Bool            print( const char* fmt, ... );
    Bool            printLn( const char* fmt = "", ... );

    // Write elements.
    //
    Bool            write( String val );
    Bool            write( const StringIO& val );
    Bool            write( const char* val );
    Bool            write( char   val );
    Bool            write( Bool   val );
    Bool            write( Char   val );
    Bool            write( Byte   val );
    Bool            write( Int16  val );
    Bool            write( Int32  val );
    Bool            write( Int64  val );
    Bool            write( UInt16 val );
    Bool            write( UInt32 val );
    Bool            write( UInt64 val );
    Bool            write( Float  val );
    Bool            write( Double val );
    Bool            write( BufRef val, Bool compress = true );
    Bool            write( Vec2   val );
    Bool            write( Vec3   val );
    Bool            write( Vec4   val );
    Bool            write( Pos2   val );
    Bool            write( Pos3   val );
    Bool            write( Quat   val );
    Bool            write( Rgba   val );
    Bool            write( Mat3   val );
    Bool            write( Mat4   val );

    File&           operator<<( String val );
    File&           operator<<( const StringIO& val );
    File&           operator<<( const char* val );
    File&           operator<<( char   val );
    File&           operator<<( Bool   val );
    File&           operator<<( Char   val );
    File&           operator<<( Byte   val );
    File&           operator<<( Int16  val );
    File&           operator<<( Int32  val );
    File&           operator<<( Int64  val );
    File&           operator<<( UInt16 val );
    File&           operator<<( UInt32 val );
    File&           operator<<( UInt64 val );
    File&           operator<<( Float  val );
    File&           operator<<( Double val );
    File&           operator<<( BufRef val );
    File&           operator<<( Vec2   val );
    File&           operator<<( Vec3   val );
    File&           operator<<( Vec4   val );
    File&           operator<<( Pos2   val );
    File&           operator<<( Pos3   val );
    File&           operator<<( Quat   val );
    File&           operator<<( Rgba   val );
    File&           operator<<( Mat3   val );
    File&           operator<<( Mat4   val );

    // Write a blind buffer (NOTE: use BufRef if endian conversion is required).
    //
    Bool            write( const Ptr data, UInt64 size, Compression compression = Compression_Zlib );
    Bool            write( const ds::Buffer& buf, Compression compression = Compression_Zlib );

    // Write using the wrong data type.
    //
    template<typename T> Bool writeBool( T   val ) { return write( DL_CAST( Bool,   val ) ); }
    template<typename T> Bool writeChar( T   val ) { return write( DL_CAST( Char,   val ) ); }
    template<typename T> Bool writeByte( T   val ) { return write( DL_CAST( Byte,   val ) ); }
    template<typename T> Bool writeInt16( T  val ) { return write( DL_CAST( Int16,  val ) ); }
    template<typename T> Bool writeInt32( T  val ) { return write( DL_CAST( Int32,  val ) ); }
    template<typename T> Bool writeInt64( T  val ) { return write( DL_CAST( Int64,  val ) ); }
    template<typename T> Bool writeUInt16( T val ) { return write( DL_CAST( UInt16, val ) ); }
    template<typename T> Bool writeUInt32( T val ) { return write( DL_CAST( UInt32, val ) ); }
    template<typename T> Bool writeUInt64( T val ) { return write( DL_CAST( UInt64, val ) ); }
    template<typename T> Bool writeFloat( T  val ) { return write( DL_CAST( Float,  val ) ); }
    template<typename T> Bool writeDouble( T val ) { return write( DL_CAST( Double, val ) ); }

    // Read all text (not supported with Format_Binary).
    //
    String          read();
    StringVector    readLines() { return read().split( '\n' ); }

    // Read elements (not supported with Format_Text).
    //
    Bool            read( String& val );
    Bool            read( StringIO& val );
    Bool            read( char&   val );
    Bool            read( Bool&   val );
    Bool            read( Char&   val );
    Bool            read( Byte&   val );
    Bool            read( Int16&  val );
    Bool            read( Int32&  val );
    Bool            read( Int64&  val );
    Bool            read( UInt16& val );
    Bool            read( UInt32& val );
    Bool            read( UInt64& val );
    Bool            read( Float&  val );
    Bool            read( Double& val );
    Bool            read( BufRef& val );
    Bool            read( Vec2&   val );
    Bool            read( Vec3&   val );
    Bool            read( Vec4&   val );
    Bool            read( Pos2&   val );
    Bool            read( Pos3&   val );
    Bool            read( Quat&   val );
    Bool            read( Rgba&   val );
    Bool            read( Mat3&   val );
    Bool            read( Mat4&   val );

    File&           operator>>( String& val );
    File&           operator>>( StringIO& val );
    File&           operator>>( char&   val );
    File&           operator>>( Bool&   val );
    File&           operator>>( Char&   val );
    File&           operator>>( Byte&   val );
    File&           operator>>( Int16&  val );
    File&           operator>>( Int32&  val );
    File&           operator>>( Int64&  val );
    File&           operator>>( UInt16& val );
    File&           operator>>( UInt32& val );
    File&           operator>>( UInt64& val );
    File&           operator>>( Float&  val );
    File&           operator>>( Double& val );
    File&           operator>>( BufRef& val );
    File&           operator>>( Vec2&   val );
    File&           operator>>( Vec3&   val );
    File&           operator>>( Vec4&   val );
    File&           operator>>( Pos2&   val );
    File&           operator>>( Pos3&   val );
    File&           operator>>( Quat&   val );
    File&           operator>>( Rgba&   val );
    File&           operator>>( Mat3&   val );
    File&           operator>>( Mat4&   val );

    // Read a blind buffer (NOTE: use BufRef if endian conversion is required).
    //
    Bool            read( ds::Buffer& data, Compression compression = Compression_Zlib );

    // Read using the wrong data type.
    //
    template<typename T> Bool readBool( T&   val ) { Bool   tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }
    template<typename T> Bool readChar( T&   val ) { Char   tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }
    template<typename T> Bool readByte( T&   val ) { Byte   tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }
    template<typename T> Bool readInt16( T&  val ) { Int16  tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }
    template<typename T> Bool readInt32( T&  val ) { Int32  tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }
    template<typename T> Bool readInt64( T&  val ) { Int64  tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }
    template<typename T> Bool readUInt16( T& val ) { UInt16 tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }
    template<typename T> Bool readUInt32( T& val ) { UInt32 tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }
    template<typename T> Bool readUInt64( T& val ) { UInt64 tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }
    template<typename T> Bool readFloat( T&  val ) { Float  tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }
    template<typename T> Bool readDouble( T& val ) { Double tmp; Bool ok = read( tmp ); val = ok ? DL_CAST( T, tmp ) : ( T )0; return ok; }

    // Utility for working directly with raw data originally written with File. Note that you must
    // ensure that the buffer is at least 21 bytes long, or else this function will commit access
    // violations (since 21 bytes of metadata are written at the beginning of such blocks).
    //
    static Bool decodeRawFileBlock( const Byte* buffer,
                                    Compression& compressionType,
                                    UInt64& compressedLen,
                                    UInt64& decompressedLen,
                                    Byte*& compressedData );

    // Similar to decodeRawFileBlock, but for use when it is known that you want to read the entire
    // buffer as a single compressed data block. On successful exit, compressedLen == buffer.size()
    // and compressedData points to the beginning of compressed data within the supplied buffer.
    //
    static Bool decodeRawFileData( const ds::Buffer& buffer,
                                   Compression& compressionType,
                                   UInt64& compressedLen,
                                   UInt64& decompressedLen,
                                   Byte*& compressedData );
};

} // dl

#endif // DL_CORE_FILE_H
