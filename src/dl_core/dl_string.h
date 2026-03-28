// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_STRING_H
#define DL_CORE_STRING_H

#include "dl_defines.h"
#include "dl_types.h"
#include "dl_references.h"
#include <string>

namespace dl {

class String;

// Refcounted Vector<String>. As with all the refcounted classes you should pass this by value
// to ensure it stays alive while you are using it. Note that where String is immutable, this is
// decidedly not and may have elements added/removed by a function even when passed by value, as
// you are really passing a copy of the implementation's wrapper, not of the implementation itself.
// This is also the reason for the existence of the dup() function.
//
namespace ds { template<typename T> class Vector; }
class DL_API StringVector : public RefersTo<IRefCounted>
{
public:
    StringVector();
    StringVector( std::initializer_list<String> init );
    StringVector( Size size );
    StringVector( const ds::Vector<String>& v );

    void            swap( StringVector& o );
    Size            size() const;
    Bool            empty() const;
    Size            capacity() const;
    String          operator[]( Size idx );
    String          operator[]( Size idx ) const;
    String          front();
    String          front() const;
    String          back();
    String          back() const;
    Int             find( String t, bool ignoreCase = false ) const;
    bool            contains( String t, bool ignoreCase = false ) const;
    void            clear();
    void            assign( Size size, String t );
    void            reserve( Size newCapacity );
    void            resize( Size newSize );
    void            push_back( String t );
    StringVector& operator <<( String t );
    void            pop_back();
    void            insert( Size idx, String t );
    String          extract( Size idx );
    bool            erase_at( Size idx );
    Size            erase( String t, bool ignoreCase = false );
    String          join( String delimiter ) const;
    StringVector    dup() const;
    UInt64          hash() const;
    bool            equalTo( StringVector other, bool sameOrder = true, bool ignoreCase = false ) const;
    void            sort( bool ascending = true, bool natural = false );
    StringVector    sorted( bool ascending = true, bool natural = false ) const;

    // These should generally not be used, they are for stl (e.g. std::find).
    //
    String* begin();
    const String* begin() const;
    String* end();
    const String* end() const;
};

// String is an immutable, reference-counted utf-8 string, which may be passed safely between
// libraries, due to its hidden implementation. Copies are cheap and instances should always be
// passed by value, to ensure they stay alive while you are using them. Pass-by-reference would
// only be used in the rare case of an out param, which we typically avoid in this project, to
// ease generating wrappers for other languages.
// 
// Technically, if you really wanted to shoot yourself in the foot you could mutate the string
// by directly modifying the pointer you get from buf(), but this would not update the string's
// hash value, so it wouldn't acheive anything useful anyway.
//
class DL_API String : public RefersTo<IRefCounted>
{
public:
    String();
    String( const char* utf8, Size maxLen = 0 );
    String( char val, UInt64 count );

    // Explicit conversions.
    //
    DL_EXPLICIT String( char val );
    DL_EXPLICIT String( Bool val );
    DL_EXPLICIT String( Int8 val );
    DL_EXPLICIT String( Int16 val );
    DL_EXPLICIT String( Int32 val );
    DL_EXPLICIT String( Int64 val );
    DL_EXPLICIT String( UInt8 val );
    DL_EXPLICIT String( UInt16 val );
    DL_EXPLICIT String( UInt32 val );
    DL_EXPLICIT String( UInt64 val );
    DL_EXPLICIT String( Float val );
    DL_EXPLICIT String( Double val );

    // Buffer for UTF-16 conversion. This is used internally on windows quite a bit, since we
    // are utf8 and always call the wide apis like: Win32ApiW(str.toUtf16().as<LPCWSTR>())
    //
    class DL_API Utf16 : public RefersTo<IRefCounted>
    {
    public:
        Utf16( String str );
        Utf16( UInt nElems );

        Size            len() const;
        UInt16          operator[]( Size idx ) const;
        const UInt16*   buf() const;
        UInt16*         buf();

        template<typename T> T as() const { return ( T )buf(); }
    };

