// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_TYPES_H
#define DL_CORE_TYPES_H

#include "dl_defines.h"
#include "dl_math.h"

// General typedefs. These are not intended for allowing to switch types in libraries that use this
// code; rather they are to clearly define the types used throughout this system. Since files are
// read & written based on the types defined here, they can NEVER be changed.
//
namespace dl {

// These are aliases used throughout the system, such that when you say "Vec2" without specifying
// the type, you are using a Vec2d. This is used later in the node system, where attributes have
// methods like isVec2() and asVec2() where this refers to this Vec2 typedef for Vec2d.
//
typedef math::Vec2d     Vec2;
typedef math::Vec3d     Vec3;
typedef math::Vec4d     Vec4;
typedef math::Pos2d     Pos2;
typedef math::Pos3d     Pos3;
typedef math::Quatd     Quat;
typedef math::Rgbd      Rgb;
typedef math::Rgbf      Rgbf;
typedef math::Rgbd      Rgbd;
typedef math::Rgbad     Rgba;
typedef math::Rgba8     Rgba8;
typedef math::Mat3d     Mat3;
typedef math::Mat4d     Mat4;
typedef math::Xformd    Xform;
typedef math::Aabbd     Aabb;
typedef math::Obbd      Obb;
typedef math::Regioni   Region;

// This list is used to generate typed vectors in dl_vector.h.
//
#define DL_DATATYPE_ALIASES(X) \
    X(Vec2)     \
    X(Vec3)     \
    X(Vec4)     \
    X(Pos2)     \
    X(Pos3)     \
    X(Quat)     \
    X(Rgb)      \
    X(Rgba)     \
    X(Mat3)     \
    X(Mat4)     \
    X(Xform)    \
    X(Aabb)     \
    X(Obb)

// Generate typedefs for all dl::math types.
//
#define DL_MAKE_MATH_TYPEDEF(X) \
    typedef math::X##f X##f; \
    typedef math::X##d X##d; \
    typedef math::X##i X##i; \
    typedef math::X##u X##u;

DL_MAKE_MATH_TYPEDEF( Vec2 )
DL_MAKE_MATH_TYPEDEF( Vec3 )
DL_MAKE_MATH_TYPEDEF( Vec4 )
DL_MAKE_MATH_TYPEDEF( Pos2 )
DL_MAKE_MATH_TYPEDEF( Pos3 )
DL_MAKE_MATH_TYPEDEF( Quat )
DL_MAKE_MATH_TYPEDEF( Mat3 )
DL_MAKE_MATH_TYPEDEF( Mat4 )
DL_MAKE_MATH_TYPEDEF( Xform )
DL_MAKE_MATH_TYPEDEF( Aabb )
DL_MAKE_MATH_TYPEDEF( Obb )

using math::Rgb8;
using math::Rgba8;
using math::Argb8;
using math::ColorRef;

// The following macros are used to generate the DataType enum and its associated functions. Note
// that there are no DataType_Int8 and DataType_UInt8, because templates for UInt8 resolve to the
// same type as those for unsigned char. Therefore, we use the Char and Byte typedefs for these,
// exclusively.
//
#define DL_NATIVE_DATATYPES(X) \
    X(Null)     \
    X(Bool)     \
    X(Char)     \
    X(Byte)     \
    X(Int16)    \
    X(Int32)    \
    X(Int64)    \
    X(UInt16)   \
    X(UInt32)   \
    X(UInt64)   \
    X(Float)    \
    X(Double)   \
    X(Ptr)

#define DL_MATH_DATATYPES(X) \
    X(Vec2f)  X(Vec2d)  X(Vec2i)  X(Vec2u)      \
    X(Vec3f)  X(Vec3d)  X(Vec3i)  X(Vec3u)      \
    X(Vec4f)  X(Vec4d)  X(Vec4i)  X(Vec4u)      \
    X(Pos2f)  X(Pos2d)  X(Pos2i)                \
    X(Pos3f)  X(Pos3d)  X(Pos3i)                \
    X(Quatf)  X(Quatd)                          \
    X(Rgbf)   X(Rgbd)             X(Rgb8)       \
    X(Rgbaf)  X(Rgbad)            X(Rgba8)      \
    X(Mat3f)  X(Mat3d)  X(Mat3i)                \
    X(Mat4f)  X(Mat4d)  X(Mat4i)                \
    X(Xformf) X(Xformd) X(Xformi)               \
    X(Aabbf)  X(Aabbd)  X(Aabbi)                \
    X(Obbf)   X(Obbd)   X(Obbi)                 \
    X(Hsvf)   X(Hsvd)                           \
    X(Hslf)   X(Hsld)                           \
                                  X(Argb8)      \
                                  X(ColorRef)

#define DL_DATATYPES(X) DL_NATIVE_DATATYPES(X) DL_MATH_DATATYPES(X)

// The DataType enum. It is created using these X macros, ending up looking like this:
//
//  enum DataType
//  {
//      DataType_Null,
//      DataType_Bool,
//      DataType_Char,
//      [...]
//  };
//
// There is one entry for each type listed in the DL_NATIVE_DATATYPES/DL_MATH_DATATYPES macros
// above. One main purpose for this is to support the "smart" BufRefs defined below.
//
enum DataType
{
#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) DataType_##TYPE,
    DL_DATATYPES( DL_ENUMERATE_TYPES )
};

// dataTypeSize(DataType) returns the storage size (in bytes) for a given DataType.
//
static inline Size dataTypeSize( DataType type )
{
#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) case DataType_##TYPE: return sizeof( TYPE );
    switch ( type )
    {
    DL_NATIVE_DATATYPES( DL_ENUMERATE_TYPES )
    default:
        break;
    }

#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) case DataType_##TYPE: return sizeof( math::TYPE );
    switch ( type )
    {
    DL_MATH_DATATYPES( DL_ENUMERATE_TYPES )
    default:
        break;
    }

    DL_ASSERT( !!!"Unknown DataType." );
    return 0;
};

// dataElemType(DataType) returns the type of a compound DataType's elements.
//
static inline DataType dataElemType( DataType type )
{
    switch ( type )
    {
    case DataType_Null:   return type;
    case DataType_Bool:   return type;
    case DataType_Char:   return type;
    case DataType_Byte:   return type;
    case DataType_Int16:  return type;
    case DataType_Int32:  return type;
    case DataType_Int64:  return type;
    case DataType_UInt16: return type;
    case DataType_UInt32: return type;
    case DataType_UInt64: return type;
    case DataType_Float:  return type;
    case DataType_Double: return type;
    case DataType_Ptr:    return type;

    case DataType_Vec2f:
    case DataType_Vec3f:
    case DataType_Vec4f:
    case DataType_Pos2f:
    case DataType_Pos3f:
    case DataType_Quatf:
    case DataType_Rgbf:
    case DataType_Rgbaf:
    case DataType_Mat3f:
    case DataType_Mat4f:
    case DataType_Xformf:
    case DataType_Aabbf:
    case DataType_Obbf:
    case DataType_Hsvf:
    case DataType_Hslf:
        return DataType_Float;

    case DataType_Vec2d:
    case DataType_Vec3d:
    case DataType_Vec4d:
    case DataType_Pos2d:
    case DataType_Pos3d:
    case DataType_Quatd:
    case DataType_Rgbd:
    case DataType_Rgbad:
    case DataType_Mat3d:
    case DataType_Mat4d:
    case DataType_Xformd:
    case DataType_Aabbd:
    case DataType_Obbd:
    case DataType_Hsvd:
    case DataType_Hsld:
        return DataType_Double;

    case DataType_Vec2i:
    case DataType_Vec3i:
    case DataType_Vec4i:
    case DataType_Pos2i:
    case DataType_Pos3i:
    case DataType_Mat3i:
    case DataType_Mat4i:
    case DataType_Xformi:
    case DataType_Aabbi:
    case DataType_Obbi:
        return DataType_Int32;

    case DataType_Vec2u:
    case DataType_Vec3u:
    case DataType_Vec4u:
        return DataType_UInt32;

    case DataType_Rgb8:
    case DataType_Rgba8:
        return DataType_Byte;

    case DataType_Argb8:
    case DataType_ColorRef:
        return DataType_Byte;

    default:
        break;
    }

    DL_ASSERT( !!!"Unknown DataType." );
    return DataType_Null;
}

// dataElementSize(DataType) returns the size of a DataType's elements.
//
static inline Size dataElemSize( DataType type )
{
    switch ( type )
    {
    case DataType_Null:   return 0;
    case DataType_Bool:   return sizeof( Bool );
    case DataType_Char:   return sizeof( Char );
    case DataType_Byte:   return sizeof( Byte );
    case DataType_Int16:  return sizeof( Int16 );
    case DataType_Int32:  return sizeof( Int32 );
    case DataType_Int64:  return sizeof( Int64 );
    case DataType_UInt16: return sizeof( UInt16 );
    case DataType_UInt32: return sizeof( UInt32 );
    case DataType_UInt64: return sizeof( UInt64 );
    case DataType_Float:  return sizeof( Float );
    case DataType_Double: return sizeof( Double );
    case DataType_Ptr:    return sizeof( Ptr );

    case DataType_Vec2f:
    case DataType_Vec3f:
    case DataType_Vec4f:
    case DataType_Pos2f:
    case DataType_Pos3f:
    case DataType_Quatf:
    case DataType_Rgbf:
    case DataType_Rgbaf:
    case DataType_Mat3f:
    case DataType_Mat4f:
    case DataType_Xformf:
    case DataType_Aabbf:
    case DataType_Obbf:
    case DataType_Hsvf:
    case DataType_Hslf:
        return sizeof( Float );

    case DataType_Vec2d:
    case DataType_Vec3d:
    case DataType_Vec4d:
    case DataType_Pos2d:
    case DataType_Pos3d:
    case DataType_Quatd:
    case DataType_Rgbd:
    case DataType_Rgbad:
    case DataType_Mat3d:
    case DataType_Mat4d:
    case DataType_Xformd:
    case DataType_Aabbd:
    case DataType_Obbd:
    case DataType_Hsvd:
    case DataType_Hsld:
        return sizeof( Double );

    case DataType_Vec2i:
    case DataType_Vec3i:
    case DataType_Vec4i:
    case DataType_Pos2i:
    case DataType_Pos3i:
    case DataType_Mat3i:
    case DataType_Mat4i:
    case DataType_Xformi:
    case DataType_Aabbi:
    case DataType_Obbi:
        return sizeof( Int32 );

    case DataType_Vec2u:
    case DataType_Vec3u:
    case DataType_Vec4u:
        return sizeof( UInt32 );

    case DataType_Rgb8:
    case DataType_Rgba8:
        return sizeof( UInt8 );

    case DataType_Argb8:
    case DataType_ColorRef:
        return sizeof( UInt8 );

    default:
        break;
    }

    DL_ASSERT( !!!"Unknown DataType." );
    return 0;
}

// dataType<T>() gets the DataType corresponding to T.
//
template<typename T> static inline DataType dataType() { return DataType_Null; } // No impl.

#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) \
    template<> inline DataType dataType<TYPE>() { return DataType_##TYPE; } \
    template<> inline DataType dataType<const TYPE>() { return DataType_##TYPE; }
DL_NATIVE_DATATYPES( DL_ENUMERATE_TYPES )

#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) \
    template<> inline DataType dataType<math::TYPE>() { return DataType_##TYPE; } \
    template<> inline DataType dataType<const math::TYPE>() { return DataType_##TYPE; }
DL_MATH_DATATYPES( DL_ENUMERATE_TYPES )

// dataTypeName(DataType) returns the string name for a given DataType.
//
static inline const char* dataTypeName( DataType type )
{
#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) case DataType_##TYPE: return #TYPE;
    switch ( type )
    {
    DL_DATATYPES( DL_ENUMERATE_TYPES )
    default:
        break;
    }

    DL_ASSERT( !!!"Unknown DataType." );
    return "<unknown>";
};

// dataTypeName<T>() returns the string name (corresponding to DataType) for T.
//
template<typename T> static inline const char* dataTypeName()
{
    return dataTypeName( dataType<T>() );
}

// dataTypeSize<T>() returns the storage size (corresponding to DataType) for T.
//
template<typename T> static inline Size dataTypeSize()
{
    return dataTypeSize( dataType<T>() );
}

// Forward for BufRefT, which is used by it's base class, BufRef.
//
template<typename T> struct BufRefT;

// BufRef is a wrapper around some native memory buffer. Perhaps confusingly, it does not manage any
// memory of its own (hence the "Ref") and is used exclusively for passing buffers between different
// pieces of code with DataType, element count, etc. To reinterpret a buffer of one type as another,
// use the as<T>() method; if the types do not match, warnings will be printed.
//
struct BufRef
{
    Ptr         ptr;        // Pointer to the memory buffer (void* in BufRef, T* in BufRefT).
    DataType    type;       // The type of elements contained in the buffer.
    UInt        count;      // The number of elements in the buffer.
    Size        stride;     // If non-zero, loops over the buffer will skip by this amount.
    UInt        offset;     // If non-zero, loops over the buffer will begin at ptr+offset.

    // When set to true on a BufRef with non-null ptr, which is being given to an API method, this
    // will cause the method to assume ownership of the ptr, instead of making a copy as would be
    // done otherwise.
    // 
    // When set to true on a BufRef with null ptr, which is being used to clear a buffer internal to
    // the API, this will cause the API to forgo deleting its internal buffer, allowing the caller
    // to take ownership.
    //
    Bool        takeOwnership;

    BufRef() :
        ptr( nullptr ), type( DataType_Null ), count( 0 ), stride( 0 ), offset( 0 ), takeOwnership( false ) { }

    BufRef( Ptr ptr, DataType type, UInt count, Size stride, UInt offset = 0 ) :
        ptr( ptr ), type( type ), count( count ), stride( stride ), offset( offset ), takeOwnership( false ) { }

    template<typename T> BufRef( T* ptr, UInt count, Size stride = dataTypeSize<T>(), UInt offset = 0 ) :
        ptr( ( Ptr )ptr ), type( dataType<T>() ), count( count ), stride( stride ), offset( offset ), takeOwnership( false ) { }

    template<typename T> BufRef( BufRefT<T> o ) :
        ptr( ( Ptr )o.ptr ), type( o.type ), count( o.count ), stride( o.stride ), offset( o.offset ), takeOwnership( o.takeOwnership ) { }

    Size typeSize() const { return dataTypeSize( type ); }
    Size typeElemSize() const { return dataElemSize( type ); }
    Size typeElemCount() const { return typeSize() / typeElemSize(); }

    template<typename T> inline BufRefT<T> as() const
    {
        DataType srcType = type;
        DataType dstType = dataType<T>();

        if ( srcType == dstType )
            return BufRefT<T>( ( T* )ptr, count, stride, offset );

        Size srcTypeSize = dataTypeSize( srcType );
        Size dstTypeSize = dataTypeSize( dstType );
        Size srcTypeElemSize = dataElemSize( srcType );
        Size dstTypeElemSize = dataElemSize( dstType );
        if ( srcTypeSize == dstTypeSize && srcTypeElemSize == dstTypeElemSize )
        {
            //dl::logWarning( "Reading %s[%llu] buffer as %s[%llu].", dataTypeName( srcType ), count, dataTypeName( dstType ), count );
            return BufRefT<T>( ( T* )ptr, count, stride, offset );
        }

        auto ratio = ( Real )srcTypeSize / dstTypeSize;
        UInt count_ = ( UInt )( ( Real )count* ratio );
        Size stride_ = dstTypeSize;
        UInt offset_ = ( UInt )( ( Real )offset* ratio );
        //dl::logWarning( "Reading %s[%llu] buffer as %s[%llu].", dataTypeName( srcType ), count, dataTypeName( dstType ), count_ );
        return BufRefT<T>( ( T* )ptr, count_, stride_, offset_ );
    }

    template<typename T> operator BufRefT<T>() const { return as<T>(); }

    template<typename T> bool operator==( const T& o ) const
    {
        return o.ptr == ptr && o.type == type && o.count == count && o.offset == offset && o.stride == stride && o.takeOwnership == takeOwnership;
    }

    template<typename T> bool operator!=( const T& o ) const
    {
        return !( *this == o );
    }
};

// BufRefT is a typed version of BufRef. Its T* ptr replaces the void* ptr of BufRef, and it adds
// indexing operators.
//
template<typename T> struct BufRefT
{
    T*          ptr;
    DataType    type;
    UInt        count;
    Size        stride;
    UInt        offset;
    Bool        takeOwnership;

    BufRefT() :
        ptr( nullptr ), type( dataType<T>() ), count( 0 ), stride( dataTypeSize<T>() ), offset( 0 ), takeOwnership( false ) { }

    BufRefT( T* ptr, UInt count, Size stride = dataTypeSize<T>(), UInt offset = 0 ) :
        ptr( ptr ), type( dataType<T>() ), count( count ), stride( stride ), offset( offset ), takeOwnership( false ) { }

    template<typename T2> BufRefT( T2* ptr, UInt count, Size stride = dataTypeSize<T>(), UInt offset = 0 ) :
        ptr( ( T* )ptr ), type( dataType<T>() ), count( count ), stride( stride ), offset( offset ), takeOwnership( false ) { }

    template<typename T2> BufRefT( BufRefT<T2> o ) :
        ptr( ( T* )o.ptr ), type( dataType<T>() ), count( o.count ), stride( o.stride ), offset( o.offset ), takeOwnership( o.takeOwnership ) { }

    Size typeSize() const { return dataTypeSize( type ); }
    Size typeElemSize() const { return dataElemSize( type ); }
    Size typeElemCount() const { return typeSize() / typeElemSize(); }

    T& operator[]( UInt index )
    {
        DL_MATH_CHECKRANGE( index, count );
        return ptr[index];
    }

    const T& operator[]( UInt index ) const
    {
        DL_MATH_CHECKRANGE( index, count );
        return ptr[index];
    }

    template<typename T2> operator BufRefT<T2>() const { return BufRef( *this ).as<T2>(); }

    template<typename T2> bool operator==( const T2& o ) const
    {
        return o.ptr == ptr && o.type == type && o.count == count && o.offset == offset && o.stride == stride && o.takeOwnership == takeOwnership;
    }

    template<typename T2> bool operator!=( const T2& o ) const
    {
        return !( *this == o );
    }
};

// These macros generate BufRefT typedefs for all DataTypes.
//
#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) typedef BufRefT<TYPE> TYPE##BufRef;
DL_NATIVE_DATATYPES( DL_ENUMERATE_TYPES )

#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) typedef BufRefT<math::TYPE> TYPE##BufRef;
DL_MATH_DATATYPES( DL_ENUMERATE_TYPES )

// Small RAII class for managing a bool.
//
class BoolScope
{
private:
    bool& _ref;
    bool _prev;
public:
    BoolScope( bool& ref, bool scopeVal ) : _ref( ref ), _prev( ref ) { _ref = scopeVal; }
    ~BoolScope() { _ref = _prev; }
};

} // dl

#endif // DL_CORE_TYPES_H
