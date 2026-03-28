// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef BELLA_SDK_TYPES_H
#define BELLA_SDK_TYPES_H

#include "../dl_core/dl_types.h"
#include "../dl_core/dl_string.h"

namespace dl {
namespace bella_sdk {

// Interface type forwards.
//
namespace interfaces {
class INode;
class IAttr;
class IInput;
class IOutput;
}
using namespace interfaces;

// These macros map attr types, the sizes of their representations, and their node definition file
// names. This is used to generate the AttrType enum, attrTypeSize() and attrTypeName() functions.
//
#define BELLA_SDK_PRIMITIVE_ATTRTYPES(X) \
    X( Null,     Null     ) \
    X( Bool,     Bool     ) \
    X( Int,      Int      ) \
    X( UInt,     UInt     ) \
    X( Real,     Real     ) \
    X( Vec2,     Vec2     ) \
    X( Vec3,     Vec3     ) \
    X( Vec4,     Vec4     ) \
    X( Pos2,     Pos2     ) \
    X( Pos3,     Pos3     ) \
    X( Quat,     Quat     ) \
    X( Rgba,     Rgba     ) \
    X( Mat3,     Mat3     ) \
    X( Mat4,     Mat4     )

#define BELLA_SDK_COMPLEX_ATTRTYPES(X) \
    X( String,   Ptr      ) \
    X( Node,     INode*   ) \
    X( Buffer,   Ptr      ) \
    X( Array,    IInput** ) \
    X( Object,   IInput** )

#define BELLA_SDK_ATTRTYPES(X) BELLA_SDK_PRIMITIVE_ATTRTYPES(X) BELLA_SDK_COMPLEX_ATTRTYPES(X)

// The AttrType enum.
//
enum AttrType
{
#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE,SIZETYPE) AttrType_##TYPE,
    BELLA_SDK_ATTRTYPES( DL_ENUMERATE_TYPES )
};

// This returns the storage size for a given AttrType.
//
static inline Size attrTypeSize( AttrType type )
{
#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE,SIZETYPE) case AttrType_##TYPE: return sizeof( SIZETYPE );
    switch ( type )
    {
    BELLA_SDK_ATTRTYPES( DL_ENUMERATE_TYPES )
    default:
        break;
    }

    DL_ASSERT( !!!"Unknown AttrType" );
    return 0;
}

// This returns the short name for a given AttrType.
//
static inline const char* attrTypeName( AttrType type )
{
#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE,SIZETYPE) case AttrType_##TYPE: return #TYPE;
    switch ( type )
    {
    BELLA_SDK_ATTRTYPES( DL_ENUMERATE_TYPES )
    default:
        break;
    }

    DL_ASSERT( !!!"Unknown AttrType" );
    return "<unknown>";
}

// attrType<T>() gets the AttrType corresponding to T (for primitive types only).
//
template<typename T> static inline AttrType attrType();

#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(NAME,TYPE) \
    template<> inline AttrType attrType<TYPE>() { return AttrType_##NAME; } \
    template<> inline AttrType attrType<const TYPE>() { return AttrType_##NAME; }
BELLA_SDK_PRIMITIVE_ATTRTYPES( DL_ENUMERATE_TYPES )
template<> inline AttrType attrType<String>()       { return AttrType_String; }
template<> inline AttrType attrType<const String>() { return AttrType_String; }
template<> inline AttrType attrType<INode*>()       { return AttrType_Node; }
template<> inline AttrType attrType<const INode*>() { return AttrType_Node; }
template<> inline AttrType attrType<BufRef>()       { return AttrType_Buffer; }
template<> inline AttrType attrType<const BufRef>() { return AttrType_Buffer; }

// Check type of AttrType.
//
static inline bool isNull( AttrType type )   { return type == AttrType_Null; }
static inline bool isBool( AttrType type )   { return type == AttrType_Bool; }
static inline bool isInt( AttrType type )    { return type == AttrType_Int; }
static inline bool isUInt( AttrType type )   { return type == AttrType_UInt; }
static inline bool isReal( AttrType type )   { return type == AttrType_Real; }
static inline bool isVec2( AttrType type )   { return type == AttrType_Vec2; }
static inline bool isVec3( AttrType type )   { return type == AttrType_Vec3; }
static inline bool isVec4( AttrType type )   { return type == AttrType_Vec4; }
static inline bool isPos2( AttrType type )   { return type == AttrType_Pos2; }
static inline bool isPos3( AttrType type )   { return type == AttrType_Pos3; }
static inline bool isQuat( AttrType type )   { return type == AttrType_Quat; }
static inline bool isRgba( AttrType type )   { return type == AttrType_Rgba; }
static inline bool isMat3( AttrType type )   { return type == AttrType_Mat3; }
static inline bool isMat4( AttrType type )   { return type == AttrType_Mat4; }
static inline bool isString( AttrType type ) { return type == AttrType_String; }
static inline bool isNode( AttrType type )   { return type == AttrType_Node; }
static inline bool isBuffer( AttrType type ) { return type == AttrType_Buffer; }
static inline bool isArray( AttrType type )  { return type == AttrType_Array; }
static inline bool isObject( AttrType type ) { return type == AttrType_Object; }

static inline bool isScalar( AttrType type )
{
    switch ( type )
    {
    case AttrType_Int:
    case AttrType_UInt:
    case AttrType_Real:
        return true;
    default:
        return false;
    }
}

static inline bool isVector( AttrType type )
{
    switch ( type )
    {
    case AttrType_Vec2:
    case AttrType_Vec3:
    case AttrType_Vec4:
    case AttrType_Pos2:
    case AttrType_Pos3:
    case AttrType_Quat:
    case AttrType_Rgba:
        return true;
    default:
        return false;
    }
}

static inline bool isMatrix( AttrType type )
{
    switch ( type )
    {
    case AttrType_Mat3:
    case AttrType_Mat4:
        return true;
    default:
        return false;
    }
}

// Inputs and Outputs support automatic conversion (warnings will be printed) between convertible
// types For instance, if you use a Mat3 to set a Mat4 input, you will end up with the equivalent
// equivalent Mat4: an identity matrix with the specified Mat3 assigned to the upper 3x3. And vice-
// versa if you were to read a Mat3 input as a Mat4.
//
static inline bool canConvert( AttrType from, AttrType to, bool* narrowing = nullptr )
{
    if ( narrowing )
        *narrowing = false;

    switch ( from )
    {
    case AttrType_Bool:
        switch ( to )
        {
        case AttrType_Bool:
        case AttrType_Int:
        case AttrType_UInt:
        case AttrType_Real:
            return true;
        default:
            return false;
        }

    case AttrType_Int:
    case AttrType_UInt:
        switch ( to )
        {
        case AttrType_Bool:
        case AttrType_Int:
        case AttrType_UInt:
        case AttrType_Real:
            if ( narrowing ) *narrowing = (to == AttrType_Bool);
            return true;
        default:
            return false;
        }

    case AttrType_Real:
        switch ( to )
        {
        case AttrType_Int:
        case AttrType_UInt:
        case AttrType_Real:
        case AttrType_Vec2:
        case AttrType_Vec3:
        case AttrType_Vec4:
        case AttrType_Pos2:
        case AttrType_Pos3:
        case AttrType_Quat:
        case AttrType_Rgba:
            if ( narrowing ) *narrowing = (to == AttrType_Int || to == AttrType_UInt);
            return true;
        default:
            return false;
        }

    case AttrType_Vec2:
    case AttrType_Vec3:
    case AttrType_Vec4:
    case AttrType_Pos2:
    case AttrType_Pos3:
    case AttrType_Quat:
    case AttrType_Rgba:
        switch ( to )
        {
        case AttrType_Vec2:
        case AttrType_Vec3:
        case AttrType_Vec4:
        case AttrType_Pos2:
        case AttrType_Pos3:
        case AttrType_Quat:
        case AttrType_Rgba:
            switch ( from )
            {
            case AttrType_Vec3:
            case AttrType_Pos3:
                if ( narrowing ) *narrowing = (to == AttrType_Vec2 || to == AttrType_Pos2);
                break;
            case AttrType_Vec4:
            case AttrType_Quat:
            case AttrType_Rgba:
                if ( narrowing ) *narrowing = (to != AttrType_Vec4 && to != AttrType_Quat && to != AttrType_Rgba);
                break;
            default:
                break;
            }
            return true;
        default:
            return false;
        }

    case AttrType_Mat3:
    case AttrType_Mat4:
        switch ( to )
        {
        case AttrType_Mat3:
        case AttrType_Mat4:
            if ( narrowing ) *narrowing = (from == AttrType_Mat4 && to == AttrType_Mat3);
            return true;
        default:
            return false;
        }

    default:
        return from == to;
    }
}

// This is used to determine whether a given AttrType is supported as an Output attribute.
//
static inline bool isValidOutputType( AttrType type )
{
    switch ( type )
    {
    //case AttrType_Null:   // Ad-hoc inputs start as null. Maybe should have a type just for this.
    case AttrType_Node:     // Can't allow Node outputs, as this would allow creation of cycles.
    case AttrType_Buffer:   // Buffers are allowed, but only from .bnd files, with eval functions.
    case AttrType_Array:
    case AttrType_Object:
        return false;
    default:
        return true;
    }
}

// BufRefT typedefs for (primitive) AttrTypes.
//
#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE,TYPESIZE) typedef BufRefT<TYPE> TYPE##BufRef;
BELLA_SDK_PRIMITIVE_ATTRTYPES( DL_ENUMERATE_TYPES )

// LayoutType.
//
enum LayoutType
{
    LayoutType_Invalid,     // This should never be encountered.
    LayoutType_TopLevel,    // This is the top-level Layout.
    LayoutType_Header,      // This is a header, grouping parent Inputs.
    LayoutType_Input,       // This is a non-object Input.
    LayoutType_Object,      // This is an object Input.
    LayoutType_ObjectArray, // This is an object[] Input.
    LayoutType_Separator    // This is a separator between inputs.
};

} // bella_sdk
} // dl

#endif // BELLA_SDK_TYPES_H