    // Buffer for UTF-32 conversion. Hasn't been too useful yet, but is included for completeness.
    //
    class DL_API Utf32 : public RefersTo<IRefCounted>
    {
    public:
        Utf32( String str );
        Utf32( UInt nElems );

        Size            len() const;
        UInt32          operator[]( Size idx ) const;
        const UInt32*   buf() const;
        UInt32*         buf();

        template<typename T> T as() const { return ( T )buf(); }
    };

    // Construction from other encodings. Accept null-terminated strings (i.e. no embedded nulls).
    //
    String( const Utf16 val );
    String( const Utf32 val );

    DL_EXPLICIT String( const UInt16* utf16 );
    DL_EXPLICIT String( const UInt32* utf32 );

    // Get UTF-16/32 buffers for this String.
    //
    Utf16           toUtf16() const;
    Utf32           toUtf32() const;

    // These return the required size (regardless whether the buffer is null), including a null
    // terminator. They are mainly for convenience in writing to buffers already allocated.
    //
    Size            toUtf16( UInt16* buf ) const;
    Size            toUtf32( UInt32* buf ) const;

    // Length, and unicode length (since in utf8 one code point may use several chars).
    //
    Size            len() const;
    Size            ulen() const;

    // C string buffer. Note that there is purposefully no operator const char* and you have to
    // call buf() each time you need it (common when formatting strings).
    //
    const char*     buf() const;

    // The hash is computed at creation and never changes.
    //
    UInt64          hash() const;

    // Operators.
    //
    char            operator[]( Size idx ) const;

    Bool            operator<( String str ) const;
    Bool            operator>( String str ) const;

    Bool            operator==( String str ) const;
    Bool            operator==( const char* cstr ) const;
    Bool            operator==( char c ) const;

    Bool            operator!=( String str ) const;
    Bool            operator!=( const char* cstr ) const;
    Bool            operator!=( char c ) const;

    String          operator+( String str ) const;
    String          operator+( const char* cstr ) const;
    String          operator+( char c ) const;

    DL_EXPLICIT     operator bool() const { return !isEmpty(); }

    // Append & insert. Though these appear mutating, they do not alter the internal state, and
    // rather replace it with another. So if two pieces of code had the same string and one used
    // these methods, the other would continue to see its version of the string.
    //
    String&         operator=( const char* cstr );
    String&         operator=( char c );

    String&         operator+=( String str );
    String&         operator+=( const char* cstr );
    String&         operator+=( char c );

    String&         operator<<( String val );
    String&         operator<<( const char* val );
    String&         operator<<( char   val );
    String&         operator<<( Bool   val );
    String&         operator<<( Char   val );
    String&         operator<<( Byte   val );
    String&         operator<<( Int16  val );
    String&         operator<<( Int32  val );
    String&         operator<<( Int64  val );
    String&         operator<<( UInt16 val );
    String&         operator<<( UInt32 val );
    String&         operator<<( UInt64 val );
    String&         operator<<( Float  val );
    String&         operator<<( Double val );

    // Extraction of values.
    //
    String&         operator>>( String& val );
    String&         operator>>( char&   val );
    String&         operator>>( Bool&   val );
    String&         operator>>( Char&   val );
    String&         operator>>( Byte&   val );
    String&         operator>>( Int16&  val );
    String&         operator>>( Int32&  val );
    String&         operator>>( Int64&  val );
    String&         operator>>( UInt16& val );
    String&         operator>>( UInt32& val );
    String&         operator>>( UInt64& val );
    String&         operator>>( Float&  val );
    String&         operator>>( Double& val );

    // Static methods, most with versions operating in terms of const char* as well as String.
    //
    static Bool     isUpper( char c );
    static Bool     isUpper( UInt32 cp );
    static Bool     isLower( char c );
    static Bool     isLower( UInt32 cp );

    static char     toUpper( char c );
    static UInt32   toUpper( UInt32 cp );
    static char     toLower( char c );
    static UInt32   toLower( UInt32 cp );

