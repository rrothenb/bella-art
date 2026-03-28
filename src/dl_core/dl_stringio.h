// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_STRINGIO_H
#define DL_CORE_STRINGIO_H

#include "dl_string.h"
#include "dl_vector.h"
#include <string>

namespace dl {

// Atypical of types in this system, StringIO is a plain class that should not be passed around
// between libraries. Its purpose is to be a faster alternative to String, when you know you will
// be doing lots of appending, or when you know you are just using tiny strings on the stack (it
// has two buffers, one of which is a fixed 32-byte buffer on the stack). It is a char-based string
// so it can hold all the same utf8 data that String does, but it lacks awareness of it, and you
// need to convert to String if you need to iterate by codepoint or get other accurate info.
//
class DL_API StringIO
{
public:
    StringIO();
    StringIO( const StringIO& io );
    StringIO( const char* cstr, Size maxLen = 0 );
    StringIO( char val, UInt64 count );
    StringIO( String str );

    // Explicit conversions.
    //
    DL_EXPLICIT StringIO( char val );
    DL_EXPLICIT StringIO( Bool val );
    DL_EXPLICIT StringIO( Int16 val );
    DL_EXPLICIT StringIO( Int32 val );
    DL_EXPLICIT StringIO( Int64 val );
    DL_EXPLICIT StringIO( Byte val );
    DL_EXPLICIT StringIO( UInt16 val );
    DL_EXPLICIT StringIO( UInt32 val );
    DL_EXPLICIT StringIO( UInt64 val );
    DL_EXPLICIT StringIO( Float val );
    DL_EXPLICIT StringIO( Double val );
    DL_EXPLICIT StringIO( BufRef val, Bool compress = true );

    size_t          len() const;
    const char*     buf() const;
    String          str() const { return String( buf() ); }

    void            clear();
    Bool            isEmpty() const;

    Bool            startsWith( char c ) const;
    Bool            endsWith( char c ) const;

    static int      compare( const char* l, const char* r, bool ignoreCase = false );
    static int      compareNoCase( const char* l, const char* r );

    int             compare( const StringIO& io, bool ignoreCase = false ) const;
    int             compareNoCase( const StringIO& io ) const;
    
    // Operators.
    //
    char            operator[]( Size idx ) const;

    Bool            operator<( const StringIO& io ) const;
    Bool            operator>( const StringIO& io ) const;

    Bool            operator==( const StringIO& io ) const;
    Bool            operator==( const char* cstr ) const;
    Bool            operator==( char c ) const;

    Bool            operator!=( const StringIO& io ) const;
    Bool            operator!=( const char* cstr ) const;
    Bool            operator!=( char c ) const;

    StringIO        operator+( const StringIO& io ) const;
    StringIO        operator+( String str ) const;
    StringIO        operator+( const char* cstr ) const;
    StringIO        operator+( char c ) const;

    StringIO&       operator=( const StringIO& io );
    StringIO&       operator=( String str );
    StringIO&       operator=( const char* cstr );
    StringIO&       operator=( char c );

    StringIO&       operator+=( const StringIO& io );
    StringIO&       operator+=( String str );
    StringIO&       operator+=( const char* cstr );
    StringIO&       operator+=( char c );

    // Write values to StringIO.
    //
    StringIO&       write( const StringIO& io, bool newline = false );
    StringIO&       writeLn( const StringIO& io ) { return write( io, true ); }

    template<typename T> StringIO& write( T val, bool newline = false ) { return write( StringIO( val ), newline ); }
    template<typename T> StringIO& writeLn( T val ) { return write( StringIO( val ), true ); }

    StringIO&       operator<<( String val );
    StringIO&       operator<<( const char* val );
    StringIO&       operator<<( char   val );
    StringIO&       operator<<( Bool   val );
    StringIO&       operator<<( Char   val );
    StringIO&       operator<<( Int16  val );
    StringIO&       operator<<( Int32  val );
    StringIO&       operator<<( Int64  val );
    StringIO&       operator<<( Byte   val );
    StringIO&       operator<<( UInt16 val );
    StringIO&       operator<<( UInt32 val );
    StringIO&       operator<<( UInt64 val );
    StringIO&       operator<<( Float  val );
    StringIO&       operator<<( Double val );
    StringIO&       operator<<( BufRef val );

    // Print to StringIO with formatting.
    //
    static StringIO format( const char* fmt, ... );
    static StringIO formatv( const char* fmt, va_list ap );

    StringIO&       print( const char* fmt, ... );
    StringIO&       printLn( const char* fmt = "", ... );

    // Escape and unescape double quotes and backslashes throughout the string.
    //
    StringIO&       escape();
    StringIO&       unescape();

    // These are really only good for backing up in the string. pos currently just returns len and
    // seek will either return the offset given, if it is >= len, or else it will truncate to the
    // given offset (meaning, length will be set to offset). The purpose for their existence is to
    // be used in template methods that take either a File or a StringIO. For instance in the bsa
    // writer, there is a case where we need to back up 2 chars and write something different, and
    // that looks like:
    // 
    //   io.printLn( "%s %s:", node.type().buf(), node.name().buf() );
    //   auto pos = io.pos();
    // 
    //   // Here a bunch of stuff is maybe written to io.
    // 
    //   if ( io.pos() == pos ) // But if not then we need to replace that colon with a semicolon.
    //   {
    //       io.seek( pos-2 );
    //       io.write( ';' );
    //   }
    // 
    // The point being, this same code works whether io is a File or a StringIO. and indeed the bsa
    // writer is implemented in terms of a template "IO" in order to use the same code when writing
    // files, as when serializing a single node to StrignIO in memory.
    //
    UInt64          pos() const;
    UInt64          seek( UInt64 offset );

private:
    StringIO&       assign( const char* cstr, Size maxLen = 0 );

    Size            _len;
    char            _buf[32];
    ds::CharVector  _vec;
};

// StringIO on right hand side.
//
static inline Bool operator==( String l, const StringIO& r )         { return StringIO( l ) == r; }
static inline Bool operator==( const char* l, const StringIO& r )    { return StringIO( l ) == r; }
static inline Bool operator==( char l, const StringIO& r )           { return StringIO( l ) == r; }

static inline Bool operator!=( String l, const StringIO& r )         { return StringIO( l ) != r; }
static inline Bool operator!=( const char* l, const StringIO& r )    { return StringIO( l ) != r; }
static inline Bool operator!=( char l, const StringIO& r )           { return StringIO( l ) != r; }

static inline StringIO operator+( String l, const StringIO& r )      { return StringIO( l ) + r; }
static inline StringIO operator+( const char* l, const StringIO& r ) { return StringIO( l ) + r; }
static inline StringIO operator+( char l, const StringIO& r )        { return StringIO( l ) + r; }

// std stream insertion/extraction.
// 
template <typename Stream> static inline Stream& operator<<( Stream& stream, const StringIO& str )
{
    stream << std::string( str.buf() );
    return stream;
}

template <typename Stream> static inline Stream& operator>>( Stream& stream, StringIO& str )
{
    std::string sstr;
    stream >> sstr;
    str = sstr.c_str();
    return stream;
}

} // dl

#endif // DL_CORE_STRINGIO_H