    static Bool     isDigit( char c );
    static Bool     isHexDigit( char c );

    static Int32    toDigit( char c );
    static UInt8    toHexDigit( char c );

    static String   toHex( Byte b );

    static Bool     isLetter( char c );
    static Bool     isSpace( char c );
    static Bool     isWhitespace( char c );
    static Bool     isPrintable( char c );

    static Bool     isNull( char c );
    static Bool     isNull( const char* s );
    static Bool     isEmpty( const char* s );
    static Bool     isNullOrEmpty( const char* s );
    static Bool     isSimpleCID( const char* s ); // CID = C identifier.

    static UInt64   hash( const char* s );

    static Int32    compare( char l, char r, bool ignoreCase = false );
    static Int32    compare( const char* l, const char* r, bool ignoreCase = false );
    static Int32    compareNoCase( const char* l, const char* r );

    // Compare natural means to attempt to split off any trailing numeric substring and use it for
    // sorting, such that you get correct sorting of strings like "item 0", "item 2" and "item 10".
    //
    static Int32    compareNatural( const char* l, const char* r );

    // Depending on the numeric value given, return the input string with "s" appended. This is
    // only for english, and will not do anything fancy like put "es" when required. It just adds
    // an "s" unless the value is 1.
    //
    static String   pluralize( const char* s, Int8 v );
    static String   pluralize( const char* s, UInt8 v );
    static String   pluralize( const char* s, Int32 v );
    static String   pluralize( const char* s, UInt32 v );
    static String   pluralize( const char* s, Int64 v );
    static String   pluralize( const char* s, UInt64 v );
    static String   pluralize( const char* s, Float v );
    static String   pluralize( const char* s, Double v );

    // We often have need of finding whether a string is a valid C identifier, and/or altering a
    // string to be one. The toSimpleCID method will prepend an underscore if necessary (i.e. if
    // the string does not start with a letter or underscore), and then replace all all invalid
    // chars (i.e. not letters, or numbers or underscores) with underscore.
    //
    bool            isSimpleCID() const;
    String          toSimpleCID( String ifEmpty = "unnamed" ) const;

    // Instance versions of many of the static methods above.
    //
    Bool            isEmpty() const; // String is never null.

    // Has to be orElse, since this wonderful language reserves "or".
    //
    String          orElse( String ifEmpty ) const { return isEmpty() ? ifEmpty : *this; }

    Int32           compare( String str, bool ignoreCase = false ) const;
    Int32           compareNoCase( String str ) const;
    Int32           compareNatural( String str ) const;

    String          pluralize( Int8 v ) const;
    String          pluralize( UInt8 v ) const;
    String          pluralize( Int32 v ) const;
    String          pluralize( UInt32 v ) const;
    String          pluralize( Int64 v ) const;
    String          pluralize( UInt64 v ) const;
    String          pluralize( Float v ) const;
    String          pluralize( Double v ) const;

    // Trim whitespace from start & end.
    //
    String          trim( Bool start = true, Bool end = true ) const;
    String          trimStart() const;
    String          trimEnd() const;

    // Trim characters from the start & end. This doesn't remove just the exact pattern, but rather
    // any characters found in the given string.
    //
    String          trimChars( String start, String end = "" ) const;
    String          trimCharsStart( String chars ) const;
    String          trimCharsEnd( String chars ) const;

    // Convert to upper or lower case, optionally using simple ascii, or iterating by code point,
    // which can be much slower.
    //
    String          toUpper( bool useAscii = true ) const;
    String          toLower( bool useAscii = true ) const;

    // Get pretty versions of this string, mostly for use in user interfaces, to show a "display"
    // name when one has not specifically been given. Setting useAscii=false can take more time,
    // and is not currently implemented for titleCased and sentenceCased().
    //
    String          capitalized( bool useAscii = true ) const;
    String          uncapitalized( bool useAscii = true ) const;
    String          titleCased( bool useAscii = true ) const;
    String          sentenceCased( bool useAscii = true ) const;

    // Find index of the searched item (-1 if not found), beginning on the left, plus offset.
    //
    Int32           find( char c, bool ignoreCase = false, Size offset = 0 ) const;
    Int32           find( String s, bool ignoreCase = false, Size offset = 0 ) const;

    // Find index of the searched item (-1 if not found), beginning on the right, minus offset.
    //
    Int32           rfind( char c, bool ignoreCase = false, Size offset = 0 ) const;
    Int32           rfind( String s, bool ignoreCase = false, Size offset = 0 ) const;

    Bool            contains( char c, bool ignoreCase = false ) const;
    Bool            contains( String s, bool ignoreCase = false ) const;

    Bool            startsWith( String s, bool ignoreCase = false ) const;
    Bool            endsWith( String s, bool ignoreCase = false ) const;

    // Generally useAscii should be left false, since you never know when your string may have utf8
    // characters that cannot simply be reversed. With useAscii=false, we iterate the string by its
    // unicode code points.
    //
    String          reverse( bool useAscii = false ) const;
    String          substring( Size start, Size len = ( Size )-1 ) const;

    String          left( Size count ) const;
    String          right( Size count ) const;

    String          toFirst( char c, bool inclusive = true ) const;
    String          toLast( char c, bool inclusive = true ) const;

    String          fromFirst( char c, bool inclusive = true ) const;
    String          fromLast( char c, bool inclusive = true ) const;

    // Deep in replace & remove means to check again after replacement, until the string is no
    // longer found.
    //
    String          replace( String s, String replacement, bool deep = false, bool ignoreCase = false ) const;
    String          replace( char c, char replacement, bool deep = false, bool ignoreCase = false ) const;

    String          remove( String s, bool deep = false, bool ignoreCase = false ) const;
    String          remove( char c, bool ignoreCase = false ) const;

    String          removeFirst( String s, bool ignoreCase = false ) const;
    String          removeLast( String s, bool ignoreCase = false ) const;

    String          removeAny( String s, bool ignoreCase = false ) const;

    StringVector    split( char c = ' ', bool ignoreEmpty = true, bool ignoreCase = false ) const;

    // Unicode methods. cleanUtf8 will replace any bad code points with the unicode replacement
    // character (0xfffd).
    //
    Bool            isValidUtf8() const;
    String          cleanUtf8() const;

    // Methods for sequential-number naming. sequenceValue() returns UINT64_MAX if there is no
    // sequence.
    //
    Bool            splitSeq( String& base, UInt64& value ) const;
    String          seqBase() const;
    UInt64          seqValue() const;

    // Escape and unescape double quotes and backslashes throughout the string.
    //
    String          escape() const;
    String          unescape() const;

    // Hex escape/unescape double quote, backslash, cr, lf, and/or unprintable and non-ascii
    // characters. If invalid hex escape sequences are encountered while unescaping, they will be
    // replaced with plain text substitutes, e.g. \x0G would be rendered as Xx0G in the unescaped
    // string.
    //
    String          escapeHex( bool unprintable = false, bool nonAscii = false ) const;
    String          unescapeHex() const;

    // Strip c/c++ comments. Does not handle c++ raw string literals.
    //
    String          stripComments() const;

    // Like printf. These do not enforce the "C" locale, so you must do it yourself if necessary.
    //
    static String   format( const char* fmt, ... );
    static String   formatv( const char* fmt, va_list ap );

    // Format time (compactFormat=true produces colon-separated H:M:S type format).
    //
    static String   formatSeconds( UInt s, bool compactFormat = true );
    static String   formatMilliSeconds( UInt ms, bool compactFormat = true );
    static String   formatMicroSeconds( UInt us, bool compactFormat = true );

    // Format unix time. If secondsFromEpoch is zero, this will format the current time.
    //
    static String   formatTime( UInt secondsFromEpoch = 0 );

    // Format & parse general types.
    //
    static String   format( Bool val );
    static String   format( char val );
    static String   format( Float val );
    static String   format( Float val, int prec );
    static String   format( Double val );
    static String   format( Double val, int prec );

    static Bool     parse( String str, Bool& val );
    static Bool     parse( String str, char& val );
    static Bool     parse( String str, Float& val );
    static Bool     parse( String str, Double& val );

    Bool            parse( Bool& val ) const { return parse( *this, val ); }
    Bool            parse( char& val ) const { return parse( *this, val ); }
    Bool            parse( Float& val ) const { return parse( *this, val ); }
    Bool            parse( Double& val ) const { return parse( *this, val ); }

    // Radix for formatting/parsing integers.
    //
    enum Radix
    {
        Radix_Dec,
        Radix_Hex
    };

    // Format & parse signed integers.
    //
    static String   format( Int8  val, Radix radix = Radix_Dec );
    static String   format( Int16 val, Radix radix = Radix_Dec );
    static String   format( Int32 val, Radix radix = Radix_Dec );
    static String   format( Int64 val, Radix radix = Radix_Dec );

    static Bool     parse( String str, Int8&  val, Radix radix = Radix_Dec );
    static Bool     parse( String str, Int16& val, Radix radix = Radix_Dec );
    static Bool     parse( String str, Int32& val, Radix radix = Radix_Dec );
    static Bool     parse( String str, Int64& val, Radix radix = Radix_Dec );

    Bool            parse( Int8&  val, Radix radix = Radix_Dec ) const { return parse( *this, val, radix ); }
    Bool            parse( Int16& val, Radix radix = Radix_Dec ) const { return parse( *this, val, radix ); }
    Bool            parse( Int32& val, Radix radix = Radix_Dec ) const { return parse( *this, val, radix ); }
    Bool            parse( Int64& val, Radix radix = Radix_Dec ) const { return parse( *this, val, radix ); }

    // Format & parse unsigned integers.
    //
    static String   format( UInt8  val, Radix radix = Radix_Dec );
    static String   format( UInt16 val, Radix radix = Radix_Dec );
    static String   format( UInt32 val, Radix radix = Radix_Dec );
    static String   format( UInt64 val, Radix radix = Radix_Dec );

    static Bool     parse( String str, UInt8&  val, Radix radix = Radix_Dec );
    static Bool     parse( String str, UInt16& val, Radix radix = Radix_Dec );
    static Bool     parse( String str, UInt32& val, Radix radix = Radix_Dec );
    static Bool     parse( String str, UInt64& val, Radix radix = Radix_Dec );

    Bool            parse( UInt8&  val, Radix radix = Radix_Dec ) const { return parse( *this, val, radix ); }
    Bool            parse( UInt16& val, Radix radix = Radix_Dec ) const { return parse( *this, val, radix ); }
    Bool            parse( UInt32& val, Radix radix = Radix_Dec ) const { return parse( *this, val, radix ); }
    Bool            parse( UInt64& val, Radix radix = Radix_Dec ) const { return parse( *this, val, radix ); }

    // Format & parse tuples (space-delimited).
    //
    template<typename T> static inline String format( Dim N, const T& t )
    {
        auto s = format( t.m[0] );
        for ( Size i = 1; i < N; ++i )
            s = s + " " + format( t.m[i] );

        return s;
    }

    template<Dim N, typename S> static inline String format( const math::VecT<N, S>& v ) { return format( N, v ); }
    template<Dim N, typename S> static inline String format( const math::PosT<N, S>& v ) { return format( N, v ); }
    template<       typename S> static inline String format( const math::QuatT<S>&   v ) { return format( 4, v ); }
    template<Dim N, typename S> static inline String format( const math::MatT<N, S>& v ) { return format( N*N, v ); }
    template<       typename S> static inline String format( const math::RgbT<S>&    c ) { return format( 3, c ); }
    template<       typename S> static inline String format( const math::RgbaT<S>&   c ) { return format( 4, c ); }
    template<       typename S> static inline String format( const math::HsvT<S>&    c ) { return format( 3, c ); }
    template<       typename S> static inline String format( const math::HslT<S>&    c ) { return format( 3, c ); }

    template<typename T> static inline Bool parse( String s, Dim N, T& t )
    {
        auto v = s.split( ' ' );
        if ( v.size() < N )
            return false;

        auto ok = true;
        for ( Size i = 0; i < N; ++i )
            ok &= parse( v[i], t.m[i] );

        return ok;
    }

    template<Dim N, typename S> static inline Bool parse( String s, math::VecT<N, S>& v ) { return parse( s, N, v ); }
    template<Dim N, typename S> static inline Bool parse( String s, math::PosT<N, S>& v ) { return parse( s, N, v ); }
    template<       typename S> static inline Bool parse( String s, math::QuatT<S>&   v ) { return parse( s, 4, v ); }
    template<Dim N, typename S> static inline Bool parse( String s, math::MatT<N, S>& v ) { return parse( s, N*N, v ); }
    template<       typename S> static inline Bool parse( String s, math::RgbT<S>&    c ) { return parse( s, 3, c ); }
    template<       typename S> static inline Bool parse( String s, math::HsvT<S>&    c ) { return parse( s, 3, c ); }
    template<       typename S> static inline Bool parse( String s, math::HslT<S>&    c ) { return parse( s, 3, c ); }

    template<typename S> static inline Bool parse( String s, math::RgbaT<S>& c )
    {
        if ( parse( s, 4, c ) )
            return true;

        if ( !parse( s, 3, c ) )
            return false;

        c.a = ( S )1.0;
        return true;
    }

    template<Dim N, typename S> Bool parse( math::VecT<N, S>& v ) const { return parse<N, S>( *this, v ); }
    template<Dim N, typename S> Bool parse( math::PosT<N, S>& v ) const { return parse<N, S>( *this, v ); }
    template<       typename S> Bool parse( math::QuatT<S>&   v ) const { return parse<S>( *this, v ); }
    template<Dim N, typename S> Bool parse( math::MatT<N, S>& v ) const { return parse<N, S>( *this, v ); }
    template<       typename S> Bool parse( math::RgbT<S>&    c ) const { return parse<S>( *this, c ); }
    template<       typename S> Bool parse( math::RgbaT<S>&   c ) const { return parse<S>( *this, c ); }
    template<       typename S> Bool parse( math::HsvT<S>&    c ) const { return parse<S>( *this, c ); }
    template<       typename S> Bool parse( math::HslT<S>&    c ) const { return parse<S>( *this, c ); }

    // Format & parse Argb/ColorRef.
    //
    static String   format( math::Argb8 val );
    static String   format( math::ColorRef val );

    static Bool     parse( String str, math::Argb8& val );
    static Bool     parse( String str, math::ColorRef& val );

    Bool            parse( math::Argb8& val ) const { return parse( *this, val ); }
    Bool            parse( math::ColorRef& val ) const { return parse( *this, val ); }

    // For templates.
    //
    static String   format( String str ) { return str; }

};

// String on right hand side.
//
static inline Bool operator==( const char* l, String r )  { return String( l ) == r; }
static inline Bool operator==( char l, String r )         { return String( l ) == r; }

static inline Bool operator!=( const char* l, String r )  { return String( l ) != r; }
static inline Bool operator!=( char l, String r )         { return String( l ) != r; }

static inline String operator+( const char* l, String r ) { return String( l ) + r; }
static inline String operator+( char l, String r )        { return String( l ) + r; }

// std stream insertion/extraction.
//
template<typename Stream> static inline Stream& operator<<( Stream& stream, String str )
{
    stream << std::string( str.buf() );
    return stream;
}

template<typename Stream> static inline Stream& operator>>( Stream& stream, String& str )
{
    std::string sstr;
    stream >> sstr;
    str = sstr.c_str();
    return stream;
}

// Need this for HashMap.
//
namespace ds { static inline Size hash( String t ) { return t.hash(); } }

} // dl

#endif // DL_CORE_STRING_H
