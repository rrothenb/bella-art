// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_MATH_H
#define DL_CORE_MATH_H

#include "dl_defines.h"
#include <cmath>
#include <cstring>

// Basic math library. Probably we will replace this with glm at some point. Though it is nice to
// have something simple in a single header.

namespace dl {
namespace math {

//=================================================================================================
//  Compiler.
//=================================================================================================

#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning(disable: 4201) // C4201: nameless struct/union.
    #pragma warning(disable: 4127) // C4127: conditional expression constant.
#elif defined(__clang__)
    #pragma clang diagnostic push
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
#endif

//=================================================================================================
// Vector declarations.
//=================================================================================================

template<Dim N, typename S> struct VecT;

typedef VecT<2, Float>      Vec2f;
typedef VecT<2, Double>     Vec2d;
typedef VecT<2, Int32>      Vec2i;
typedef VecT<2, UInt32>     Vec2u;

typedef VecT<3, Float>      Vec3f;
typedef VecT<3, Double>     Vec3d;
typedef VecT<3, Int32>      Vec3i;
typedef VecT<3, UInt32>     Vec3u;

typedef VecT<4, Float>      Vec4f;
typedef VecT<4, Double>     Vec4d;
typedef VecT<4, Int32>      Vec4i;
typedef VecT<4, UInt32>     Vec4u;

//=================================================================================================
// Position declarations.
//=================================================================================================

template<Dim N, typename S> struct PosT;

typedef PosT<2, Float>      Pos2f;
typedef PosT<2, Double>     Pos2d;
typedef PosT<2, Int32>      Pos2i;
typedef PosT<2, UInt32>     Pos2u;

typedef PosT<3, Float>      Pos3f;
typedef PosT<3, Double>     Pos3d;
typedef PosT<3, Int32>      Pos3i;
typedef PosT<3, UInt32>     Pos3u;

//=================================================================================================
// Quaternion declarations.
//=================================================================================================

template<typename S> struct QuatT;

typedef QuatT<Float>        Quatf;
typedef QuatT<Double>       Quatd;
typedef QuatT<Int32>        Quati;
typedef QuatT<UInt32>       Quatu;

//=================================================================================================
// Matrix declarations.
//=================================================================================================

template<Dim N, typename S> struct MatT;

typedef MatT<3, Float>      Mat3f;
typedef MatT<3, Double>     Mat3d;
typedef MatT<3, Int32>      Mat3i;
typedef MatT<3, UInt32>     Mat3u;

typedef MatT<4, Float>      Mat4f;
typedef MatT<4, Double>     Mat4d;
typedef MatT<4, Int32>      Mat4i;
typedef MatT<4, UInt32>     Mat4u;

//=================================================================================================
// Xform declarations.
//=================================================================================================

template<typename S> struct XformT;

typedef XformT<Float>       Xformf;
typedef XformT<Double>      Xformd;
typedef XformT<Int32>       Xformi;
typedef XformT<UInt32>      Xformu;

//=================================================================================================
// AABB declarations.
//=================================================================================================

template<typename S> struct AabbT;

typedef AabbT<Float>        Aabbf;
typedef AabbT<Double>       Aabbd;
typedef AabbT<Int32>        Aabbi;
typedef AabbT<UInt32>       Aabbu;

//=================================================================================================
// OBB declarations.
//=================================================================================================

template<typename S> struct ObbT;

typedef ObbT<Float>         Obbf;
typedef ObbT<Double>        Obbd;
typedef ObbT<Int32>         Obbi;
typedef ObbT<UInt32>        Obbu;

//=================================================================================================
// Rgb declarations.
//=================================================================================================

template<typename S> struct RgbT;

typedef RgbT<Float>         Rgbf;
typedef RgbT<Double>        Rgbd;
typedef RgbT<UInt8>         Rgb8; // '8' instead of 'u'.

//=================================================================================================
// Rgba declarations.
//=================================================================================================

template<typename S> struct RgbaT;

typedef RgbaT<Float>        Rgbaf;
typedef RgbaT<Double>       Rgbad;
typedef RgbaT<UInt8>        Rgba8; // '8' instead of 'u'.

//=================================================================================================
// Hsv declarations.
//=================================================================================================

template<typename S> struct HsvT;

typedef HsvT<Float>         Hsvf;
typedef HsvT<Double>        Hsvd;

//=================================================================================================
// Hsl declarations.
//=================================================================================================

template<typename S> struct HslT;

typedef HslT<Float>         Hslf;
typedef HslT<Double>        Hsld;

//=================================================================================================
// Special color struct declarations.
//=================================================================================================

struct                      Argb8;
struct                      ColorRef;

//=================================================================================================
// Region declarations.
//=================================================================================================

template<typename S> struct RegionT;

typedef RegionT<Int32>      Regioni;
typedef RegionT<UInt32>     Regionu;
typedef RegionT<Float>      Regionf;
typedef RegionT<Double>     Regiond;

//=================================================================================================
// Generate short aliases for a specific type.
//=================================================================================================

#define DL_MATH_MAKE_TYPEDEFS(T) \
    typedef dl::math::VecT<2, T> V2; \
    typedef dl::math::PosT<2, T> P2; \
    typedef dl::math::VecT<3, T> V3; \
    typedef dl::math::PosT<3, T> P3; \
    typedef dl::math::VecT<4, T> V4; \
    typedef dl::math::QuatT<T>    Q; \
    typedef dl::math::MatT<3, T> M3; \
    typedef dl::math::MatT<4, T> M4; \
    typedef dl::math::RgbT<T>    C3; \
    typedef dl::math::RgbaT<T>   C4; \
    typedef dl::math::HsvT<T>   HSV; \
    typedef dl::math::HslT<T>   HSL; \
    typedef dl::math::XformT<T>  XF; \
    typedef dl::math::AabbT<T> AABB; \
    typedef dl::math::ObbT<T>   OBB;

//=================================================================================================
// Options.
//=================================================================================================

#ifndef DL_MATH_ENABLE_MUTATING_OPERATORS
    #define DL_MATH_ENABLE_MUTATING_OPERATORS  0
#endif

#ifndef DL_MATH_CHECK_PODNESS
    #define DL_MATH_CHECK_PODNESS              1
#endif

#ifndef DL_MATH_USE_FORCEINLINE
    #define DL_MATH_USE_FORCEINLINE            0
#endif

#if DL_MATH_USE_FORCEINLINE
    #define DL_MATH_INL                        DL_FI
#else
    #define DL_MATH_INL                        inline
#endif

//=================================================================================================
// General macros.
//=================================================================================================

#if DL_MATH_ENABLE_RUNTIME_ASSERTS
    #define DL_MATH_CHECKZERO(s)                DL_ASSERT((s)!=0)
    #define DL_MATH_CHECKRANGE(i,N)             DL_ASSERT((i)<(N))
#else
    #define DL_MATH_CHECKZERO(s)                (void)0
    #define DL_MATH_CHECKRANGE(i,N)             (void)0
#endif

#define DL_MATH_FOR_X(var,from,toLessThan)      for(Dim var = from; var < toLessThan; ++var)
#define DL_MATH_FOR_i(from,toLessThan)          DL_MATH_FOR_X(i,from,toLessThan)
#define DL_MATH_FOR_j(from,toLessThan)          DL_MATH_FOR_X(j,from,toLessThan)
#define DL_MATH_FOR_k(from,toLessThan)          DL_MATH_FOR_X(k,from,toLessThan)
#define DL_MATH_FOR_ij(from,toLessThan)         DL_MATH_FOR_i(from,toLessThan) DL_MATH_FOR_j(from,toLessThan)
#define DL_MATH_FOR_ijk(from,toLessThan)        DL_MATH_FOR_i(from,toLessThan) DL_MATH_FOR_j(from,toLessThan) DL_MATH_FOR_k(from,toLessThan)

//=================================================================================================
// Numeric constants.
//=================================================================================================

namespace nc {
static const Double e                           = 2.71828182845904523536;   // e                (M_E)
static const Double log2e                       = 1.44269504088896340736;   // log2(e)          (M_LOG2E)
static const Double log10e                      = 0.434294481903251827651;  // log10(e)         (M_LOG10E)
static const Double ln2                         = 0.693147180559945309417;  // ln(2)            (M_LN2)
static const Double ln10                        = 2.30258509299404568402;   // ln(10)           (M_LN10)
static const Double pi                          = 3.14159265358979323846;   // pi               (M_PI)
static const Double tau                         = 6.28318530717958647692;   // 2*pi
static const Double halfPi                      = 1.57079632679489661923;   // pi/2             (M_PI_2)
static const Double quarterPi                   = 0.785398163397448309616;  // pi/4             (M_PI_4)
static const Double oneOverPi                   = 0.318309886183790671538;  // 1/pi             (M_1_PI)
static const Double twoOverPi                   = 0.636619772367581343076;  // 2/pi             (M_2_PI)
static const Double twoOverRootPi               = 1.12837916709551257390;   // 2/sqrt(pi)       (M_2_SQRTPI)
static const Double rootTwo                     = 1.41421356237309504880;   // sqrt(2)          (M_SQRT2)
static const Double oneOverRootTwo              = 0.707106781186547524401;  // 1/sqrt(2)        (M_SQRT1_2)

static const Double radPerDeg                   = 180.0/pi;
static const Double degPerRad                   = pi/180.0;

static const Float  epsilonF                    = 1.0e-5f;                  // Used to determine Float equality.
static const Double epsilonD                    = 10.0e-13;                 // Used to determine Double equality.
} // nc

//=================================================================================================
// Physical constants.
//=================================================================================================

namespace pc {
static const Double c                           = 299792458.0;              // Speed of light in vacuum     (m/s)
static const Double G                           = 6.6740831e-11;            // Gravitational constant       (m^3*kg^-1*s^-2)
static const Double g                           = 9.80665;                  // Standard gravity             (m/s^2)
static const Double atm                         = 101325.0;                 // Standard atmosphere          (Pa)
static const Double h                           = 6.62606993489e-34;        // Planck constant              (J*s)
static const Double hbar                        = 1.05457180013e-34;        // Dirac constant               (J*s/rad)
static const Double eV                          = 1.60217656535e-19;        // Electron volt                (J)
static const Double me                          = 0.5109989461;             // Electron mass                (MeV)
static const Double mp                          = 938.2720813;              // Proton mass                  (MeV)
static const Double mn                          = 939.5654133;              // Neutron mass                 (MeV)
static const Double mu                          = 931.49406121;             // Atomic mass constant         (MeV)
static const Double Na                          = 6.022140857e23;           // Avogadro's constant          (mol^-1)
static const Double k                           = 1.38064852e-23;           // Boltzmann's constant         (J*K^-1)
static const Double R                           = 8.314459848;              // Ideal gas constant           (J*mol^-1*K^-1)
} // pc

//=================================================================================================
// Generic functions.
//=================================================================================================

template<typename T> static DL_MATH_INL T       min( T a, T b ) { return a < b ? a : b; }
template<typename T> static DL_MATH_INL T       min( T a, T b, T c ) { return min( a, min( b, c ) ); }
template<typename T> static DL_MATH_INL T       min( T a, T b, T c, T d ) { return min( a, min( b, c, d ) ); }

template<typename T> static DL_MATH_INL T       max( T a, T b ) { return a > b ? a : b; }
template<typename T> static DL_MATH_INL T       max( T a, T b, T c ) { return max( a, max( b, c ) ); }
template<typename T> static DL_MATH_INL T       max( T a, T b, T c, T d ) { return max( a, max( b, c, d ) ); }

template<typename T> static DL_MATH_INL T       d2r( T d ) { return ( T )( ( Double )d * nc::degPerRad ); }
template<> DL_MATH_INL Float                    d2r( Float  d ) { return d * ( Float )nc::degPerRad; }
template<> DL_MATH_INL Double                   d2r( Double d ) { return d * nc::degPerRad; }

template<typename T> static DL_MATH_INL T       r2d( T r ) { return ( T )( ( Double )r * nc::radPerDeg ); }
template<> DL_MATH_INL Float                    r2d( Float  r ) { return r * ( Float )nc::radPerDeg; }
template<> DL_MATH_INL Double                   r2d( Double r ) { return r * nc::radPerDeg; }

template<typename T> static DL_MATH_INL T       epsilon() { return ( T )0; }
template<> DL_MATH_INL Float                    epsilon() { return nc::epsilonF; }
template<> DL_MATH_INL Double                   epsilon() { return nc::epsilonD; }

template<typename T> static DL_MATH_INL Bool    zerolike( T t ) { return t == ( T )0; }
template<> DL_MATH_INL Bool                     zerolike( Float  s ) { return fabs( s ) < epsilon<Float>(); }
template<> DL_MATH_INL Bool                     zerolike( Double s ) { return fabs( s ) < epsilon<Double>(); }

template<typename L, typename R> static DL_MATH_INL Bool equal( L l, R r ) { return l == r; }
static DL_MATH_INL Bool                         equal( Float  l, Float  r ) { return zerolike( r-l ); }
static DL_MATH_INL Bool                         equal( Double l, Double r ) { return zerolike( r-l ); }

template<typename T> static DL_MATH_INL T       round( Float  v ) { return ( T )std::round( v ); }
template<typename T> static DL_MATH_INL T       round( Double v ) { return ( T )std::round( v ); }

// These are as accurate as std::round, in case that is not available.
//template<typename T> static DL_MATH_INL T     round( Float  v ) { return ( T )( v > 0 ? v < ( floor( v ) + 0.5 ) ? floor( v ) : ceil( v ) : v > ( ceil( v ) - 0.5 ) ? ceil( v ) : floor( v ) ); }
//template<typename T> static DL_MATH_INL T     round( Double v ) { return ( T )( v > 0 ? v < ( floor( v ) + 0.5 ) ? floor( v ) : ceil( v ) : v > ( ceil( v ) - 0.5 ) ? ceil( v ) : floor( v ) ); }

template<typename T> static DL_MATH_INL T       clamp( T t, T min, T max ) { return t > max ? max : t < min ? min : t; }
template<typename T> static DL_MATH_INL T       wrap( T t, T max ) { return t - max * floor( t / max ); }
template<typename T> static DL_MATH_INL Int32   compare( T a, T b ) { return a < b ? -1 : a > b ? 1 : 0; }
template<typename T, typename S> static DL_MATH_INL T lerp( T l, T r, S lt, S rt ) { return l*lt + r*rt; }
template<typename T, typename S> static DL_MATH_INL T lerp( T l, T r, S t ) { return lerp( l, r, ( S )1 - t, t ); }



} // math

// Tuple hash, defined in dl::ds.
//
namespace ds {
template<Dim N, typename T> static DL_MATH_INL UInt64 hash( const T& t )
{
    using ds::hash;
    UInt64 s = hash( t.comp[0] );
    DL_MATH_FOR_i( 1, N ) { s ^= hash( t.comp[i] ) + 0x9e3779b9; }
    return s;
}
} // ds

//=================================================================================================
// The "op" namespace contains the core vector algebra logic. Currently, this is all generic, but
// can be tuned for specific types in the future, by specializing the desired functions. Functions
// in this namespace are generally suitable for use from other contexts; as long as you supply the
// required dimensions, types, etc, you can re-use the code directly; otherwise, having the logic
// extracted here makes it easier to ensure that if you must completely reimplement something, you
// can be sure to use the correct conventions (by looking here to see what they are).
//
// As much as possible, these functions should be useable from any context where a vector (or such)
// has .x, .y, and .z components, and where a matrix has .m00, .m01, and so forth. In some cases,
// it is assumed that there will be further information available; for instance, in some cases, it
// is necessary that a matrix have a "V" typedef, representing the vector type of its columns. Or,
// some of the functions refer to a "c" component, which in the case of a vector refers to an array
// of scalars corresponding to its components, but which refers to an array of column vectors, in
// the case of a matrix. For instance, look at the compare(T,T) function; when given a vector, it
// will compare the scalar values in the vector by iterating vector::comp[], while with a matrix,
// it will (recursively) compare the vector values of the matrix by iterating matrix::comp[].
//=================================================================================================

namespace math {
namespace op {

// Print types to a stream.
//
template<Dim N, typename T, typename Stream> static DL_MATH_INL Stream& op_tostream( Stream& s, const T& v )
{
    s << v.comp[0];
    DL_MATH_FOR_i( 1, N ) { s << ' ' << v.comp[i]; }
    return s;
}

// Read types from a stream.
//
template<Dim N, typename T, typename Stream> static DL_MATH_INL Stream& op_fromstream( Stream& s, T& v )
{
    char c;
    s >> v.comp[0];
    DL_MATH_FOR_i( 1, N ) { s >> c >> v.comp[i]; }
    return s;
}

// Internal sqrt.
//
template<typename T> static DL_MATH_INL T sqrt( T t )
{
    return ( T )::sqrt( ( Double )t );
}

// Create a tuple from a buffer.
//
template<typename T, typename S> static DL_MATH_INL T from_ptr( const S* p )
{
    T o = { 0 };
    if ( p ) { o = *( ( T* )p ); }
    return o;
}

// Tuple equality.
//
template<Dim N, typename T1, typename T2> static DL_MATH_INL Bool op_equal( const T1& l, const T2& r )
{
    DL_MATH_FOR_i( 0, N ) { if ( !math::equal( l.comp[i], r.comp[i] ) ) return false; }
    return true;
}

// Tuple zerolike.
//
template<Dim N, typename T> static DL_MATH_INL Bool zerolike( const T& t )
{
    DL_MATH_FOR_i( 0, N ) { if ( !math::zerolike( t.comp[i] ) ) return false; }
    return true;
}

// Get (a const reference to) component 'i' (scalar for vector, vector for matrix).
//
template<Dim N, typename S, typename T> static DL_MATH_INL const S& op_index( const T& t, Dim i )
{
    DL_MATH_CHECKRANGE( i, N );
    return t.comp[i];
}

// Tuple compare.
//
template<Dim N, typename T> static DL_MATH_INL Int32 compare( const T& l, const T& r )
{
    DL_MATH_FOR_i( 0, N )
    {
        Int32 c = math::compare( l.comp[i], r.comp[i] );
        if ( c != 0 )
            return c;
    }
    return 0;
}

// Tuple lesser than.
//
template<Dim N, typename T> static DL_MATH_INL Bool op_less( const T& l, const T& r )
{
    return compare( l, r ) < 0;
}

// Tuple greater than.
//
template<Dim N, typename T> static DL_MATH_INL Bool op_greater( const T& l, const T& r )
{
    return compare( l, r ) > 0;
}

// Tuple negation.
//
template<Dim N, typename T> static DL_MATH_INL T op_negate( const T& t )
{
    T o;
    DL_MATH_FOR_i( 0, N ) { o.comp[i] = -t.comp[i]; }
    return o;
}

// Tuple/tuple arithmetic.
//
template<Dim N, typename S, typename T> static DL_MATH_INL T op_ttadd( T l, T r ) { T o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = l.comp[i] + r.comp[i]; } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL T op_ttsub( T l, T r ) { T o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = l.comp[i] - r.comp[i]; } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL T op_ttmul( T l, T r ) { T o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = l.comp[i] * r.comp[i]; } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL T op_ttdiv( T l, T r ) { return op_ttmul<N, S, T>( l, ( S )1 / r ); }

template<Dim N, typename S> DL_MATH_INL VecT<N, Int32> op_ttdiv( VecT<N, Int32> l, VecT<N, Int32> r ) { return ( VecT<N, Int32> )( ( VecT<N, Double> )l / ( VecT<N, Double> )r ); }
template<Dim N, typename S> DL_MATH_INL PosT<N, Int32> op_ttdiv( PosT<N, Int32> l, PosT<N, Int32> r ) { return ( PosT<N, Int32> )( ( PosT<N, Double> )l / ( PosT<N, Double> )r ); }

// Tuple/scalar arithmetic.
//
template<Dim N, typename S, typename T> static DL_MATH_INL T op_tsadd( T l, S r ) { T o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = l.comp[i] + r; } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL T op_tssub( T l, S r ) { T o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = l.comp[i] - r; } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL T op_tsmul( T l, S r ) { T o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = l.comp[i] * r; } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL T op_tsdiv( T l, S r ) { DL_MATH_CHECKZERO( r ); return op_tsmul<N, S, T>( l, ( S )1 / r ); }

template<Dim N, typename S> DL_MATH_INL VecT<N, Int32> op_tsdiv( VecT<N, Int32> l, Int32 r ) { return ( VecT<N, Int32> )( ( VecT<N, Double> )l / ( Double )r ); }
template<Dim N, typename S> DL_MATH_INL PosT<N, Int32> op_tsdiv( PosT<N, Int32> l, Int32 r ) { return ( PosT<N, Int32> )( ( PosT<N, Double> )l / ( Double )r ); }

// Scalar/tuple arithmetic.
//
template<Dim N, typename S, typename T> static DL_MATH_INL T op_stadd( S l, T r ) { T o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = l + r.comp[i]; } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL T op_stsub( S l, T r ) { T o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = l - r.comp[i]; } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL T op_stmul( S l, T r ) { T o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = l * r.comp[i]; } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL T op_stdiv( S l, T r ) { T o; DL_MATH_FOR_i( 0, N ) { DL_MATH_CHECKZERO( r.comp[i] );  o.comp[i] = l / r.comp[i]; } return o; }

template<Dim N, typename S> DL_MATH_INL VecT<N, Int32> op_stdiv( Int32 l, VecT<N, Int32> r ) { return ( VecT<N, Int32> )( ( Double )l / ( VecT<N, Double> )r ); }
template<Dim N, typename S> DL_MATH_INL PosT<N, Int32> op_stdiv( Int32 l, PosT<N, Int32> r ) { return ( PosT<N, Int32> )( ( Double )l / ( PosT<N, Double> )r ); }

// Quaternion/quaternion arithmetic.
//
template<typename Q> static DL_MATH_INL Q op_qqprod( Q l, Q r )
{
    Q o;
    o.x =  l.x*r.w + l.y*r.z - l.z*r.y + l.w*r.x;
    o.y = -l.x*r.z + l.y*r.w + l.z*r.x + l.w*r.y;
    o.z =  l.x*r.y - l.y*r.x + l.z*r.w + l.w*r.z;
    o.w = -l.x*r.x - l.y*r.y - l.z*r.z + l.w*r.w;
    return o;
}

// Tuple min/max.
//
template<Dim N, typename S, typename T> static DL_MATH_INL S min( T t ) { DL_SASSERT( N > 0 ); S o = t.comp[0]; DL_MATH_FOR_i( 1, N ) { if ( t.comp[i] < o ) { o = t.comp[i]; } } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL S max( T t ) { DL_SASSERT( N > 0 ); S o = t.comp[0]; DL_MATH_FOR_i( 1, N ) { if ( t.comp[i] > o ) { o = t.comp[i]; } } return o; }
template<Dim N, typename S, typename T> static DL_MATH_INL S avg( T t ) { DL_SASSERT( N > 0 ); S o = t.comp[0]; DL_MATH_FOR_i( 1, N ) { o += t.comp[i]; } return o/N; }

// Tuple order.
//
template<Dim N, typename S, typename T> static DL_MATH_INL VecT<N, UInt32> order( T t )
{
    VecT<N, UInt32> o;
    DL_MATH_FOR_i( 0, N ) { o.comp[i] = ( UInt32 )i; }
    DL_MATH_FOR_ij( 0, N )
    {
        if ( j < ( N-1 ) && t.comp[j] > t.comp[j+1] )
        {
            using std::swap;
            swap( t.comp[j], t.comp[j+1] );
            swap( o.comp[j], o.comp[j+1] );
        }
    }
    return o;
}

// Tuple round.
//
template<Dim N, typename S> static DL_MATH_INL VecT<N, S> round( VecT<N, S> v )
{
    DL_SASSERT( N > 0 );
    DL_MATH_FOR_i( 0, N ) { v.comp[i] = ( S )math::round<Int64>( ( Real )v.comp[i] ); }
    return v;
}

template<typename S> static DL_MATH_INL RgbT<S> round( RgbT<S> c )
{
    DL_MATH_FOR_i( 0, 3 ) { c.comp[i] = ( S )math::round<Int64>( ( Real )c.comp[i] ); }
    return c;
}

template<typename S> static DL_MATH_INL RgbaT<S> round( RgbaT<S> c )
{
    DL_MATH_FOR_i( 0, 4 ) { c.comp[i] = ( S )math::round<Int64>( ( Real )c.comp[i] ); }
    return c;
}

// Matrix min/max.
//
template<Dim N, typename S, typename M> static DL_MATH_INL S minM( const M& m ) { DL_SASSERT( N > 0 ); S o = min( m.comp[0] ); DL_MATH_FOR_i( 1, N ) { S x = min( m.comp[i] ); if ( x < o ) { o = x; } } return o; }
template<Dim N, typename S, typename M> static DL_MATH_INL S maxM( const M& m ) { DL_SASSERT( N > 0 ); S o = max( m.comp[0] ); DL_MATH_FOR_i( 1, N ) { S x = max( m.comp[i] ); if ( x > o ) { o = x; } } return o; }

// Tuple sum.
//
template<Dim N, typename S, typename T> static DL_MATH_INL S sum( T t )
{
    DL_SASSERT( N > 0 );
    S o = t.comp[0];
    DL_MATH_FOR_i( 1, N ) { o += t.comp[i]; }
    return o;
}

// Tuple absolute value.
//
template<Dim N, typename S, typename T> static DL_MATH_INL T abs( T t )
{
    T o;
    DL_MATH_FOR_i( 0, N ) { o.comp[i] = ( S )fabs( t.comp[i] ); }
    return o;
}

// Tuple dot product.
//
template<Dim N, typename S, typename T> static DL_MATH_INL S dot( T l, T r )
{
    S o = ( S )0;
    DL_MATH_FOR_i( 0, N ) { o += l.comp[i]*r.comp[i]; }
    return o;
}

// Tuple norm squared (i.e. inner product, a.k.a lengthSquared, magnitudeSquared).
//
template<Dim N, typename S, typename T> static DL_MATH_INL S normSq( T t )
{
    return dot( t, t );
}

// Tuple norm (a.k.a length, magnitude).
//
template<Dim N, typename S, typename T> static DL_MATH_INL S norm( T t )
{
    return op::sqrt( normSq( t ) );
}
template<Dim N, typename T> DL_MATH_INL Int32 norm( VecT<N, Int32> t )
{
    return ( Int32 ) norm( ( VecT<N, Double> )t );
}

// Tuple unit (a.k.a. normalize).
//
template<Dim N, typename S, typename T> static DL_MATH_INL T unit( T t )
{
    return t * ( ( S )1 / norm( t ));
}

// Unit Matrix (normalize components to norm=1, according to longest component).
//
template<Dim N, typename S, typename M> static DL_MATH_INL M unitM( const M& m )
{
    M o = m;
    S nmax = ( S )0;
    DL_MATH_FOR_i( 0, 3 )
    {
        S n = norm( ( typename M::V3 )m.comp[i] );
        if ( n > nmax )
            nmax = n;
    }

    S imax = nmax != ( S )0 ? ( S )1 / nmax : ( S )1;
    DL_MATH_FOR_i( 0, 3 )
    {
        typename M::V3 v = ( ( typename M::V3 )m.comp[i] ) * imax;
        o.comp[i].x = v.x;
        o.comp[i].y = v.y;
        o.comp[i].z = v.z;
    }
    return o;
}

// Find if a tuple is a unit (a.k.a normalized) vector.
//
template<Dim N, typename S, typename T> static DL_MATH_INL Bool isUnit( T t )
{
    return math::equal( ( S )1, norm( t ) );
}

// Find if a matrix is a unit (a.k.a. normalized) matrix.
//
template<Dim N, typename S, typename M> static DL_MATH_INL Bool isUnitM( const M& m )
{
    S nmax = ( S )0;
    DL_MATH_FOR_i( 0, 3 )
    {
        S n = norm( ( typename M::V3 )m.comp[i] );
        if ( n > nmax )
            nmax = n;
    }
    return equal( nmax, ( S )1 );
}

// Tuple cross product.
//
template<Dim N, typename S> static DL_MATH_INL VecT<N, S> cross( VecT<N, S> l, VecT<N, S> r )
{
    VecT<N, S> o;
    o.x = l.y*r.z - l.z*r.y;
    o.y = l.z*r.x - l.x*r.z;
    o.z = l.x*r.y - l.y*r.x;
    return o;
}

// Scalar "cross product" for 2-vectors. Yields the area of the parallelogram described by the two
// vectors. The sign also indicates clockwise or counter-clockwise rotation from one to the other.
//
template<typename S> static DL_MATH_INL S cross( VecT<2, S> l, VecT<2, S> r )
{
    return l.x*r.y - l.y*r.x;
}

// Convert a Vec4 to Pos3 (with division by the vector's w component).
//
template<typename S> static DL_MATH_INL PosT<3, S> op_v4top3( VecT<4, S> v )
{
    DL_MATH_CHECKZERO( v.w );
    S i = ( S )1/v.w;
    PosT<3, S> o;
    o.x = v.x*i;
    o.y = v.y*i;
    o.z = v.z*i;
    return o;
}

// Vector project.
//
template<typename S, typename T> static DL_MATH_INL T project( T v, T n )
{
    S dotnn = dot( n, n );
    DL_MATH_CHECKZERO( dotnn );
    return ( dot( v, n ) / dotnn ) * n;
}

// Vector reflect.
//
template<typename S, typename T> static DL_MATH_INL T reflect( T v, T n )
{
    return v - ( ( S )2 * dot( v, n ) ) * n;
}

// Point/point distance.
//
template<typename S, typename T> static DL_MATH_INL S distance( T a, T b )
{
    return norm( b - a );
}

// Point/plane project.
//
template<typename S, typename P, typename V> static DL_MATH_INL P project( P pt, P pl, V n )
{
    return pt - project( ( pt - pl ), n );
}

// Point/plane reflect.
//
template<typename S, typename P, typename V> static DL_MATH_INL P reflect( P pt, P pl, V n )
{
    return pt - ( ( S )2 * ( project( pt, pl, n ) - pl ) );
}

// Lerp for any tuple.
//
template<Dim N, typename T, typename S> static DL_MATH_INL T lerp( T l, T r, S lt, S rt )
{
    T o;
    DL_MATH_FOR_i( 0, N ) { o.comp[i] = l.comp[i]*lt + r.comp[i]*rt; }
    return o;
}

// Normalized linear quaternion interpolation.
//
template<Dim N, typename T, typename S> static DL_MATH_INL T nlerp( T l, T r, S t )
{
    S lt = ( S )1 - t;
    S rt = dot( l, r ) >= ( S )0 ? t : -t;
    return unit( lerp( l, r, lt, rt ) );
}

// Standard slerp, using trigonometric functions.
//
template<Dim N, typename T, typename S> static DL_MATH_INL T slerp( T l, T r, S t )
{
    S lt, rt;
    S ca = dot( l, r );
    if ( fabs( ca ) < ( S )0.99999 )
    {
        S a = ( S )acos( ca );
        S rsa = ( S )1 / ( S )sin( a );
        lt = ( S )sin( ( ( S )1 - t ) * a ) * rsa;
        rt = ( S )sin( t * a ) * rsa;
    }
    else
    {
        lt = ( S )1 - t;
        rt = t;
    }
    return unit( lerp( l, r, lt, ca >= ( S )0 ? rt : -rt ) );
}

// Faster, less accurate slerp approximation (abs(norm(a-b))<1e-2). See here for an interesting discussion:
//
//  a) http://zeuxcg.org/2015/07/23/approximating-slerp
//  b) http://zeuxcg.org/2016/05/05/optimizing-slerp
//
template<Dim N, typename T, typename S> static DL_MATH_INL T fnlerp( T l, T r, S t )
{
    S ca = dot( l, r );
    S d = ( S )fabs( ca );
    S k = ( S )0.931872 + d * ( ( S )-1.25654 + d * ( S )0.331442 );
    S ot = t + t * ( t - ( S )0.5 ) * ( t - ( S )1 ) * k;
    S lt = ( S )1 - ot;
    S rt = ca >= ( S )0 ? ot : -ot;
    return unit( lerp( l, r, lt, rt ) );
}

// Slower, more accurate slerp approximation (abs(norm(a-b))<1e-3).
//
template<Dim N, typename T, typename S> static DL_MATH_INL T onlerp( T l, T r, S t )
{
    S ca = dot( l, r );
    S d = ( S )fabs( ca );
    S A = ( S )1.0904 + d * ( ( S )-3.2452 + d * ( ( S )3.55645 - d * ( S )1.43519 ) );
    S B = ( S )0.848013 + d * ( ( S )-1.06021 + d * ( S )0.215638 );
    S k = A * ( t - ( S )0.5 ) * ( t - ( S )0.5 ) + B;
    S ot = t + t * ( t - ( S )0.5 ) * ( t - ( S )1 ) * k;
    S lt = ( S )1 - ot;
    S rt = ca >= ( S )0 ? ot : -ot;
    return unit( lerp( l, r, lt, rt ) );
}

// Compute a quaternion for the given axis/angle.
//
template<typename Q, typename V, typename S> static DL_MATH_INL Q qfromaa( V v, S a )
{
    S s = ( S )sin( a *( S )0.5 );
    S c = ( S )cos( a *( S )0.5 );
    Q o;
    v = unit( v );
    o.x = v.x * s;
    o.y = v.y * s;
    o.z = v.z * s;
    o.w = c;
    return o;
}

// Compute an axis/angle pair for quaternion q.
//
template<typename Q, typename V, typename S> static DL_MATH_INL void qtoaa( Q q, V& v, S& a )
{
    q = unit( q );
    a = ( S )2 * ( S )acos( q.w );
    S s = ( S )1 / sqrt( ( S )1 - q.w*q.w );
    if ( s < ( S )0.001 ) s = ( S )1;
    v.x = q.x * s;
    v.y = q.y * s;
    v.z = q.z * s;
}

// Compute Euler angles for quaternion q.
//
template<Dim N, typename S> static DL_MATH_INL VecT<N, S> eulers( QuatT<S> q )
{
    return eulers( op_q2m( q ) );
}

// QuatT * Vec3 -> Vec3.
//
template<typename S, typename Q, typename V> static DL_MATH_INL V op_qv3mul( Q q, V v )
{
    // https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication
    V t = ( S )2 * op::cross( vector( q ), v );
    V o = v + q.w * t + op::cross( vector( q ), t );
    return o;
}

// QuatT * Vec4 -> Vec4. Expects V4 to have a 'v3' field referring to itself as a 3-vector.
//
template<typename S, typename Q, typename V4> DL_MATH_INL V4 op_qv4mul( Q q, V4 v )
{
    V4 o;
    o.v3 = q * v.v3;
    o.w = v.w;
    return o;
}

// QuatT * Pos3 -> Pos3. Expects P to have a 'v3' field referring to itself as a 3-vector.
//
template<typename S, typename Q, typename P> static DL_MATH_INL P op_qp3mul( Q q, P p )
{
    return ( P )( q * p.v3 );
}

// Add square matrices of size N, with column vectors contained in an array 'c'.
//
template<Dim N, typename S, typename M> static DL_MATH_INL M op_mmadd( const M& l, const M& r )
{
    M o;
    DL_MATH_FOR_i( 0, N ) { o.comp[i] = l.comp[i] + r.comp[i]; }
    return o;
}

// Subtract square matrices of size N, with column vectors contained in an array 'c'.
//
template<Dim N, typename S, typename M> static DL_MATH_INL M op_mmsub( const M& l, const M& r )
{
    M o;
    DL_MATH_FOR_i( 0, N ) { o.comp[i] = l.comp[i] - r.comp[i]; }
    return o;
}

// Multiply square matrices with N*N buffers of type S, named 'm'.
//
template<Dim N, typename S, typename M> static DL_MATH_INL M op_mmmul( const M& l, const M& r )
{
    M o;
    DL_MATH_FOR_ij( 0, N )
    {
        S tmp = ( S )0;
        DL_MATH_FOR_k( 0, N ) { tmp += l.m[k*N+j] * r.m[i*N+k]; }
        o.m[i*N+j] = tmp;
    }
    return o;
}

// Matrix-scalar arithmetic.
//
template<Dim N, typename S, typename M> static DL_MATH_INL M op_msadd( const M& m, S s ) { M o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = m.comp[i] + s; } return o; }
template<Dim N, typename S, typename M> static DL_MATH_INL M op_mssub( const M& m, S s ) { M o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = m.comp[i] - s; } return o; }
template<Dim N, typename S, typename M> static DL_MATH_INL M op_msmul( const M& m, S s ) { M o; DL_MATH_FOR_i( 0, N ) { o.comp[i] = m.comp[i] * s; } return o; }
template<Dim N, typename S, typename M> static DL_MATH_INL M op_msdiv( const M& m, S s ) { DL_MATH_CHECKZERO( s ); return op_msmul<N, S>( m, ( S )1 / s ); }

template<Dim N, typename T> DL_MATH_INL MatT<N, Int32> op_msdiv( const MatT<N, Int32>& m, Int32 s ) { return ( MatT<N, Int32> )( ( MatT<N, Double> )m / ( Double )s ); }

// MatT * 3-tuple -> 3-tuple (no translation, M is treated as a 3x3 matrix, regardless).
//
template<typename M, typename T3> static DL_MATH_INL T3 op_mt3mul( const M& m, T3 t )
{
    T3 o;
    o.x = m.m00*t.x + m.m10*t.y + m.m20*t.z;
    o.y = m.m01*t.x + m.m11*t.y + m.m21*t.z;
    o.z = m.m02*t.x + m.m12*t.y + m.m22*t.z;
    return o;
}

// Mat4 * Pos3 -> Pos3 (result includes any translation in m).
//
template<typename M4, typename P3> static DL_MATH_INL P3 op_m4p3mul( const M4& m, P3 p )
{
    P3 o;
    o.x = m.m00*p.x + m.m10*p.y + m.m20*p.z + m.m30;
    o.y = m.m01*p.x + m.m11*p.y + m.m21*p.z + m.m31;
    o.z = m.m02*p.x + m.m12*p.y + m.m22*p.z + m.m32;
    return o;
}

// Mat3 * Vec4 -> Vec4.
//
template<typename M3, typename V4> static DL_MATH_INL V4 op_m3v4mul( const M3& m, V4 v )
{
    V4 o;
    o.x = m.m00*v.x + m.m10*v.y + m.m20*v.z;
    o.y = m.m01*v.x + m.m11*v.y + m.m21*v.z;
    o.z = m.m02*v.x + m.m12*v.y + m.m22*v.z;
    o.w = v.w;
    return o;
}

// Mat4 * Vec4 -> Vec4.
//
template<typename M4, typename V4> static DL_MATH_INL V4 op_m4v4mul( const M4& m, V4 v )
{
    V4 o;
    o.x = m.m00*v.x + m.m10*v.y + m.m20*v.z + m.m30*v.w;
    o.y = m.m01*v.x + m.m11*v.y + m.m21*v.z + m.m31*v.w;
    o.z = m.m02*v.x + m.m12*v.y + m.m22*v.z + m.m32*v.w;
    o.w = m.m03*v.x + m.m13*v.y + m.m23*v.z + m.m33*v.w;
    return o;
}

// Mat4 * Mat3 -> Mat4.
//
template<typename M4, typename M3> static DL_MATH_INL M4 op_m43mul( const M4& l, const M3& r )
{
    M4 o;
    o.m00 = l.m00*r.m00 + l.m10*r.m01 + l.m20*r.m02;
    o.m10 = l.m00*r.m10 + l.m10*r.m11 + l.m20*r.m12;
    o.m20 = l.m00*r.m20 + l.m10*r.m21 + l.m20*r.m22;
    o.m30 = l.m30;
    o.m01 = l.m01*r.m00 + l.m11*r.m01 + l.m21*r.m02;
    o.m11 = l.m01*r.m10 + l.m11*r.m11 + l.m21*r.m12;
    o.m21 = l.m01*r.m20 + l.m11*r.m21 + l.m21*r.m22;
    o.m31 = l.m31;
    o.m02 = l.m02*r.m00 + l.m12*r.m01 + l.m22*r.m02;
    o.m12 = l.m02*r.m10 + l.m12*r.m11 + l.m22*r.m12;
    o.m22 = l.m02*r.m20 + l.m12*r.m21 + l.m22*r.m22;
    o.m32 = l.m32;
    o.m03 = l.m03*r.m00 + l.m13*r.m01 + l.m23*r.m02;
    o.m13 = l.m03*r.m10 + l.m13*r.m11 + l.m23*r.m12;
    o.m23 = l.m03*r.m20 + l.m13*r.m21 + l.m23*r.m22;
    o.m33 = l.m33;
    return o;
}

// Mat3 * Mat4 -> Mat4.
//
template<typename M3, typename M4> static DL_MATH_INL M4 op_m34mul( const M3& l, const M4& r )
{
    M4 o;
    o.m00 = l.m00*r.m00 + l.m10*r.m01 + l.m20*r.m02;
    o.m10 = l.m00*r.m10 + l.m10*r.m11 + l.m20*r.m12;
    o.m20 = l.m00*r.m20 + l.m10*r.m21 + l.m20*r.m22;
    o.m30 = l.m00*r.m30 + l.m10*r.m31 + l.m20*r.m32;
    o.m01 = l.m01*r.m00 + l.m11*r.m01 + l.m21*r.m02;
    o.m11 = l.m01*r.m10 + l.m11*r.m11 + l.m21*r.m12;
    o.m21 = l.m01*r.m20 + l.m11*r.m21 + l.m21*r.m22;
    o.m31 = l.m01*r.m30 + l.m11*r.m31 + l.m21*r.m32;
    o.m02 = l.m02*r.m00 + l.m12*r.m01 + l.m22*r.m02;
    o.m12 = l.m02*r.m10 + l.m12*r.m11 + l.m22*r.m12;
    o.m22 = l.m02*r.m20 + l.m12*r.m21 + l.m22*r.m22;
    o.m32 = l.m02*r.m30 + l.m12*r.m31 + l.m22*r.m32;
    o.m03 = r.m03;
    o.m13 = r.m13;
    o.m23 = r.m23;
    o.m33 = r.m33;
    return o;
}

// Compute a quaternion representing the rotation of matrix m.
//
template<typename S, typename Q, typename M> static DL_MATH_INL Q op_m2q( const M& m )
{
    Q q;
    if ( m.m00 + m.m11 + m.m22 > ( S )0 )
    {
        S t = +m.m00 + m.m11 + m.m22 + ( S )1;
        S s = sqrt( t ) * ( S )0.5;
        q.w = s * t;
        q.x = ( m.m12 - m.m21 ) * s;
        q.y = ( m.m20 - m.m02 ) * s;
        q.z = ( m.m01 - m.m10 ) * s;
    }
    else if ( m.m00 > m.m11 && m.m00 > m.m22 )
    {
        S t = +m.m00 - m.m11 - m.m22 + ( S )1;
        S s = sqrt( t ) * ( S )0.5;
        q.w = ( m.m12 - m.m21 ) * s;
        q.x = s * t;
        q.y = ( m.m01 + m.m10 ) * s;
        q.z = ( m.m20 + m.m02 ) * s;
    }
    else if ( m.m11 > m.m22 )
    {
        S t = -m.m00 + m.m11 - m.m22 + ( S )1;
        S s = sqrt( t ) * ( S )0.5;
        q.w = ( m.m20 - m.m02 ) * s;
        q.x = ( m.m01 + m.m10 ) * s;
        q.y = s * t;
        q.z = ( m.m12 + m.m21 ) * s;
    }
    else
    {
        S t = -m.m00 - m.m11 + m.m22 + ( S )1;
        S s = sqrt( t ) * ( S )0.5;
        q.w = ( m.m01 - m.m10 ) * s;
        q.y = ( m.m12 + m.m21 ) * s;
        q.x = ( m.m20 + m.m02 ) * s;
        q.z = s * t;
    }
    return unit( q );
}

// Compute a matrix based on the rotation described by quaternion q.
//
template<typename S, typename M, typename Q> static DL_MATH_INL M op_q2m( Q q )
{
    q = unit( q );
    const S x = ( S )2 * q.x;
    const S y = ( S )2 * q.y;
    const S z = ( S )2 * q.z;
    const S wx = x * q.w;
    const S wy = y * q.w;
    const S wz = z * q.w;
    const S xx = x * q.x;
    const S xy = y * q.x;
    const S xz = z * q.x;
    const S yy = y * q.y;
    const S yz = z * q.y;
    const S zz = z * q.z;

    M o = M::identity;
    o.m00 = ( S )1 - ( yy + zz );
    o.m01 = xy + wz;
    o.m02 = xz - wy;
    o.m10 = xy - wz;
    o.m11 = ( S )1 - ( xx + zz );
    o.m12 = yz + wx;
    o.m20 = xz + wy;
    o.m21 = yz - wx;
    o.m22 = ( S )1 - ( xx + yy );
    return o;
}

// Compute the trace for matrix m, which is expected to have an N*N buffer of type S named 'm'.
//
template<Dim N, typename S, typename M> static DL_MATH_INL S trace( const M& m )
{
    S o = ( S )0;
    for ( Dim i = 0; i < N*N; i += ( N+1 ) )
        o += m.m[i];
    return o;
}

// Retrieve matrix diagonal.
//
template<Dim N, typename S, typename M> static DL_MATH_INL VecT<N, S> diagonal( const M& m )
{
    typename M::V o = { 0 };
    DL_MATH_FOR_ij( 0, N ) { if ( i==j ) { o.comp[i] = m.comp[i].comp[i]; } }
    return o;
}

// Compute the x/y/z scale factors for 3x3 matrix M. Note that this will fail for matrices which
// contain shear, or which have had rotation applied after scaling.
//
template<typename S, typename V3> static DL_MATH_INL V3 scale( const MatT<3, S>& m )
{
    return V3
    {
        norm( m.c0 )* ( ( m.m00* m.m01 * m.m02 ) < ( S )0 ? ( S )-1 : ( S )1 ),
        norm( m.c1 )* ( ( m.m10* m.m11 * m.m12 ) < ( S )0 ? ( S )-1 : ( S )1 ),
        norm( m.c2 )* ( ( m.m20* m.m21 * m.m22 ) < ( S )0 ? ( S )-1 : ( S )1 )
    };
}

// Compute x/y/z scale factors for 4x4 matrix M, multiplying by any scale factor placed in the
// lower-right (the last) element. Note that this will fail for matrices which contain shear, or
// which have had rotation applied after scaling.
//
template<typename S, typename V3> static DL_MATH_INL V3 scale( const MatT<4, S>& m )
{
    return m.m33 * V3
    {
        norm( m.c0.v3 )* ( ( m.m00* m.m01* m.m02 * m.m03 ) < ( S )0 ? ( S )-1 : ( S )1 ),
        norm( m.c1.v3 )* ( ( m.m10* m.m11* m.m12 * m.m13 ) < ( S )0 ? ( S )-1 : ( S )1 ),
        norm( m.c2.v3 )* ( ( m.m20* m.m21* m.m22 * m.m23 ) < ( S )0 ? ( S )-1 : ( S )1 )
    };
}

// Compute the pure 3x3 rotation matrix for M. Note that this will fail for matrices which
// contain shear, or which have had rotation applied after scaling.
//
template<typename S, typename M3, typename M> static DL_MATH_INL M3 rotation( const M& m )
{
    auto s = scale( m );

    return ( math::zerolike( s.x ) || math::zerolike( s.y ) || math::zerolike( s.z ) )
           ? M3::identity
           : M3 { m.m00 / s.x, m.m01 / s.x, m.m02 / s.x,
                  m.m10 / s.y, m.m11 / s.y, m.m12 / s.y,
                  m.m20 / s.z, m.m21 / s.z, m.m22 / s.z };
}

// Get euler angles for a matrix. Expects M to be 3x3 or better, with elements m00, m01, etc.
//
template<typename S, typename V3, typename M> static DL_MATH_INL V3 eulers( const M& m )
{
    V3 o;
    if ( m.m02 < ( S )-0.999 )
    {
        o.z = 0;
        o.x = ( S )atan2( m.m10, m.m20 );
        o.y = ( S )nc::halfPi;
    }
    else if ( m.m02 > ( S )0.999 )
    {
        o.z = 0;
        o.x = ( S )atan2( -m.m10, -m.m20 );
        o.y = ( S )-nc::halfPi;
    }
    else
    {
        S ry = ( S )-asin( m.m02 );
        S i = ( S )1 / op::sqrt( ( S )1 - m.m02*m.m02 );
        o.z = ( S )atan2( m.m01 * i, m.m00 * i );
        o.x = ( S )atan2( m.m12 * i, m.m22 * i );
        o.y = ry;
    }
    return o;
}

// Transpose a matrix. Expects M type to have an N*N buffer of S named 'm'.
//
template<Dim N, typename S, typename M> static DL_MATH_INL M transpose( const M& m )
{
    M o;
    DL_MATH_FOR_ij( 0, N ) { o.m[i*N+j] = m.m[j*N+i]; }
    return o;
}

// Compute determinant for a 3x3 matrix with elements m00, m01, etc.
//
template<typename S, typename M3> static DL_MATH_INL S determinantM3( const M3& m )
{
    return m.m00 * ( m.m11*m.m22 - m.m21*m.m12 ) -
           m.m10 * ( m.m01*m.m22 - m.m21*m.m02 ) +
           m.m20 * ( m.m01*m.m12 - m.m11*m.m02 );
}

// Compute determinant for a 4x4 matrix with elements m00, m01, etc.
//
template<typename S, typename M4> static DL_MATH_INL S determinantM4( const M4& m )
{
    return m.m03*m.m12*m.m21*m.m30 - m.m02*m.m13*m.m21*m.m30 - m.m03*m.m11*m.m22*m.m30 + m.m01*m.m13*m.m22*m.m30 +
           m.m02*m.m11*m.m23*m.m30 - m.m01*m.m12*m.m23*m.m30 - m.m03*m.m12*m.m20*m.m31 + m.m02*m.m13*m.m20*m.m31 +
           m.m03*m.m10*m.m22*m.m31 - m.m00*m.m13*m.m22*m.m31 - m.m02*m.m10*m.m23*m.m31 + m.m00*m.m12*m.m23*m.m31 +
           m.m03*m.m11*m.m20*m.m32 - m.m01*m.m13*m.m20*m.m32 - m.m03*m.m10*m.m21*m.m32 + m.m00*m.m13*m.m21*m.m32 +
           m.m01*m.m10*m.m23*m.m32 - m.m00*m.m11*m.m23*m.m32 - m.m02*m.m11*m.m20*m.m33 + m.m01*m.m12*m.m20*m.m33 +
           m.m02*m.m10*m.m21*m.m33 - m.m00*m.m12*m.m21*m.m33 - m.m01*m.m10*m.m22*m.m33 + m.m00*m.m11*m.m22*m.m33;
}

// Compute the inverse of a 3x3 matrix with elements m00, m01, etc.
//
template<typename S, typename M3> static DL_MATH_INL M3 inverseM3( const M3& m )
{
    S det = op::determinantM3<S>( m );
    S invdet = ( S )1 / ( math::zerolike<S>( det ) ? math::epsilon<S>() : det );
    M3 o;
    o.m00 = ( m.m11*m.m22 - m.m12*m.m21 ) * invdet;
    o.m01 = ( m.m02*m.m21 - m.m01*m.m22 ) * invdet;
    o.m02 = ( m.m01*m.m12 - m.m02*m.m11 ) * invdet;
    o.m10 = ( m.m12*m.m20 - m.m10*m.m22 ) * invdet;
    o.m11 = ( m.m00*m.m22 - m.m02*m.m20 ) * invdet;
    o.m12 = ( m.m02*m.m10 - m.m00*m.m12 ) * invdet;
    o.m20 = ( m.m10*m.m21 - m.m11*m.m20 ) * invdet;
    o.m21 = ( m.m01*m.m20 - m.m00*m.m21 ) * invdet;
    o.m22 = ( m.m00*m.m11 - m.m01*m.m10 ) * invdet;
    return o;
}

// Compute the inverse of a 4x4 matrix with elements m00, m01, etc.
//
template<typename S, typename M4> static DL_MATH_INL M4 inverseM4( const M4& m )
{
    S det = op::determinantM4<S>( m );
    S invdet = ( S )1 / ( math::zerolike<S>( det ) ? math::epsilon<S>() : det );
    M4 o;
    o.m00 = ( m.m12*m.m23*m.m31 - m.m13*m.m22*m.m31 + m.m13*m.m21*m.m32 - m.m11*m.m23*m.m32 - m.m12*m.m21*m.m33 + m.m11*m.m22*m.m33 ) * invdet;
    o.m01 = ( m.m03*m.m22*m.m31 - m.m02*m.m23*m.m31 - m.m03*m.m21*m.m32 + m.m01*m.m23*m.m32 + m.m02*m.m21*m.m33 - m.m01*m.m22*m.m33 ) * invdet;
    o.m02 = ( m.m02*m.m13*m.m31 - m.m03*m.m12*m.m31 + m.m03*m.m11*m.m32 - m.m01*m.m13*m.m32 - m.m02*m.m11*m.m33 + m.m01*m.m12*m.m33 ) * invdet;
    o.m03 = ( m.m03*m.m12*m.m21 - m.m02*m.m13*m.m21 - m.m03*m.m11*m.m22 + m.m01*m.m13*m.m22 + m.m02*m.m11*m.m23 - m.m01*m.m12*m.m23 ) * invdet;
    o.m10 = ( m.m13*m.m22*m.m30 - m.m12*m.m23*m.m30 - m.m13*m.m20*m.m32 + m.m10*m.m23*m.m32 + m.m12*m.m20*m.m33 - m.m10*m.m22*m.m33 ) * invdet;
    o.m11 = ( m.m02*m.m23*m.m30 - m.m03*m.m22*m.m30 + m.m03*m.m20*m.m32 - m.m00*m.m23*m.m32 - m.m02*m.m20*m.m33 + m.m00*m.m22*m.m33 ) * invdet;
    o.m12 = ( m.m03*m.m12*m.m30 - m.m02*m.m13*m.m30 - m.m03*m.m10*m.m32 + m.m00*m.m13*m.m32 + m.m02*m.m10*m.m33 - m.m00*m.m12*m.m33 ) * invdet;
    o.m13 = ( m.m02*m.m13*m.m20 - m.m03*m.m12*m.m20 + m.m03*m.m10*m.m22 - m.m00*m.m13*m.m22 - m.m02*m.m10*m.m23 + m.m00*m.m12*m.m23 ) * invdet;
    o.m20 = ( m.m11*m.m23*m.m30 - m.m13*m.m21*m.m30 + m.m13*m.m20*m.m31 - m.m10*m.m23*m.m31 - m.m11*m.m20*m.m33 + m.m10*m.m21*m.m33 ) * invdet;
    o.m21 = ( m.m03*m.m21*m.m30 - m.m01*m.m23*m.m30 - m.m03*m.m20*m.m31 + m.m00*m.m23*m.m31 + m.m01*m.m20*m.m33 - m.m00*m.m21*m.m33 ) * invdet;
    o.m22 = ( m.m01*m.m13*m.m30 - m.m03*m.m11*m.m30 + m.m03*m.m10*m.m31 - m.m00*m.m13*m.m31 - m.m01*m.m10*m.m33 + m.m00*m.m11*m.m33 ) * invdet;
    o.m23 = ( m.m03*m.m11*m.m20 - m.m01*m.m13*m.m20 - m.m03*m.m10*m.m21 + m.m00*m.m13*m.m21 + m.m01*m.m10*m.m23 - m.m00*m.m11*m.m23 ) * invdet;
    o.m30 = ( m.m12*m.m21*m.m30 - m.m11*m.m22*m.m30 - m.m12*m.m20*m.m31 + m.m10*m.m22*m.m31 + m.m11*m.m20*m.m32 - m.m10*m.m21*m.m32 ) * invdet;
    o.m31 = ( m.m01*m.m22*m.m30 - m.m02*m.m21*m.m30 + m.m02*m.m20*m.m31 - m.m00*m.m22*m.m31 - m.m01*m.m20*m.m32 + m.m00*m.m21*m.m32 ) * invdet;
    o.m32 = ( m.m02*m.m11*m.m30 - m.m01*m.m12*m.m30 - m.m02*m.m10*m.m31 + m.m00*m.m12*m.m31 + m.m01*m.m10*m.m32 - m.m00*m.m11*m.m32 ) * invdet;
    o.m33 = ( m.m01*m.m12*m.m20 - m.m02*m.m11*m.m20 + m.m02*m.m10*m.m21 - m.m00*m.m12*m.m21 - m.m01*m.m10*m.m22 + m.m00*m.m11*m.m22 ) * invdet;
    return o;
}

// Check whether matrix m is the zero matrix.
//
template<Dim N, typename S, typename M> static DL_MATH_INL Bool isZero( const M& m )
{
    return m == M::zero;
}

// Check whether matrix m is the identity matrix.
//
template<Dim N, typename S, typename M> static DL_MATH_INL Bool isIdentity( const M& m )
{
    return m == M::identity;
}

// Check whether the determinant of m is zero, indicating a degenerate, non-invertible matrix.
//
template<typename S, typename M> static DL_MATH_INL Bool isSingularM3( const M& m )
{
    return equal( determinantM3<S>( m ), ( S )0 );
}
template<typename S, typename M> static DL_MATH_INL Bool isSingularM4( const M& m )
{
    return equal( determinantM4<S>( m ), ( S )0 );
}

// Check whether the determinant of m is -1, indicating a reflection.
//
template<typename S, typename M> static DL_MATH_INL Bool isReflectionM3( const M& m )
{
    return determinantM3<S>( m ) < ( S )0;
}
template<typename S, typename M> static DL_MATH_INL Bool isReflectionM4( const M& m )
{
    return determinantM4<S>( m ) < ( S )0;
}

// Check whether (the upper-left 3x3) columns of matrix m are of unit length.
//
template<Dim N, typename S, typename M> static DL_MATH_INL Bool isNormalized( const M& m )
{
    return isUnit( m.c0 ) && isUnit( m.c1 ) && isUnit( m.c2 );
}

// Check whether (the upper-left 3x3 of) matrix m is orthogonal.
//
template<Dim N, typename S, typename M> static DL_MATH_INL Bool isOrtho( const M& m )
{
    return !equal( op::normSq<3, S>( m.c0 ), ( S )0 ) &&
           !equal( op::normSq<3, S>( m.c1 ), ( S )0 ) &&
           !equal( op::normSq<3, S>( m.c2 ), ( S )0 ) &&
            equal( op::dot<3, S>( m.c0, m.c1 ), ( S )0 ) &&
            equal( op::dot<3, S>( m.c0, m.c2 ), ( S )0 ) &&
            equal( op::dot<3, S>( m.c1, m.c2 ), ( S )0 );
}

// Check whether (the upper-left 3x3 of) matrix m is both orthogonal and normalized.
//
template<Dim N, typename S, typename M> static DL_MATH_INL Bool isOrthoNormal( const M& m )
{
    return op::isOrtho<N, S>( m ) && op::isNormalized<N, S>( m );
}

// Check whether a 4x4 matrix (with elements m00, m01, etc) has column "w" values of 0,0,0,1.
//
template<typename S, typename M4> static DL_MATH_INL Bool isAffine( const M4& m )
{
    return equal( m.m03, ( S )0 ) &&
           equal( m.m13, ( S )0 ) &&
           equal( m.m23, ( S )0 ) &&
           equal( m.m33, ( S )1 );
}

// Check whether a 4x4 matrix has any non-zero translation (m30, m31, m32) elements.
//
template<typename S, typename M4> static DL_MATH_INL Bool isTranslation( const M4& m )
{
    return !equal( m.m30, ( S )0 ) ||
           !equal( m.m31, ( S )0 ) ||
           !equal( m.m32, ( S )0 );
}

// Get the translation of a 4x4 matrix (i.e. a vector with m30, m31, and m32).
//
template<typename S, typename V3, typename M4> static DL_MATH_INL V3 translation( const M4& m )
{
    V3 o;
    o.x = m.m30;
    o.y = m.m31;
    o.z = m.m32;
    return o;
}

// Create a matrix that scales according to the given x, y, and z factors.
//
template<typename S, typename M> static DL_MATH_INL M makeScale( S x, S y, S z )
{
    M o = M::identity;
    o.m00 = x;
    o.m11 = y;
    o.m22 = z;
    return o;
}

// Create a rotation matrix that rotates r (radians) about the X axis.
//
template<typename S, typename M> static DL_MATH_INL M makeRotationX( S r )
{
    S c = ( S )cos( r );
    S s = ( S )sin( r );
    M o = M::identity;
    o.m00 = 1; o.m01 =  0; o.m02 = 0 ;
    o.m10 = 0; o.m11 =  c; o.m12 = s;
    o.m20 = 0; o.m21 = -s; o.m22 = c;
    return o;
}

// Create a rotation matrix that rotates r (radians) about the Y axis.
//
template<typename S, typename M> static DL_MATH_INL M makeRotationY( S r )
{
    S c = ( S )cos( r );
    S s = ( S )sin( r );
    M o = M::identity;
    o.m00 = c; o.m01 = 0; o.m02 = -s;
    o.m10 = 0; o.m11 = 1; o.m12 =  0;
    o.m20 = s; o.m21 = 0; o.m22 =  c;
    return o;
}

// Create a rotation matrix that rotates r (radians) about the Z axis.
//
template<typename S, typename M> static DL_MATH_INL M makeRotationZ( S r )
{
    S c = ( S )cos( r );
    S s = ( S )sin( r );
    M o = M::identity;
    o.m00 =  c; o.m01 = s; o.m02 = 0;
    o.m10 = -s; o.m11 = c; o.m12 = 0;
    o.m20 =  0; o.m21 = 0; o.m22 = 1;
    return o;
}

// Create a rotation matrix that rotates by x, y, and z (in radians), in the specified order.
//
template<typename S, typename M> static DL_MATH_INL M makeRotation( S x, S y, S z, const char* order = "xyz" )
{
    M mx = op::makeRotationX<S, M>( x );
    M my = op::makeRotationY<S, M>( y );
    M mz = op::makeRotationZ<S, M>( z );
    if ( order && strcmp( order, "xzy" ) == 0 ) return my * mz * mx;
    if ( order && strcmp( order, "yxz" ) == 0 ) return mz * mx * my;
    if ( order && strcmp( order, "yzx" ) == 0 ) return mx * mz * my;
    if ( order && strcmp( order, "zxy" ) == 0 ) return my * mx * mz;
    if ( order && strcmp( order, "zyx" ) == 0 ) return mx * my * mz;
    return mz * my * mx;
}

// Create a 4x4 matrix with translation in m30, m31, and m32.
//
template<typename S, typename M4> static DL_MATH_INL M4 makeTranslation( S x, S y, S z )
{
    M4 o = M4::identity;
    o.m30 = x;
    o.m31 = y;
    o.m32 = z;
    return o;
}

// Compute hue from rgb values.
//
template<typename S> static DL_MATH_INL S hue( RgbT<S> v )
{
    S r = v.r;
    S g = v.g;
    S b = v.b;

    S _max = v.max();
    S _min = v.min();
    S c = _max - _min;

    S h = 0;

    if ( !math::zerolike( c ) )
    {
        if ( math::equal( _max, r ) )
        {
            h = 60 * ( ( g-b ) / c );
        }
        else if ( math::equal( _max, g ) )
        {
            h = 60 * ( ( b-r ) / c + 2 );
        }
        else
        {
            h = 60 * ( ( r-g ) / c + 4 );
        }
    }

    return h < 0 ? h + 360 : h;
}

template<typename S> static DL_MATH_INL S hue( RgbaT<S> v )
{
    return hue( ( RgbT<S> )v );
}

// Rgb from hue, chroma, offset.
//
template<typename S> static DL_MATH_INL RgbT<S> hco_to_rgb( S h, S c, S o )
{
    S r = 0, g = 0, b = 0;

    if ( !math::zerolike( c ) )
    {
        h /= 60;
        S x = c * ( ( S )1 - fabs( fmod( h, ( S )2 ) - ( S )1 ) );

        if ( h < 1 )
        {
            r = c;
            g = x;
        }
        else if ( h < 2 )
        {
            r = x;
            g = c;
        }
        else if ( h < 3 )
        {
            g = c;
            b = x;
        }
        else if ( h < 4 )
        {
            g = x;
            b = c;
        }
        else if ( h < 5 )
        {
            r = x;
            b = c;
        }
        else
        {
            r = c;
            b = x;
        }
    }

    return RgbT<S> { r+o, g+o, b+o };
}

// Compute hsl from rgb values.
//
template<typename S> static DL_MATH_INL RgbT<S> op_hsltorgb( HslT<S> hsl )
{
    S h = hsl.h;
    S s = hsl.s;
    S l = hsl.l;
    S c = s * ( ( S )1 - fabs( ( l * ( S )2 ) - ( S )1 ) );
    return hco_to_rgb( h, c, l - ( c / 2 ) );
}

template<typename S> static DL_MATH_INL RgbaT<S> op_hsltorgba( HslT<S> hsl )
{
    return ( RgbaT<S> )op_hsltorgb( hsl );
}

// Compute hsv from rgb values.
//
template<typename S> static DL_MATH_INL RgbT<S> op_hsvtorgb( HsvT<S> hsv )
{
    S h = hsv.h;
    S s = hsv.s;
    S v = hsv.v;
    S c = s * v;
    return hco_to_rgb( h, c, v - c );
}

template<typename S> static DL_MATH_INL RgbaT<S> op_hsvtorgba( HsvT<S> hsv )
{
    return ( RgbaT<S> )op_hsvtorgb( hsv );
}

// Compute hsl from rgb values.
//
template<typename S> static DL_MATH_INL HslT<S> op_rgbtohsl( RgbT<S> t )
{
    S h = hue<S>( t );
    S _max = t.max();
    S _min = t.min();

    S l = ( _max + _min ) / 2;
    S c = _max - _min;
    S s = 0;

    if ( !math::zerolike( c ) )
    {
        s = l > 0 ? c / ( 2 - ( l * 2 ) ) : c / ( l * 2 );
    }

    return HslT<S> { h, s, l };
}

template<typename S> static DL_MATH_INL HslT<S> op_rgbtohsl( RgbaT<S> t )
{
    return op_rgbtohsl( ( RgbT<S> )t );
}

// Compute hsv from rgb values.
//
template<typename S> static DL_MATH_INL HsvT<S> op_rgbtohsv( RgbT<S> t )
{
    S h = hue( t );
    S v = t.max();
    S c = v - t.min();
    S s = math::zerolike( c ) ? 0 : c / v;

    return HsvT<S> { h, s, v };
}

template<typename S> static DL_MATH_INL HsvT<S> op_rgbtohsv( RgbaT<S> t )
{
    return op_rgbtohsv( ( RgbT<S> )t );
}

} // op

//=================================================================================================
// Stream insertion operators.
//=================================================================================================

template<Dim N, typename S, typename Stream> static DL_MATH_INL Stream& operator<<( Stream& s, const VecT<N, S>& v ) { return op::op_tostream<N, VecT<N, S>, Stream>( s, v ); }
template<Dim N, typename S, typename Stream> static DL_MATH_INL Stream& operator<<( Stream& s, const PosT<N, S>& p ) { return op::op_tostream<N, PosT<N, S>, Stream>( s, p ); }
template<       typename S, typename Stream> static DL_MATH_INL Stream& operator<<( Stream& s, const QuatT<S>& q ) { return op::op_tostream<4, QuatT<S>, Stream>( s, q ); }
template<Dim N, typename S, typename Stream> static DL_MATH_INL Stream& operator<<( Stream& s, const MatT<N, S>& m ) { return op::op_tostream<N, MatT<N, S>, Stream>( s, m ); }

template<       typename S, typename Stream> static DL_MATH_INL Stream& operator<<( Stream& s, const RgbT<S>& c ) { return op::op_tostream<3, RgbT<S>, Stream>( s, c ); }
template<       typename S, typename Stream> static DL_MATH_INL Stream& operator<<( Stream& s, const RgbaT<S>& c ) { return op::op_tostream<4, RgbaT<S>, Stream>( s, c ); }
template<       typename S, typename Stream> static DL_MATH_INL Stream& operator<<( Stream& s, const HsvT<S>& c ) { return op::op_tostream<3, HsvT<S>, Stream>( s, c ); }
template<       typename S, typename Stream> static DL_MATH_INL Stream& operator<<( Stream& s, const HslT<S>& c ) { return op::op_tostream<3, HslT<S>, Stream>( s, c ); }

template<Dim N, typename S, typename Stream> static DL_MATH_INL Stream& operator>>( Stream& s, VecT<N, S>& v ) { return op::op_fromstream<N, VecT<N, S>, Stream>( s, v ); }
template<Dim N, typename S, typename Stream> static DL_MATH_INL Stream& operator>>( Stream& s, PosT<N, S>& p ) { return op::op_fromstream<N, PosT<N, S>, Stream>( s, p ); }
template<       typename S, typename Stream> static DL_MATH_INL Stream& operator>>( Stream& s, QuatT<S>& q ) { return op::op_fromstream<4, QuatT<S>, Stream>( s, q ); }
template<Dim N, typename S, typename Stream> static DL_MATH_INL Stream& operator>>( Stream& s, MatT<N, S>& m ) { return op::op_fromstream<N, MatT<N, S>, Stream>( s, m ); }

template<       typename S, typename Stream> static DL_MATH_INL Stream& operator>>( Stream& s, RgbT<S>& c ) { return op::op_fromstream<3, RgbT<S>, Stream>( s, c ); }
template<       typename S, typename Stream> static DL_MATH_INL Stream& operator>>( Stream& s, RgbaT<S>& c ) { return op::op_fromstream<4, RgbaT<S>, Stream>( s, c ); }
template<       typename S, typename Stream> static DL_MATH_INL Stream& operator>>( Stream& s, HsvT<S>& c ) { return op::op_fromstream<3, HsvT<S>, Stream>( s, c ); }
template<       typename S, typename Stream> static DL_MATH_INL Stream& operator>>( Stream& s, HslT<S>& c ) { return op::op_fromstream<3, HslT<S>, Stream>( s, c ); }

//=================================================================================================
// Equality.
//=================================================================================================

template<Dim N, typename S> static DL_MATH_INL Bool            operator==( VecT<N, S> l, VecT<N, S> r ) { return op::op_equal<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator==( PosT<N, S> l, PosT<N, S> r ) { return op::op_equal<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator==( VecT<N, S> l, PosT<N, S> r ) { return op::op_equal<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator==( PosT<N, S> l, VecT<N, S> r ) { return op::op_equal<N>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator==( QuatT<S> l, QuatT<S> r ) { return op::op_equal<4>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator==( const MatT<N, S>& l, const MatT<N, S>& r ) { return op::op_equal<N>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator==( const XformT<S>& l, const XformT<S>& r ) { return l.position==r.position && l.rotation==r.rotation && l.scale==r.scale; }
template<Dim N, typename S> static DL_MATH_INL Bool            operator==( const XformT<S>& l, const MatT<N, S>& r ) { return l == XformT<S>::make( r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator==( const MatT<N, S>& l, const XformT<S>& r ) { return XformT<S>::make( l ) == r; }
template<       typename S> static DL_MATH_INL Bool            operator==( const AabbT<S>& l, const AabbT<S>& r ) { return l.min == r.min && l.max == l.max; }
template<       typename S> static DL_MATH_INL Bool            operator==( const ObbT<S>& l, const ObbT<S>& r ) { return l.frame == r.frame && l.size == l.size; }

template<       typename S> static DL_MATH_INL Bool            operator==( RgbT<S> l, RgbT<S> r ) { return op::op_equal<3>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator==( RgbaT<S> l, RgbaT<S> r ) { return op::op_equal<4>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator==( HsvT<S> l, HsvT<S> r ) { return op::op_equal<3>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator==( HslT<S> l, HslT<S> r ) { return op::op_equal<3>( l, r ); }

template<Dim N, typename S> static DL_MATH_INL Bool            operator!=( VecT<N, S> l, VecT<N, S> r ) { return !( l == r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator!=( PosT<N, S> l, PosT<N, S> r ) { return !( l == r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator!=( VecT<N, S> l, PosT<N, S> r ) { return !( l == r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator!=( PosT<N, S> l, VecT<N, S> r ) { return !( l == r ); }
template<       typename S> static DL_MATH_INL Bool            operator!=( QuatT<S> l, QuatT<S> r ) { return !( l == r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator!=( const MatT<N, S>& l, const MatT<N, S>& r ) { return !( l == r ); }
template<       typename S> static DL_MATH_INL Bool            operator!=( const XformT<S>& l, const XformT<S>& r ) { return !( l == r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator!=( const XformT<S>& l, const MatT<N, S>& r ) { return !( l == r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator!=( const MatT<N, S>& l, const XformT<S>& r ) { return !( l == r ); }
template<       typename S> static DL_MATH_INL Bool            operator!=( const AabbT<S>& l, const AabbT<S>& r ) { return !( l == r ); }
template<       typename S> static DL_MATH_INL Bool            operator!=( const ObbT<S>& l, const ObbT<S>& r ) { return !( l == r ); }

template<       typename S> static DL_MATH_INL Bool            operator!=( RgbT<S> l, RgbT<S> r ) { return !( l == r ); }
template<       typename S> static DL_MATH_INL Bool            operator!=( RgbaT<S> l, RgbaT<S> r ) { return !( l == r ); }
template<       typename S> static DL_MATH_INL Bool            operator!=( HsvT<S> l, HsvT<S> r ) { return !( l == r ); }
template<       typename S> static DL_MATH_INL Bool            operator!=( HslT<S> l, HslT<S> r ) { return !( l == r ); }

//=================================================================================================
// Comparison.
//=================================================================================================

template<Dim N, typename S> static DL_MATH_INL Bool            operator<( VecT<N, S> l, VecT<N, S> r ) { return op::op_less<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator<( PosT<N, S> l, PosT<N, S> r ) { return op::op_less<N>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator<( QuatT<S> l, QuatT<S> r ) { return op::op_less<4>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator<( const MatT<N, S>& l, const MatT<N, S>& r ) { return op::op_less<N>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator<( const XformT<S>& l, const XformT<S>& r ) { return op::op_less<4>( ( MatT<4, S> )l, ( MatT<4, S> )r ); }
template<       typename S> static DL_MATH_INL Bool            operator<( const AabbT<S>& l, const AabbT<S>& r ) { return ( l.max-l.min ) < ( r.max-r.min ); }
template<       typename S> static DL_MATH_INL Bool            operator<( const ObbT<S>& l, const ObbT<S>& r ) { return l.size < r.size; }

template<       typename S> static DL_MATH_INL Bool            operator<( RgbT<S> l, RgbT<S> r ) { return op::op_less<3>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator<( RgbaT<S> l, RgbaT<S> r ) { return op::op_less<4>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator<( HsvT<S> l, HsvT<S> r ) { return op::op_less<3>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator<( HslT<S> l, HslT<S> r ) { return op::op_less<3>( l, r ); }

template<Dim N, typename S> static DL_MATH_INL Bool            operator>( VecT<N, S> l, VecT<N, S> r ) { return op::op_greater<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator>( PosT<N, S> l, PosT<N, S> r ) { return op::op_greater<N>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator>( QuatT<S> l, QuatT<S> r ) { return op::op_greater<4>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator>( const MatT<N, S>& l, const MatT<N, S>& r ) { return op::op_greater<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            operator>( const XformT<S>& l, const XformT<S>& r ) { return op::op_greater<4>( ( MatT<4, S> )l, ( MatT<4, S> )r ); }
template<       typename S> static DL_MATH_INL Bool            operator>( const AabbT<S>& l, const AabbT<S>& r ) { return ( l.max-l.min ) > ( r.max-r.min ); }
template<       typename S> static DL_MATH_INL Bool            operator>( const ObbT<S>& l, const ObbT<S>& r ) { return l.size > r.size; }

template<       typename S> static DL_MATH_INL Bool            operator>( RgbT<S> l, RgbT<S> r ) { return op::op_greater<3>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator>( RgbaT<S> l, RgbaT<S> r ) { return op::op_greater<4>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator>( HsvT<S> l, HsvT<S> r ) { return op::op_greater<3>( l, r ); }
template<       typename S> static DL_MATH_INL Bool            operator>( HslT<S> l, HslT<S> r ) { return op::op_greater<3>( l, r ); }

//=================================================================================================
// Unary negation.
//=================================================================================================

template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator-( VecT<N, S> v ) { return op::op_negate<N>( v ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      operator-( PosT<N, S> p ) { return op::op_negate<N>( p ); }
template<       typename S> static DL_MATH_INL QuatT<S>        operator-( QuatT<S> q ) { return op::op_negate<4>( q ); }

//=================================================================================================
// Vector arithmetic.
//=================================================================================================

template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator+( VecT<N, S> l, VecT<N, S> r ) { return op::op_ttadd<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator-( VecT<N, S> l, VecT<N, S> r ) { return op::op_ttsub<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator*( VecT<N, S> l, VecT<N, S> r ) { return op::op_ttmul<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator/( VecT<N, S> l, VecT<N, S> r ) { return op::op_ttdiv<N, S>( l, r ); }

template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator+( VecT<N, S> l, S r ) { return op::op_tsadd<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator-( VecT<N, S> l, S r ) { return op::op_tssub<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator*( VecT<N, S> l, S r ) { return op::op_tsmul<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator/( VecT<N, S> l, S r ) { return op::op_tsdiv<N, S>( l, r ); }

template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator+( S l, VecT<N, S> r ) { return op::op_stadd<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator-( S l, VecT<N, S> r ) { return op::op_stsub<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator*( S l, VecT<N, S> r ) { return op::op_stmul<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator/( S l, VecT<N, S> r ) { return op::op_stdiv<N, S>( l, r ); }

#if DL_MATH_ENABLE_MUTATING_OPERATORS
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>&     operator+=( VecT<N, S>& l, VecT<N, S> r ) { return ( l = ( l + r ) ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>&     operator-=( VecT<N, S>& l, VecT<N, S> r ) { return ( l = ( l - r ) ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>&     operator*=( VecT<N, S>& l, VecT<N, S> r ) { return ( l = ( l * r ) ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>&     operator/=( VecT<N, S>& l, VecT<N, S> r ) { return ( l = ( l / r ) ); }

template<Dim N, typename S> static DL_MATH_INL VecT<N, S>&     operator+=( VecT<N, S>& l, S r ) { return ( l = ( l + r ) ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>&     operator-=( VecT<N, S>& l, S r ) { return ( l = ( l - r ) ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>&     operator*=( VecT<N, S>& l, S r ) { return ( l = ( l * r ) ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>&     operator/=( VecT<N, S>& l, S r ) { return ( l = ( l / r ) ); }
#endif

//=================================================================================================
// Position arithmetic.
//=================================================================================================

template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      operator+( PosT<N, S> l, VecT<N, S> r ) { return op::op_ttadd<N, S>( l, ( PosT<N, S> )r ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      operator-( PosT<N, S> l, VecT<N, S> r ) { return op::op_ttsub<N, S>( l, ( PosT<N, S> )r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      operator-( PosT<N, S> l, PosT<N, S> r ) { return ( VecT<N, S> )op::op_ttsub<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      operator*( PosT<N, S> l, VecT<N, S> r ) { return op::op_ttmul<N, S>( l, ( PosT<N, S> )r ); }

#if DL_MATH_ENABLE_MUTATING_OPERATORS
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>&     operator+=( PosT<N, S>& l, VecT<N, S> r ) { return ( l = ( l + r ) ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>&     operator-=( PosT<N, S>& l, VecT<N, S> r ) { return ( l = ( l - r ) ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>&     operator-=( PosT<N, S>& l, PosT<N, S> r ) { return ( l = ( l * r ) ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>&     operator*=( PosT<N, S>& l, VecT<N, S> r ) { return ( l = ( l / r ) ); }
#endif

//=================================================================================================
// Quaternion arithmetic.
//=================================================================================================

template<typename S> static DL_MATH_INL QuatT<S>               operator+( QuatT<S> l, QuatT<S> r ) { return op::op_ttadd<4, S>( l, r ); }
template<typename S> static DL_MATH_INL QuatT<S>               operator-( QuatT<S> l, QuatT<S> r ) { return op::op_ttsub<4, S>( l, r ); }
template<typename S> static DL_MATH_INL QuatT<S>               operator*( QuatT<S> l, QuatT<S> r ) { return op::op_qqprod( l, r ); }

template<typename S> static DL_MATH_INL QuatT<S>               operator+( QuatT<S> l, S r ) { return op::op_tsadd<4, S>( l, r ); }
template<typename S> static DL_MATH_INL QuatT<S>               operator-( QuatT<S> l, S r ) { return op::op_tssub<4, S>( l, r ); }
template<typename S> static DL_MATH_INL QuatT<S>               operator*( QuatT<S> l, S r ) { return op::op_tsmul<4, S>( l, r ); }
template<typename S> static DL_MATH_INL QuatT<S>               operator/( QuatT<S> l, S r ) { return op::op_tsdiv<4, S>( l, r ); }

template<typename S> static DL_MATH_INL QuatT<S>               operator+( S l, QuatT<S> r ) { return op::op_stadd<4, S>( l, r ); }
template<typename S> static DL_MATH_INL QuatT<S>               operator-( S l, QuatT<S> r ) { return op::op_stsub<4, S>( l, r ); }
template<typename S> static DL_MATH_INL QuatT<S>               operator*( S l, QuatT<S> r ) { return op::op_stmul<4, S>( l, r ); }

#if DL_MATH_ENABLE_MUTATING_OPERATORS
template<typename S> static DL_MATH_INL QuatT<S>&              operator+=( QuatT<S>& l, QuatT<S> r ) { return ( l = ( l + r ) ); }
template<typename S> static DL_MATH_INL QuatT<S>&              operator-=( QuatT<S>& l, QuatT<S> r ) { return ( l = ( l - r ) ); }
template<typename S> static DL_MATH_INL QuatT<S>&              operator*=( QuatT<S>& l, QuatT<S> r ) { return ( l = ( l * r ) ); }

template<typename S> static DL_MATH_INL QuatT<S>&              operator+=( QuatT<S>& l, S r ) { return ( l = ( l + r ) ); }
template<typename S> static DL_MATH_INL QuatT<S>&              operator-=( QuatT<S>& l, S r ) { return ( l = ( l - r ) ); }
template<typename S> static DL_MATH_INL QuatT<S>&              operator*=( QuatT<S>& l, S r ) { return ( l = ( l * r ) ); }
template<typename S> static DL_MATH_INL QuatT<S>&              operator/=( QuatT<S>& l, S r ) { return ( l = ( l / r ) ); }
#endif

template<typename S> static DL_MATH_INL VecT<3, S>             operator*( QuatT<S> q, VecT<3, S> v ) { return op::op_qv3mul<S, QuatT<S>, VecT<3, S>>( q, v ); }
template<typename S> static DL_MATH_INL VecT<4, S>             operator*( QuatT<S> q, VecT<4, S> v ) { return op::op_qv4mul<S, QuatT<S>, VecT<4, S>>( q, v ); }
template<typename S> static DL_MATH_INL PosT<3, S>             operator*( QuatT<S> q, PosT<3, S> p ) { return op::op_qp3mul<S, QuatT<S>, PosT<3, S>>( q, p ); }

//=================================================================================================
// Matrix arithmetic.
//=================================================================================================

template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      operator+( const MatT<N, S>& l, const MatT<N, S>& r ) { return op::op_mmadd<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      operator-( const MatT<N, S>& l, const MatT<N, S>& r ) { return op::op_mmsub<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      operator*( const MatT<N, S>& l, const MatT<N, S>& r ) { return op::op_mmmul<N, S>( l, r ); }

template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      operator+( const MatT<N, S>& m, S s ) { return op::op_msadd<N, S>( m, s ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      operator-( const MatT<N, S>& m, S s ) { return op::op_mssub<N, S>( m, s ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      operator*( const MatT<N, S>& m, S s ) { return op::op_msmul<N, S>( m, s ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      operator/( const MatT<N, S>& m, S s ) { return op::op_msdiv<N, S>( m, s ); }

template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      operator+( S s, const MatT<N, S>& m ) { return op::op_msadd<N, S>( m, s ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      operator*( S s, const MatT<N, S>& m ) { return op::op_msmul<N, S>( m, s ); }

template<typename S> static DL_MATH_INL VecT<3, S>             operator*( const MatT<3, S>& m, VecT<3, S> v ) { return op::op_mt3mul<MatT<3, S>, VecT<3, S>>( m, v ); }
template<typename S> static DL_MATH_INL PosT<3, S>             operator*( const MatT<3, S>& m, PosT<3, S> p ) { return op::op_mt3mul<MatT<3, S>, PosT<3, S>>( m, p ); }
template<typename S> static DL_MATH_INL VecT<4, S>             operator*( const MatT<3, S>& m, VecT<4, S> v ) { return op::op_m3v4mul<MatT<3, S>, VecT<4, S>>( m, v ); }
template<typename S> static DL_MATH_INL RgbT<S>                operator*( const MatT<3, S>& m, RgbT<S> c ) { return ( RgbT<S> )( m * ( PosT<3, S> )c ); }
template<typename S> static DL_MATH_INL RgbaT<S>               operator*( const MatT<3, S>& m, RgbaT<S> c ) { return ( RgbaT<S> )( m * ( PosT<4, S> )c ); }

template<typename S> static DL_MATH_INL VecT<3, S>             operator*( const MatT<4, S>& m, VecT<3, S> v ) { return op::op_mt3mul<MatT<4, S>, VecT<3, S>>( m, v ); }
template<typename S> static DL_MATH_INL PosT<3, S>             operator*( const MatT<4, S>& m, PosT<3, S> p ) { return op::op_m4p3mul<MatT<4, S>, PosT<3, S>>( m, p ); }
template<typename S> static DL_MATH_INL VecT<4, S>             operator*( const MatT<4, S>& m, VecT<4, S> v ) { return op::op_m4v4mul<MatT<4, S>, VecT<4, S>>( m, v ); }
template<typename S> static DL_MATH_INL RgbT<S>                operator*( const MatT<4, S>& m, RgbT<S> c ) { return ( RgbT<S> )( m * ( PosT<3, S> )c ); }
template<typename S> static DL_MATH_INL RgbaT<S>               operator*( const MatT<4, S>& m, RgbaT<S> c ) { return ( RgbaT<S> )( m * ( PosT<4, S> )c ); }

template<typename S> static DL_MATH_INL MatT<4, S>             operator*( const MatT<4, S>& l, const MatT<3, S>& r ) { return op::op_m43mul<MatT<4, S>, MatT<3, S>>( l, r ); }
template<typename S> static DL_MATH_INL MatT<4, S>             operator*( const MatT<3, S>& l, const MatT<4, S>& r ) { return op::op_m34mul<MatT<3, S>, MatT<4, S>>( l, r ); }
#if DL_MATH_ENABLE_MUTATING_OPERATORS
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>&     operator+=( MatT<N, S>& l, const MatT<N, S>& r ) { return ( l = ( l + r ) ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>&     operator-=( MatT<N, S>& l, const MatT<N, S>& r ) { return ( l = ( l - r ) ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>&     operator*=( MatT<N, S>& l, const MatT<N, S>& r ) { return ( l = ( l * r ) ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>&     operator+=( MatT<N, S>& l, S s ) { return ( l = ( l + s ) ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>&     operator-=( MatT<N, S>& l, S s ) { return ( l = ( l - s ) ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>&     operator*=( MatT<N, S>& l, S s ) { return ( l = ( l * s ) ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>&     operator/=( MatT<N, S>& l, S s ) { return ( l = ( l / s ) ); }
#endif

//=================================================================================================
// Xform arithmetic.
//=================================================================================================

template<typename S> static DL_MATH_INL XformT<S>              operator+( const XformT<S>& l, const XformT<S>& r ) { return ( XformT<S> )( ( MatT<4, S> )l + ( MatT<4, S> )r ); }
template<typename S> static DL_MATH_INL XformT<S>              operator-( const XformT<S>& l, const XformT<S>& r ) { return ( XformT<S> )( ( MatT<4, S> )l - ( MatT<4, S> )r ); }
template<typename S> static DL_MATH_INL XformT<S>              operator*( const XformT<S>& l, const XformT<S>& r ) { return ( XformT<S> )( ( MatT<4, S> )l * ( MatT<4, S> )r ); }

template<typename S> static DL_MATH_INL XformT<S>              operator+( const XformT<S>& x, S s ) { return ( XformT<S> )( ( MatT<4, S> )x + s ); }
template<typename S> static DL_MATH_INL XformT<S>              operator-( const XformT<S>& x, S s ) { return ( XformT<S> )( ( MatT<4, S> )x - s ); }
template<typename S> static DL_MATH_INL XformT<S>              operator*( const XformT<S>& x, S s ) { return ( XformT<S> )( ( MatT<4, S> )x * s ); }
template<typename S> static DL_MATH_INL XformT<S>              operator/( const XformT<S>& x, S s ) { return ( XformT<S> )( ( MatT<4, S> )x / s ); }

template<typename S> static DL_MATH_INL XformT<S>              operator+( S s, const XformT<S>& x ) { return ( XformT<S> )( s + ( MatT<4, S> )x ); }
template<typename S> static DL_MATH_INL XformT<S>              operator*( S s, const XformT<S>& x ) { return ( XformT<S> )( s * ( MatT<4, S> )x ); }

template<typename S> static DL_MATH_INL VecT<3, S>             operator*( const XformT<S>& x, VecT<3, S> v ) { return ( MatT<4, S> )x * v; }
template<typename S> static DL_MATH_INL PosT<3, S>             operator*( const XformT<S>& x, PosT<3, S> p ) { return ( MatT<4, S> )x * p; }
template<typename S> static DL_MATH_INL VecT<4, S>             operator*( const XformT<S>& x, VecT<4, S> v ) { return ( MatT<4, S> )x * v; }
#if DL_MATH_ENABLE_MUTATING_OPERATORS
template<typename S> static DL_MATH_INL XformT<S>&             operator+=( XformT<S>& l, const XformT<S>& r ) { return ( l = ( l + r ) ); }
template<typename S> static DL_MATH_INL XformT<S>&             operator-=( XformT<S>& l, const XformT<S>& r ) { return ( l = ( l - r ) ); }
template<typename S> static DL_MATH_INL XformT<S>&             operator*=( XformT<S>& l, const XformT<S>& r ) { return ( l = ( l * r ) ); }
template<typename S> static DL_MATH_INL XformT<S>&             operator+=( XformT<S>& l, S s ) { return ( l = ( l + s ) ); }
template<typename S> static DL_MATH_INL XformT<S>&             operator-=( XformT<S>& l, S s ) { return ( l = ( l - s ) ); }
template<typename S> static DL_MATH_INL XformT<S>&             operator*=( XformT<S>& l, S s ) { return ( l = ( l * s ) ); }
template<typename S> static DL_MATH_INL XformT<S>&             operator/=( XformT<S>& l, S s ) { return ( l = ( l / s ) ); }
#endif

//=================================================================================================
// Vector functions.
//=================================================================================================

template<Dim N, typename S> static DL_MATH_INL S               min( VecT<N, S> v ) { return op::min<N, S>( v ); }
template<Dim N, typename S> static DL_MATH_INL S               max( VecT<N, S> v ) { return op::max<N, S>( v ); }
template<Dim N, typename S> static DL_MATH_INL S               avg( VecT<N, S> v ) { return op::avg<N, S>( v ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      round( VecT<N, S> v ) { return op::round<N, S>( v ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, UInt32> order( VecT<N, S> v ) { return op::order<N, S, VecT<N, S>>( v ); }
template<Dim N, typename S> static DL_MATH_INL S               sum( VecT<N, S> v ) { return op::sum<N, S>( v ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      abs( VecT<N, S> v ) { return op::abs<N, S, VecT<N, S>>( v ); }
template<Dim N, typename S> static DL_MATH_INL Bool            equal( VecT<N, S> l, VecT<N, S> r ) { return op::op_equal<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            zerolike( VecT<N, S> v ) { return op::zerolike<N>( v ); }
template<Dim N, typename S> static DL_MATH_INL const S&        elem( VecT<N, S> v, Dim i ) { return op::op_index<N, S>( v, i ); }
template<Dim N, typename S> static DL_MATH_INL Int32           compare( VecT<N, S> l, VecT<N, S> r ) { return op::compare<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL S               dot( VecT<N, S> l, VecT<N, S> r ) { return op::dot<N, S>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL S               normSq( VecT<N, S> v ) { return op::normSq<N, S>( v ); }
template<Dim N, typename S> static DL_MATH_INL S               norm( VecT<N, S> v ) { return op::norm<N, S>( v ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      unit( VecT<N, S> v ) { return op::unit<N, S>( v ); }
template<Dim N, typename S> static DL_MATH_INL Bool            isUnit( VecT<N, S> v ) { return op::isUnit<N, S>( v ); }
template<typename S> static DL_MATH_INL S                      cross( VecT<2, S> l, VecT<2, S> r ) { return op::cross( l, r ); }
template<typename S> static DL_MATH_INL VecT<3, S>             cross( VecT<3, S> l, VecT<3, S> r ) { return op::cross( l, r ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      project( VecT<N, S> v, VecT<N, S> n ) { return op::project<S>( v, n ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      reflect( VecT<N, S> v, VecT<N, S> n ) { return op::reflect<S>( v, n ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      lerp( VecT<N, S> l, VecT<N, S> r, S t ) { return math::lerp<VecT<N, S>, S>( l, r, t ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      nlerp( VecT<N, S> l, VecT<N, S> r, S t ) { return op::nlerp<N>( l, r, t ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      slerp( VecT<N, S> l, VecT<N, S> r, S t ) { return op::slerp<N>( l, r, t ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      fnlerp( VecT<N, S> l, VecT<N, S> r, S t ) { return op::fnlerp<N>( l, r, t ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      onlerp( VecT<N, S> l, VecT<N, S> r, S t ) { return op::onlerp<N>( l, r, t ); }

//=================================================================================================
// Position functions.
//=================================================================================================

template<Dim N, typename S> static DL_MATH_INL S               min( PosT<N, S> p ) { return op::min<N, S>( p ); }
template<Dim N, typename S> static DL_MATH_INL S               max( PosT<N, S> p ) { return op::max<N, S>( p ); }
template<Dim N, typename S> static DL_MATH_INL S               avg( PosT<N, S> p ) { return op::avg<N, S>( p ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      round( PosT<N, S> p ) { return ( PosT<N, S> )op::round<N, S>( ( VecT<N, S> )p ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, UInt32> order( PosT<N, S> p ) { return op::order<N, S, PosT<N, S>>( p ); }
template<Dim N, typename S> static DL_MATH_INL S               sum( PosT<N, S> p ) { return op::sum<N, S>( p ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      abs( PosT<N, S> p ) { return op::abs<N, S, PosT<N, S>>( p ); }
template<Dim N, typename S> static DL_MATH_INL Bool            equal( PosT<N, S> l, PosT<N, S> r ) { return op::op_equal<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            zerolike( PosT<N, S> p ) { return op::zerolike<N>( p ); }
template<Dim N, typename S> static DL_MATH_INL const S&        elem( PosT<N, S> p, Dim i ) { return op::op_index<N, S>( p, i ); }
template<Dim N, typename S> static DL_MATH_INL Int32           compare( PosT<N, S> l, PosT<N, S> r ) { return op::compare<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL S               distance( PosT<N, S> a, PosT<N, S> b ) { return op::distance<S>( a, b ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      project( PosT<N, S> pt, PosT<N, S> pl, VecT<N, S> n ) { return op::project<S>( pt, pl, n ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      reflect( PosT<N, S> pt, PosT<N, S> pl, VecT<N, S> n ) { return op::reflect<S>( pt, pl, n ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      lerp( PosT<N, S> l, PosT<N, S> r, S t ) { return ( PosT<N, S> )math::lerp( ( VecT<N, S> )l, ( VecT<N, S> )r, t ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      nlerp( PosT<N, S> l, PosT<N, S> r, S t ) { return op::nlerp<N>( l, r, t ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      slerp( PosT<N, S> l, PosT<N, S> r, S t ) { return op::slerp<N>( l, r, t ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      fnlerp( PosT<N, S> l, PosT<N, S> r, S t ) { return op::fnlerp<N>( l, r, t ); }
template<Dim N, typename S> static DL_MATH_INL PosT<N, S>      onlerp( PosT<N, S> l, PosT<N, S> r, S t ) { return op::onlerp<N>( l, r, t ); }

//=================================================================================================
// Quaternion functions.
//=================================================================================================

template<typename S> static DL_MATH_INL Bool                   equal( QuatT<S>l, QuatT<S>r ) { return op::op_equal<4>( l, r ); }
template<typename S> static DL_MATH_INL Bool                   zerolike( QuatT<S>q ) { return op::zerolike<4>( q ); }
template<typename S> static DL_MATH_INL const S&               elem( QuatT<S> q, Dim i ) { return op::op_index<4, S>( q, i ); }
template<typename S> static DL_MATH_INL Int32                  compare( QuatT<S>l, QuatT<S>   r ) { return op::compare<4>( l, r ); }
template<typename S> static DL_MATH_INL S                      min( QuatT<S> v ) { return op::min<4, S>( v ); }
template<typename S> static DL_MATH_INL S                      max( QuatT<S> v ) { return op::max<4, S>( v ); }
template<typename S> static DL_MATH_INL S                      avg( QuatT<S> v ) { return op::avg<4, S>( v ); }
template<typename S> static DL_MATH_INL S                      sum( QuatT<S> v ) { return op::sum<4, S>( v ); }
template<typename S> static DL_MATH_INL S                      dot( QuatT<S> l, QuatT<S> r ) { return op::dot<4, S>( l, r ); }
template<typename S> static DL_MATH_INL S                      normSq( QuatT<S> v ) { return op::normSq<4, S>( v ); }
template<typename S> static DL_MATH_INL S                      norm( QuatT<S> v ) { return op::norm<4, S>( v ); }
template<typename S> static DL_MATH_INL QuatT<S>               unit( QuatT<S> v ) { return op::unit<4, S>( v ); }
template<typename S> static DL_MATH_INL Bool                   isUnit( QuatT<S> v ) { return op::isUnit<4, S>( v ); }
template<typename S> static DL_MATH_INL QuatT<S>               lerp( QuatT<S> l, QuatT<S> r, S t ) { return math::lerp<QuatT<S>, S>( l, r, t ); }
template<typename S> static DL_MATH_INL QuatT<S>               nlerp( QuatT<S> l, QuatT<S> r, S t ) { return op::nlerp<4>( l, r, t ); }
template<typename S> static DL_MATH_INL QuatT<S>               slerp( QuatT<S> l, QuatT<S> r, S t ) { return op::slerp<4>( l, r, t ); }
template<typename S> static DL_MATH_INL QuatT<S>               fnlerp( QuatT<S> l, QuatT<S> r, S t ) { return op::fnlerp<4>( l, r, t ); }
template<typename S> static DL_MATH_INL QuatT<S>               onlerp( QuatT<S> l, QuatT<S> r, S t ) { return op::onlerp<4>( l, r, t ); }
template<typename S> static DL_MATH_INL S                      scalar( QuatT<S> q ) { return q.w; }
template<typename S> static DL_MATH_INL VecT<3, S>             vector( QuatT<S> q ) { return VecT<3, S>::make( q.x, q.y, q.z ); }
template<typename S> static DL_MATH_INL QuatT<S>               conjugate( QuatT<S> q ) { return QuatT<S>::make( -q.x, -q.y, -q.z, q.w ); }
template<typename S> static DL_MATH_INL QuatT<S>               inverse( QuatT<S> q ) { return unit( conjugate( q ) ); }
template<typename S> static DL_MATH_INL QuatT<S>               product( QuatT<S> l, QuatT<S> r ) { return op::op_qqprod( l, r ); }
template<typename S> static DL_MATH_INL QuatT<S>               quatFromAA( VecT<3, S> axis, S angle ) { return op::qfromaa<QuatT<S>>( axis, angle ); }
template<typename S> static DL_MATH_INL void                   quattoAA( QuatT<S> q, VecT<3, S>& axis, S& angle ) { op::qtoaa( q, axis, angle ); }

//=================================================================================================
// Xform functions.
//=================================================================================================

template<typename S> static DL_MATH_INL XformT<S>              nlerp( const XformT<S>& l, const XformT<S>& r, S t ) { return XformT<S>::make( lerp( l.position, r.position, t ), nlerp( l.rotation, r.rotation, t ), lerp( l.scale, r.scale, t ) ); }
template<typename S> static DL_MATH_INL XformT<S>              slerp( const XformT<S>& l, const XformT<S>& r, S t ) { return XformT<S>::make( lerp( l.position, r.position, t ), slerp( l.rotation, r.rotation, t ), lerp( l.scale, r.scale, t ) ); }
template<typename S> static DL_MATH_INL XformT<S>              fnlerp( const XformT<S>& l, const XformT<S>& r, S t ) { return XformT<S>::make( lerp( l.position, r.position, t ), fnlerp( l.rotation, r.rotation, t ), lerp( l.scale, r.scale, t ) ); }
template<typename S> static DL_MATH_INL XformT<S>              onlerp( const XformT<S>& l, const XformT<S>& r, S t ) { return XformT<S>::make( lerp( l.position, r.position, t ), onlerp( l.rotation, r.rotation, t ), lerp( l.scale, r.scale, t ) ); }

//=================================================================================================
// Matrix functions.
//=================================================================================================

template<Dim N, typename S> static DL_MATH_INL Bool            equal( const MatT<N, S>& l, const MatT<N, S>& r ) { return op::op_equal<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            zerolike( const MatT<N, S>& m ) { return op::zerolike<N>( m ); }
template<Dim N, typename S> static DL_MATH_INL const VecT<N, S>& elem( const MatT<N, S>& m, Dim i ) { return op::op_index<N, VecT<N, S>>( m, i ); }
template<Dim N, typename S> static DL_MATH_INL Int32           compare( const MatT<N, S>& l, const MatT<N, S>& r ) { return op::compare<N>( l, r ); }
template<Dim N, typename S> static DL_MATH_INL Bool            isZero( const MatT<N, S>& m ) { return op::isZero<N, S>( m ); }
template<Dim N, typename S> static DL_MATH_INL Bool            isIdentity( const MatT<N, S>& m ) { return op::isIdentity<N, S>( m ); }
template<typename S> static DL_MATH_INL S                      isSingular( const MatT<3, S>& m ) { return op::isSingularM3<S>( m ); }
template<typename S> static DL_MATH_INL S                      isSingular( const MatT<4, S>& m ) { return op::isSingularM4<S>( m ); }
template<typename S> static DL_MATH_INL S                      isReflection( const MatT<3, S>& m ) { return op::isReflectionM3<S>( m ); }
template<typename S> static DL_MATH_INL S                      isReflection( const MatT<4, S>& m ) { return op::isReflectionM4<S>( m ); }
template<Dim N, typename S> static DL_MATH_INL Bool            isNormalized( const MatT<N, S>& m ) { return op::isNormalized<N, S>( m ); }
template<Dim N, typename S> static DL_MATH_INL Bool            isOrtho( const MatT<N, S>& m ) { return op::isOrtho<N, S>( m ); }
template<Dim N, typename S> static DL_MATH_INL Bool            isOrthoNormal( const MatT<N, S>& m ) { return op::isOrthoNormal<N, S>( m ); }
template<Dim N, typename S> static DL_MATH_INL S               min( const MatT<N, S>& m ) { return op::minM<N, S>( m ); }
template<Dim N, typename S> static DL_MATH_INL S               max( const MatT<N, S>& m ) { return op::maxM<N, S>( m ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, UInt32> order( MatT<N, S> m ) { return op::order<N, S, MatT<N, S>>( m ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      unit( const MatT<N, S>& m ) { return op::unitM<N, S>( m ); }
template<Dim N, typename S> static DL_MATH_INL Bool            isUnit( const MatT<N, S>& m ) { return op::isUnitM<N, S>( m ); }
template<Dim N, typename S> static DL_MATH_INL S               trace( const MatT<N, S>& m ) { return op::trace<N, S>( m ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, S>      diagonal( const MatT<N, S>& m ) { return op::diagonal<N, S>( m ); }
template<Dim N, typename S> static DL_MATH_INL VecT<3, S>      scale( const MatT<N, S>& m ) { return op::scale<S, VecT<3, S>>( m ); }
template<Dim N, typename S> static DL_MATH_INL MatT<3, S>      rotation( const MatT<N, S>& m ) { return op::rotation<S, MatT<3, S>>( m ); }
template<Dim N, typename S> static DL_MATH_INL VecT<3, S>      eulers( const MatT<N, S>& m ) { return op::eulers<S, VecT<3, S>>( m ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      transpose( const MatT<N, S>& m ) { return op::transpose<N, S>( m ); }
template<typename S> static DL_MATH_INL S                      determinant( const MatT<3, S>& m ) { return op::determinantM3<S>( m ); }
template<typename S> static DL_MATH_INL S                      determinant( const MatT<4, S>& m ) { return op::determinantM4<S>( m ); }
template<typename S> static DL_MATH_INL MatT<3, S>             inverse( const MatT<3, S>& m ) { return op::inverseM3<S>( m ); }
template<typename S> static DL_MATH_INL MatT<4, S>             inverse( const MatT<4, S>& m ) { return op::inverseM4<S>( m ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      inverseTranspose( const MatT<N, S>& m ) { return transpose( inverse( m ) ); }
template<typename S> static DL_MATH_INL Bool                   isAffine( const MatT<4, S>& m ) { return op::isAffine<S>( m ); }
template<typename S> static DL_MATH_INL Bool                   isTranslation( const MatT<4, S>& m ) { return op::isTranslation<S>( m ); }
template<typename S> static DL_MATH_INL VecT<3, S>             translation( const MatT<4, S>& m ) { return op::translation<S, VecT<3, S>>( m ); }

template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      nlerp( const MatT<N, S>& l, const MatT<N, S>& r, S t ) { return ( MatT<N, S> )math::nlerp( ( XformT<S> )l, ( XformT<S> )r, t ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      slerp( const MatT<N, S>& l, const MatT<N, S>& r, S t ) { return ( MatT<N, S> )math::slerp( ( XformT<S> )l, ( XformT<S> )r, t ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      fnlerp( const MatT<N, S>& l, const MatT<N, S>& r, S t ) { return ( MatT<N, S> )math::fnlerp( ( XformT<S> )l, ( XformT<S> )r, t ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      onlerp( const MatT<N, S>& l, const MatT<N, S>& r, S t ) { return ( MatT<N, S> )math::onlerp( ( XformT<S> )l, ( XformT<S> )r, t ); }

template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      makeScale( S x, S y, S z ) { return op::makeScale<S, MatT<N, S>>( x, y, z ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      makeRotationX( S a ) { return op::makeRotationX<S, MatT<N, S>>( a ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      makeRotationY( S a ) { return op::makeRotationY<S, MatT<N, S>>( a ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      makeRotationZ( S a ) { return op::makeRotationZ<S, MatT<N, S>>( a ); }
template<Dim N, typename S> static DL_MATH_INL MatT<N, S>      makeRotation( S x, S y, S z, const char* order = "xyz" ) { return op::makeRotation<S, MatT<N, S>>( x, y, z, order ); }
template<typename S> static DL_MATH_INL MatT<4, S>             makeTranslation( S x, S y, S z ) { return op::makeTranslation<S, MatT<4, S>>( x, y, z ); }

//=================================================================================================
// Rgb.
//=================================================================================================

template<typename S> static DL_MATH_INL S                      min( RgbT<S> c ) { return op::min<3, S>( c ); }
template<typename S> static DL_MATH_INL S                      max( RgbT<S> c ) { return op::max<3, S>( c ); }
template<typename S> static DL_MATH_INL S                      avg( RgbT<S> c ) { return op::avg<3, S>( c ); }
template<typename S> static DL_MATH_INL RgbT<S>                round( RgbT<S> c ) { return op::round<S>( c ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, UInt32> order( RgbT<S> c ) { return op::order<N, S, RgbT<S>>( c ); }
template<typename S> static DL_MATH_INL S                      sum( RgbT<S> c ) { return op::sum<3, S>( c ); }
template<typename S> static DL_MATH_INL RgbT<S>                abs( RgbT<S> c ) { return op::abs<3, S, RgbT<S>>( c ); }
template<typename S> static DL_MATH_INL Bool                   equal( RgbT<S> l, RgbT<S>r ) { return op::op_equal<3>( l, r ); }
template<typename S> static DL_MATH_INL Bool                   zerolike( RgbT<S> q ) { return op::zerolike<3>( q ); }
template<typename S> static DL_MATH_INL const S&               elem( RgbT<S> q, Dim i ) { return op::op_index<3, S>( q, i ); }
template<typename S> static DL_MATH_INL Int32                  compare( RgbT<S> l, RgbT<S> r ) { return op::compare<3>( l, r ); }
template<typename S> static DL_MATH_INL HsvT<S>                toHsv( RgbT<S> c ) { return op::op_rgbtohsv( c ); }
template<typename S> static DL_MATH_INL HslT<S>                toHsl( RgbT<S> c ) { return op::op_rgbtohsl( c ); }

//=================================================================================================
// Rgba.
//=================================================================================================

template<typename S> static DL_MATH_INL S                      min( RgbaT<S> c ) { return op::min<4, S>( c ); }
template<typename S> static DL_MATH_INL S                      max( RgbaT<S> c ) { return op::max<4, S>( c ); }
template<typename S> static DL_MATH_INL S                      avg( RgbaT<S> c ) { return op::avg<4, S>( c ); }
template<typename S> static DL_MATH_INL RgbaT<S>               round( RgbaT<S> c ) { return op::round<S>( c ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, UInt32> order( RgbaT<S> c ) { return op::order<N, S, RgbaT<S>>( c ); }
template<typename S> static DL_MATH_INL S                      sum( RgbaT<S> c ) { return op::sum<4, S>( c ); }
template<typename S> static DL_MATH_INL RgbaT<S>               abs( RgbaT<S> c ) { return op::abs<4, S, RgbaT<S>>( c ); }
template<typename S> static DL_MATH_INL Bool                   equal( RgbaT<S> l, RgbaT<S>r ) { return op::op_equal<4>( l, r ); }
template<typename S> static DL_MATH_INL Bool                   zerolike( RgbaT<S> q ) { return op::zerolike<4>( q ); }
template<typename S> static DL_MATH_INL const S&               elem( RgbaT<S> q, Dim i ) { return op::op_index<4, S>( q, i ); }
template<typename S> static DL_MATH_INL Int32                  compare( RgbaT<S> l, RgbaT<S> r ) { return op::compare<4>( l, r ); }
template<typename S> static DL_MATH_INL HsvT<S>                toHsv( RgbaT<S> c ) { return op::op_rgbtohsv( c ); }
template<typename S> static DL_MATH_INL HslT<S>                toHsl( RgbaT<S> c ) { return op::op_rgbtohsl( c ); }

//=================================================================================================
// Hsv.
//=================================================================================================

template<typename S> static DL_MATH_INL S                      min( HsvT<S> c ) { return op::min<3, S>( c ); }
template<typename S> static DL_MATH_INL S                      max( HsvT<S> c ) { return op::max<3, S>( c ); }
template<typename S> static DL_MATH_INL S                      avg( HsvT<S> c ) { return op::avg<3, S>( c ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, UInt32> order( HsvT<S> c ) { return op::order<N, S, HsvT<S>>( c ); }
template<typename S> static DL_MATH_INL S                      sum( HsvT<S> c ) { return op::sum<3, S>( c ); }
template<typename S> static DL_MATH_INL HsvT<S>                abs( HsvT<S> c ) { return op::abs<3, S, HsvT<S>>( c ); }
template<typename S> static DL_MATH_INL Bool                   equal( HsvT<S> l, HsvT<S>r ) { return op::op_equal<3>( l, r ); }
template<typename S> static DL_MATH_INL Bool                   zerolike( HsvT<S> q ) { return op::zerolike<3>( q ); }
template<typename S> static DL_MATH_INL const S&               elem( HsvT<S> q, Dim i ) { return op::op_index<3, S>( q, i ); }
template<typename S> static DL_MATH_INL Int32                  compare( HsvT<S> l, HsvT<S> r ) { return op::compare<3>( l, r ); }
template<typename S> static DL_MATH_INL RgbT<S>                toRgb( HsvT<S> c ) { return op::op_hsvtorgb( c ); }
template<typename S> static DL_MATH_INL RgbaT<S>               toRgba( HsvT<S> c ) { return op::op_hsvtorgba( c ); }

//=================================================================================================
// Hsl.
//=================================================================================================

template<typename S> static DL_MATH_INL S                      min( HslT<S> c ) { return op::min<3, S>( c ); }
template<typename S> static DL_MATH_INL S                      max( HslT<S> c ) { return op::max<3, S>( c ); }
template<typename S> static DL_MATH_INL S                      avg( HslT<S> c ) { return op::avg<3, S>( c ); }
template<Dim N, typename S> static DL_MATH_INL VecT<N, UInt32> order( HslT<S> c ) { return op::order<N, S, HslT<S>>( c ); }
template<typename S> static DL_MATH_INL S                      sum( HslT<S> c ) { return op::sum<3, S>( c ); }
template<typename S> static DL_MATH_INL HslT<S>                abs( HslT<S> c ) { return op::abs<3, S, HslT<S>>( c ); }
template<typename S> static DL_MATH_INL Bool                   equal( HslT<S> l, HslT<S>r ) { return op::op_equal<3>( l, r ); }
template<typename S> static DL_MATH_INL Bool                   zerolike( HslT<S> q ) { return op::zerolike<3>( q ); }
template<typename S> static DL_MATH_INL const S&               elem( HslT<S> q, Dim i ) { return op::op_index<3, S>( q, i ); }
template<typename S> static DL_MATH_INL Int32                  compare( HslT<S> l, HslT<S> r ) { return op::compare<3>( l, r ); }
template<typename S> static DL_MATH_INL RgbT<S>                toRgb( HslT<S> c ) { return op::op_hsltorgb( c ); }
template<typename S> static DL_MATH_INL RgbaT<S>               toRgba( HslT<S> c ) { return op::op_hsltorgba( c ); }

} // math

//=================================================================================================
// Define dl::ds::hash for math types.
//=================================================================================================

namespace ds {
template<Dim N, typename S> static DL_MATH_INL UInt64          hash( math::VecT<N, S> v ) { return hash<N, math::VecT<N, S>>( v ); }
template<Dim N, typename S> static DL_MATH_INL UInt64          hash( math::PosT<N, S> p ) { return hash<N, math::PosT<N, S>>( p ); }
template<       typename S> static DL_MATH_INL UInt64          hash( math::QuatT<S> q ) { return hash<4, math::QuatT<S>>( q ); }
template<Dim N, typename S> static DL_MATH_INL UInt64          hash( const math::MatT<N, S>& m ) { return hash<N, math::MatT<N, S>>( m ); }
template<       typename S> static DL_MATH_INL UInt64          hash( math::RgbT<S> c ) { return hash<3, math::RgbT<S>>( c ); }
template<       typename S> static DL_MATH_INL UInt64          hash( math::RgbaT<S> c ) { return hash<4, math::RgbaT<S>>( c ); }
template<       typename S> static DL_MATH_INL UInt64          hash( math::HsvT<S> c ) { return hash<3, math::HsvT<S>>( c ); }
template<       typename S> static DL_MATH_INL UInt64          hash( math::HslT<S> c ) { return hash<3, math::HslT<S>>( c ); }
} // ds

//=================================================================================================
// Macro for setting up N-tuple specializations.
//=================================================================================================

namespace math {

#define DL_MATH_SETUP_STRUCT(NUM,SCALAR) \
    enum { N = NUM };                     \
    DL_MATH_MAKE_TYPEDEFS(SCALAR)

#define DL_MATH_SETUP_STRUCT_WITH_S(NUM,SCALAR) \
    enum { N = NUM };                     \
    typedef SCALAR S;                     \
    DL_MATH_MAKE_TYPEDEFS(SCALAR)

//=================================================================================================
// Vec2.
//=================================================================================================

template<typename S> struct VecT<2, S> DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 2, S );

    union
    {
        struct { S x, y; };
        struct { S u, v; };
        struct { S a, b; };
        S m[N];
        S comp[N];
    };

    static const V2 zero;
    static const V2 one;
    static const V2 unitX;
    static const V2 unitY;

    static DL_MATH_INL V2 make()
    {
        return make( 0, 0 );
    }

    static DL_MATH_INL V2 make( const S p[N] )
    {
        return op::from_ptr<V2>( p );
    }

    template<typename S2, typename S3>
    static DL_MATH_INL V2 make( S2 x, S3 y )
    {
        V2 o = { ( S )x, ( S )y };
        return o;
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator VecT<N, S2>() const
    {
        return VecT<N, S2>::make( x, y );
    }

    DL_EXPLICIT DL_MATH_INL       operator V2() const { return *this; }
    DL_EXPLICIT DL_MATH_INL       operator P2() const { return P2 { x, y }; }
    DL_EXPLICIT DL_MATH_INL       operator V3() const { return V3 { x, y, ( S )0 }; }
    DL_EXPLICIT DL_MATH_INL       operator P3() const { return P3 { x, y, ( S )0 }; }
    DL_EXPLICIT DL_MATH_INL       operator V4() const { return V4 { x, y, ( S )0, ( S )0 }; }

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL S                  aspect() const { return ( S )x / ( S )y; }
    DL_MATH_INL V2                 abs() const { return math::abs( *this ); }
    DL_MATH_INL V2                 unit() const { return math::unit( *this ); }
    DL_MATH_INL Bool               isUnit() const { return math::isUnit( *this ); }

    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL V2                 round() const { return math::round( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  sum() const { return math::sum( *this ); }
    DL_MATH_INL S                  dot( V2 t ) const { return math::dot( *this, t ); }
    DL_MATH_INL S                  normSq() const { return math::normSq( *this ); }
    DL_MATH_INL S                  norm() const { return math::norm( *this ); }
    DL_MATH_INL S                  cross( V2 t ) const { return math::cross( *this, t ); }

    DL_MATH_INL V2                 project( V2 n ) const { return math::project( *this, n ); }
    DL_MATH_INL V2                 reflect( V2 n ) const { return math::reflect( *this, n ); }

    template<typename S2> DL_MATH_INL V2 lerp( V2 v, S2 t ) const { return math::lerp( *this, v, t ); }
    template<typename S2> DL_MATH_INL V2 nlerp( V2 v, S2 t ) const { return math::nlerp( *this, v, t ); }
    template<typename S2> DL_MATH_INL V2 slerp( V2 v, S2 t ) const { return math::slerp( *this, v, t ); }
};

//=================================================================================================
// Pos2.
//=================================================================================================

template<typename S> struct PosT<2, S> DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 2, S );

    union
    {
        struct { S x, y; };
        struct { S u, v; };
        struct { S a, b; };
        S m[N];
        S comp[N];
    };

    static const P2 zero;

    static DL_MATH_INL P2 make()
    {
        return make( 0, 0 );
    }

    static DL_MATH_INL P2 make( const S p[N] )
    {
        return op::from_ptr<P2>( p );
    }

    template<typename S2, typename S3>
    static DL_MATH_INL P2 make( S2 x, S3 y )
    {
        P2 o = { ( S )x, ( S )y };
        return o;
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator PosT<N, S2>() const
    {
        return PosT<N, S2>::make( x, y );
    }

    DL_EXPLICIT DL_MATH_INL       operator V2() const { return V2 { x, y }; }
    DL_EXPLICIT DL_MATH_INL       operator P2() const { return *this; }
    DL_EXPLICIT DL_MATH_INL       operator V3() const { return V3 { x, y, ( S )0 }; }
    DL_EXPLICIT DL_MATH_INL       operator P3() const { return P3 { x, y, ( S )0 }; }
    DL_EXPLICIT DL_MATH_INL       operator V4() const { return V4 { x, y, ( S )0, ( S )0 }; }

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL S                  aspect() const { return ( S )x / ( S )y; }
    DL_MATH_INL P2                 abs() const { return math::abs( *this ); }
    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL P2                 round() const { return math::round( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  distance( P2 t ) const { return math::distance( *this, t ); }

    DL_MATH_INL P2                 project( P2 pl, V2 n ) const { return math::project( *this, pl, n ); }
    DL_MATH_INL P2                 reflect( P2 pl, V2 n ) const { return math::reflect( *this, pl, n ); }

    template<typename S2> DL_MATH_INL P2 lerp( P2 p, S2 t ) const { return math::lerp( *this, v, t ); }
    template<typename S2> DL_MATH_INL P2 nlerp( P2 p, S2 t ) const { return math::nlerp( *this, v, t ); }
    template<typename S2> DL_MATH_INL P2 slerp( P2 p, S2 t ) const { return math::slerp( *this, v, t ); }
};

//=================================================================================================
// Vec3.
//=================================================================================================

template<typename S> struct VecT<3, S> DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 3, S );

    union
    {
        struct { S x, y, z; };
        struct { S u, v, w; };
        struct { S a, b, c; };
        S m[N];
        S comp[N];
        V2 v2;
        V2 xy;
        P2 p2;
    };

    static const V3 zero;
    static const V3 one;
    static const V3 unitX;
    static const V3 unitY;
    static const V3 unitZ;

    static DL_MATH_INL V3 make()
    {
        return make( 0, 0, 0 );
    }

    static DL_MATH_INL V3 make( const S p[N] )
    {
        return op::from_ptr<V3>( p );
    }

    template<typename S2, typename S3, typename S4>
    static DL_MATH_INL V3 make( S2 x, S3 y, S4 z )
    {
        V3 o = { ( S )x, ( S )y, ( S )z };
        return o;
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator VecT<N, S2>() const
    {
        return VecT<N, S2>::make( x, y, z );
    }

    DL_EXPLICIT DL_MATH_INL       operator V2() const { return v2; }
    DL_EXPLICIT DL_MATH_INL       operator P2() const { return p2; }
    DL_EXPLICIT DL_MATH_INL       operator V3() const { return *this; }
    DL_EXPLICIT DL_MATH_INL       operator P3() const { return P3 { x, y, z }; }
    DL_EXPLICIT DL_MATH_INL       operator C3() const { return C3 { x, y, z }; }
    DL_EXPLICIT DL_MATH_INL       operator V4() const { return V4 { x, y, z, ( S )0 }; }
    DL_EXPLICIT DL_MATH_INL       operator C4() const { return C4 { x, y, z, ( S )1 }; }

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL V3                 abs() const { return math::abs( *this ); }
    DL_MATH_INL V3                 unit() const { return math::unit( *this ); }
    DL_MATH_INL Bool               isUnit() const { return math::isUnit( *this ); }

    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL V3                 round() const { return math::round( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  sum() const { return math::sum( *this ); }
    DL_MATH_INL S                  dot( V3 t ) const { return math::dot( *this, t ); }
    DL_MATH_INL S                  normSq() const { return math::normSq( *this ); }
    DL_MATH_INL S                  norm() const { return math::norm( *this ); }

    DL_MATH_INL V3                 cross( V3 t ) const { return math::cross( *this, t ); }
    DL_MATH_INL V3                 project( V3 n ) const { return math::project( *this, n ); }
    DL_MATH_INL V3                 reflect( V3 n ) const { return math::reflect( *this, n ); }

    template<typename S2> DL_MATH_INL V3 lerp( V3 v, S2 t ) const { return math::lerp( *this, v, t ); }
    template<typename S2> DL_MATH_INL V3 nlerp( V3 v, S2 t ) const { return math::nlerp( *this, v, t ); }
    template<typename S2> DL_MATH_INL V3 slerp( V3 v, S2 t ) const { return math::slerp( *this, v, t ); }
};

//=================================================================================================
// Pos3.
//=================================================================================================

template<typename S> struct PosT<3, S> DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 3, S );

    union
    {
        struct { S x, y, z; };
        struct { S u, v, w; };
        struct { S a, b, c; };
        S m[N];
        S comp[N];
        V2 v2;
        V2 xy;
        P2 p2;
        V3 v3;
        V3 xyz;
    };

    static const P3 zero;

    static DL_MATH_INL P3 make()
    {
        return make( 0, 0, 0 );
    }

    static DL_MATH_INL P3 make( const S p[N] )
    {
        return op::from_ptr<P3>( p );
    }

    template<typename S2, typename S3, typename S4>
    static DL_MATH_INL P3 make( S2 x, S3 y, S4 z )
    {
        P3 o = { ( S )x, ( S )y, ( S )z };
        return o;
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator PosT<N, S2>() const
    {
        return PosT<N, S2>::make( x, y, z );
    }

    DL_EXPLICIT DL_MATH_INL       operator V2() const { return v2; }
    DL_EXPLICIT DL_MATH_INL       operator P2() const { return p2; }
    DL_EXPLICIT DL_MATH_INL       operator V3() const { return v3; }
    DL_EXPLICIT DL_MATH_INL       operator P3() const { return *this; }
    DL_EXPLICIT DL_MATH_INL       operator C3() const { return C3 { x, y, z }; }
    DL_EXPLICIT DL_MATH_INL       operator V4() const { return V4 { x, y, z, ( S )1 }; }
    DL_EXPLICIT DL_MATH_INL       operator C4() const { return C4 { x, y, z, ( S )1 }; }

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL P3                 abs() const { return math::abs( *this ); }
    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL P3                 round() const { return math::round( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  distance( P3 t ) const { return math::distance( *this, t ); }

    DL_MATH_INL P3                 project( P3 pl, V3 n ) const { return math::project( *this, pl, n ); }
    DL_MATH_INL P3                 reflect( P3 pl, V3 n ) const { return math::reflect( *this, pl, n ); }

    template<typename S2> DL_MATH_INL P3 lerp( P3 p, S2 t ) const { return math::lerp( *this, v, t ); }
    template<typename S2> DL_MATH_INL P3 nlerp( P3 p, S2 t ) const { return math::nlerp( *this, v, t ); }
    template<typename S2> DL_MATH_INL P3 slerp( P3 p, S2 t ) const { return math::slerp( *this, v, t ); }
};

//=================================================================================================
// Vec4.
//=================================================================================================

template<typename S> struct VecT<4, S> DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 4, S );

    union
    {
        struct { S x, y, z, w; };
        struct { S a, b, c, d; };
        S m[N];
        S comp[N];
        V2 v2;
        V2 xy;
        P2 p2;
        P3 p3;
        V3 v3;
        V3 xyz;
    };

    static const V4 zero;
    static const V4 one;
    static const V4 unitX;
    static const V4 unitY;
    static const V4 unitZ;
    static const V4 unitW;

    static DL_MATH_INL V4 make()
    {
        return make( 0, 0, 0, 0 );
    }

    static DL_MATH_INL V4 make( const S p[N] )
    {
        return op::from_ptr<V4>( p );
    }

    template<typename S2, typename S3, typename S4, typename S5>
    static DL_MATH_INL V4 make( S2 x, S3 y, S4 z, S5 w )
    {
        V4 o = { ( S )x, ( S )y, ( S )z, ( S )w };
        return o;
    }

    template<typename S2, typename S3, typename S4>
    static DL_MATH_INL V4 make( S2 x, S3 y, S4 z )
    {
        return make( x, y, z, 0 );
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator VecT<N, S2>() const
    {
        return VecT<N, S2>::make( x, y, z, w );
    }

    DL_EXPLICIT DL_MATH_INL       operator V2() const { return v2; }
    DL_EXPLICIT DL_MATH_INL       operator P2() const { return p2; }
    DL_EXPLICIT DL_MATH_INL       operator V3() const { return v3; }
    DL_EXPLICIT DL_MATH_INL       operator P3() const { return op::op_v4top3( *this ); }
    DL_EXPLICIT DL_MATH_INL       operator C3() const { return C3 { x, y, z }; }
    DL_EXPLICIT DL_MATH_INL       operator C4() const { return C4 { x, y, z, w }; }
    DL_EXPLICIT DL_MATH_INL       operator Q() const { return Q { x, y, z, w }; }

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL V4                 abs() const { return math::abs( *this ); }
    DL_MATH_INL V4                 unit() const { return math::unit( *this ); }
    DL_MATH_INL Bool               isUnit() const { return math::isUnit( *this ); }

    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL V4                 round() const { return math::round( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  sum() const { return math::sum( *this ); }
    DL_MATH_INL S                  dot( V4 t ) const { return math::dot( *this, t ); }
    DL_MATH_INL S                  normSq() const { return math::normSq( *this ); }
    DL_MATH_INL S                  norm() const { return math::norm( *this ); }

    template<typename S2> DL_MATH_INL V4 lerp( V4 v, S2 t ) const { return math::lerp( *this, v, t ); }
    template<typename S2> DL_MATH_INL V4 nlerp( V4 v, S2 t ) const { return math::nlerp( *this, v, t ); }
    template<typename S2> DL_MATH_INL V4 slerp( V4 v, S2 t ) const { return math::slerp( *this, v, t ); }
};

//=================================================================================================
// Quaternion.
//=================================================================================================

template<typename S> struct QuatT DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 4, S );

    union
    {
        struct { S x, y, z, w; };
        struct { S a, b, c, d; };
        S m[N];
        S comp[N];
    };

    static const Q zero;
    static const Q identity;

    static DL_MATH_INL Q make()
    {
        return make( 0, 0, 0, 1 );
    }

    static DL_MATH_INL Q make( const S p[N] )
    {
        return op::from_ptr<Q>( p );
    }

    template<typename S2, typename S3, typename S4, typename S5>
    static DL_MATH_INL Q make( S2 x, S3 y, S4 z, S5 w )
    {
        Q o = { ( S )x, ( S )y, ( S )z, ( S )w };
        return o;
    }

    template<typename S2, typename S3, typename S4>
    static DL_MATH_INL Q make( S2 x, S3 y, S4 z )
    {
        return make( x, y, z, 1 );
    }

    template<typename S2>
    static DL_MATH_INL Q make( V3 v, S2 s )
    {
        return make( v.x, v.y, v.z, s );
    }

    static DL_MATH_INL Q make( V3 v )
    {
        return make( v, 1 );
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator QuatT<S2>() const
    {
        return QuatT<S2>::make( x, y, z, w );
    }

    DL_EXPLICIT DL_MATH_INL       operator V4() const { return V4 { x, y, z, w }; }
    DL_EXPLICIT DL_MATH_INL       operator M3() const { return op::op_q2m<S, M3>( *this ); }
    DL_EXPLICIT DL_MATH_INL       operator M4() const { return op::op_q2m<S, M4>( *this ); }

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL Q                  unit() const { return math::unit( *this ); }
    DL_MATH_INL Bool               isUnit() const { return math::isUnit( *this ); }

    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL S                  sum() const { return math::sum( *this ); }
    DL_MATH_INL S                  dot( Q t ) const { return math::dot( *this, t ); }
    DL_MATH_INL S                  normSq() const { return math::normSq( *this ); }
    DL_MATH_INL S                  norm() const { return math::norm( *this ); }

    DL_MATH_INL S                  scalar() const { return math::scalar( *this ); }
    DL_MATH_INL V3                 vector() const { return math::vector( *this ); }

    DL_MATH_INL Q                  conjugate() const { return math::conjugate( *this ); }
    DL_MATH_INL Q                  inverse() const { return math::inverse( *this ); }
    DL_MATH_INL Q                  product( Q q ) const { return math::product( *this, q ); }

    static DL_MATH_INL Q           fromAA( V3 axis, S angle ) { return math::quatFromAA( axis, angle ); }
    DL_MATH_INL void               toAA( V3& axis, S& angle ) const { math::quattoAA( *this, axis, angle ); }

    template<typename S2> DL_MATH_INL Q lerp( Q q, S2 t ) const { return math::lerp( *this, q, t ); }
    template<typename S2> DL_MATH_INL Q nlerp( Q q, S2 t ) const { return math::nlerp( *this, q, t ); }
    template<typename S2> DL_MATH_INL Q slerp( Q q, S2 t ) const { return math::slerp( *this, q, t ); }
};

//=================================================================================================
// Matrix 3x3.
//=================================================================================================

template<typename S> struct MatT<3, S> DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 3, S );

    typedef M3 M;
    typedef V3 V;

    union
    {
        struct { V3 c0, c1, c2; };
        struct { V3 v0, v1, v2; };
        struct { V3 xaxis, yaxis, zaxis; };
        struct { V3 tangent, bitangent, normal; };
        struct { S m00, m01, m02, m10, m11, m12, m20, m21, m22; };
        struct { S A11, A12, A13, A21, A22, A23, A31, A32, A33; };
        S m[N*N];
        V3 comp[N];
    };

    static const M3 zero;
    static const M3 identity;

    static DL_MATH_INL M3 make()
    {
        return make( V3::unitX, V3::unitY, V3::unitZ );
    }

    static DL_MATH_INL M3 make( const S p[N] )
    {
        return op::from_ptr<M3>( p );
    }

    template<typename S2>
    static DL_MATH_INL M3 make(
    S2 m00, S2 m01, S2 m02,
    S2 m10, S2 m11, S2 m12,
    S2 m20, S2 m21, S2 m22 )
    {
        M3 o;
        o.m00 = ( S )m00;
        o.m01 = ( S )m01;
        o.m02 = ( S )m02;
        o.m10 = ( S )m10;
        o.m11 = ( S )m11;
        o.m12 = ( S )m12;
        o.m20 = ( S )m20;
        o.m21 = ( S )m21;
        o.m22 = ( S )m22;
        return o;
    }

    static DL_MATH_INL M3 make( V3 x, V3 y, V3 z )
    {
        return make( x.x, x.y, x.z, y.x, y.y, y.z, z.x, z.y, z.z );
    }

    template<typename T>
    static DL_MATH_INL M3 make( T x, T y, T z )
    {
        return make( ( V3 )x, ( V3 )y, ( V3 )z );
    }

    template<typename Rot, typename Sca>
    static DL_MATH_INL M3 make( Rot rotation, Sca scale )
    {
        M3 rot = ( M3 )rotation;
        V3 sca = ( V3 )scale;
        return make(
               rot.m00*sca.x, rot.m01*sca.x, rot.m02*sca.x,
               rot.m10*sca.y, rot.m11*sca.y, rot.m12*sca.y,
               rot.m20*sca.z, rot.m21*sca.z, rot.m22*sca.z
               );
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator MatT<3, S2>() const
    {
        return MatT<3, S2>::make( m00, m01, m02, m10, m11, m12, m20, m21, m22 );
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator MatT<4, S2>() const
    {
        return MatT<4, S2>::make( m00, m01, m02, ( S )0, m10, m11, m12, ( S )0, m20, m21, m22, ( S )0, ( S )0, ( S )0, ( S )0, ( S )1 );
    }

    DL_EXPLICIT DL_MATH_INL       operator Q()  const { return op::op_m2q<S, Q>( *this ); }
    DL_EXPLICIT DL_MATH_INL       operator XF() const { return XF::make( *this ); }

    DL_MATH_INL const V3&          operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL Bool               isUnit() const { return math::isUnit( *this ); }
    DL_MATH_INL Bool               isZero() const { return math::isZero( *this ); }
    DL_MATH_INL Bool               isIdentity() const { return math::isIdentity( *this ); }
    DL_MATH_INL Bool               isSingular() const { return math::isSingular( *this ); }
    DL_MATH_INL Bool               isReflection() const { return math::isReflection( *this ); }
    DL_MATH_INL Bool               isNormalized() const { return math::isNormalized( *this ); }
    DL_MATH_INL Bool               isOrtho() const { return math::isOrtho( *this ); }
    DL_MATH_INL Bool               isOrthoNormal() const { return math::isOrthoNormal( *this ); }

    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL M3                 unit() const { return math::unit( *this ); }
    DL_MATH_INL S                  trace() const { return math::trace( *this ); }
    DL_MATH_INL V3                 diagonal() const { return math::diagonal( *this ); }
    DL_MATH_INL V3                 scale() const { return math::scale( *this ); }
    DL_MATH_INL M3                 rotation() const { return math::rotation( *this ); }
    DL_MATH_INL V3                 eulers() const { return math::eulers( *this ); }
    DL_MATH_INL M3                 transpose() const { return math::transpose( *this ); }
    DL_MATH_INL S                  determinant() const { return math::determinant( *this ); }
    DL_MATH_INL M3                 inverse() const { return math::inverse( *this ); }
    DL_MATH_INL M3                 inverseTranspose() const { return math::inverseTranspose( *this ); }

    DL_MATH_INL M3                 nlerp( const M3& other, S t ) { return math::nlerp( *this, other, t ); }
    DL_MATH_INL M3                 slerp( const M3& other, S t ) { return math::slerp( *this, other, t ); }
    DL_MATH_INL M3                 fnlerp( const M3& other, S t ) { return math::fnlerp( *this, other, t ); }
    DL_MATH_INL M3                 onlerp( const M3& other, S t ) { return math::onlerp( *this, other, t ); }

    template<typename S2> static DL_MATH_INL M3 makeScale( S2 x, S2 y, S2 z ) { return math::makeScale<N>( ( S )x, ( S )y, ( S )z ); }
    template<typename Vec> static DL_MATH_INL M3 makeScale( Vec v ) { return makeScale( ( S )v.x, ( S )v.y, ( S )v.z ); }
    template<typename S2> static DL_MATH_INL M3 makeRotationX( S2 r ) { return math::makeRotationX<N>( ( S )r ); }
    template<typename S2> static DL_MATH_INL M3 makeRotationY( S2 r ) { return math::makeRotationY<N>( ( S )r ); }
    template<typename S2> static DL_MATH_INL M3 makeRotationZ( S2 r ) { return math::makeRotationZ<N>( ( S )r ); }
    template<typename S2> static DL_MATH_INL M3 makeRotation( S2 x, S2 y, S2 z, const char* order = "xyz" ) { return math::makeRotation<N>( ( S )x, ( S )y, ( S )z, order ); }
};

//=================================================================================================
// Matrix 4x4.
//=================================================================================================

template<typename S> struct MatT<4, S> DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 4, S );

    typedef M4 M;
    typedef V4 V;

    union
    {
        struct { V4 c0, c1, c2, c3; };
        struct { V4 v0, v1, v2, v3; };
        struct { V4 xaxis, yaxis, zaxis, position; };
        struct { S m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33; };
        struct { S A11, A12, A13, A14, A21, A22, A23, A24, A31, A32, A33, A34, A41, A42, A43, A44; };
        S m[N*N];
        V4 comp[N];
    };

    static const M4 zero;
    static const M4 identity;

    static DL_MATH_INL M4 make()
    {
        return make( V4::unitX, V4::unitY, V4::unitZ, V4::unitW );
    }

    static DL_MATH_INL M4 make( const S p[N] )
    {
        return op::from_ptr<M4>( p );
    }

    template<typename S2>
    static DL_MATH_INL M4 make(
    S2 m00, S2 m01, S2 m02, S2 m03,
    S2 m10, S2 m11, S2 m12, S2 m13,
    S2 m20, S2 m21, S2 m22, S2 m23,
    S2 m30, S2 m31, S2 m32, S2 m33 )
    {
        M4 o;
        o.m00 = ( S )m00;
        o.m01 = ( S )m01;
        o.m02 = ( S )m02;
        o.m03 = ( S )m03;
        o.m10 = ( S )m10;
        o.m11 = ( S )m11;
        o.m12 = ( S )m12;
        o.m13 = ( S )m13;
        o.m20 = ( S )m20;
        o.m21 = ( S )m21;
        o.m22 = ( S )m22;
        o.m23 = ( S )m23;
        o.m30 = ( S )m30;
        o.m31 = ( S )m31;
        o.m32 = ( S )m32;
        o.m33 = ( S )m33;
        return o;
    }

    static DL_MATH_INL M4 make( V4 x, V4 y, V4 z, V4 t )
    {
        return make( x.x, x.y, x.z, x.w, y.x, y.y, y.z, y.w, z.x, z.y, z.z, z.w, t.x, t.y, t.z, t.w );
    }

    static DL_MATH_INL M4 make( V3 x, V3 y, V3 z, V3 t = V3::zero )
    {
        return make( ( V4 )x, ( V4 )y, ( V4 )z, V4::make( t.x, t.y, t.z, 1 ) );
    }

    template<typename Pos, typename Rot, typename Sca>
    static DL_MATH_INL M4 make( Pos position, Rot rotation, Sca scale )
    {
        P3 pos = ( P3 )position;
        M3 rot = ( M3 )rotation;
        V3 sca = ( V3 )scale;
        return make(
               rot.m00*sca.x, rot.m01*sca.x, rot.m02*sca.x, ( S )0,
               rot.m10*sca.y, rot.m11*sca.y, rot.m12*sca.y, ( S )0,
               rot.m20*sca.z, rot.m21*sca.z, rot.m22*sca.z, ( S )0,
               pos.x, pos.y, pos.z, ( S )1
               );
    }

    DL_EXPLICIT DL_MATH_INL       operator Q()  const { return op::op_m2q<S, Q>( *this ); }
    DL_EXPLICIT DL_MATH_INL       operator XF() const { return XF::make( *this ); }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator MatT<4, S2>() const
    {
        return MatT<4, S2>::make( m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33 );
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator MatT<3, S2>() const
    {
        return MatT<3, S2>::make( m00, m01, m02, m10, m11, m12, m20, m21, m22 );
    }

    DL_MATH_INL const V4&          operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL Bool               isUnit() const { return math::isUnit( *this ); }
    DL_MATH_INL Bool               isZero() const { return math::isZero( *this ); }
    DL_MATH_INL Bool               isIdentity() const { return math::isIdentity( *this ); }
    DL_MATH_INL Bool               isSingular() const { return math::isSingular( *this ); }
    DL_MATH_INL Bool               isReflection() const { return math::isReflection( *this ); }
    DL_MATH_INL Bool               isNormalized() const { return math::isNormalized( *this ); }
    DL_MATH_INL Bool               isOrtho() const { return math::isOrtho( *this ); }
    DL_MATH_INL Bool               isOrthoNormal() const { return math::isOrthoNormal( *this ); }

    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL M4                 unit() const { return math::unit( *this ); }
    DL_MATH_INL S                  trace() const { return math::trace( *this ); }
    DL_MATH_INL V4                 diagonal() const { return math::diagonal( *this ); }
    DL_MATH_INL V3                 scale() const { return math::scale( *this ); }
    DL_MATH_INL M3                 rotation() const { return math::rotation( *this ); }
    DL_MATH_INL V3                 eulers() const { return math::eulers( *this ); }
    DL_MATH_INL M4                 transpose() const { return math::transpose( *this ); }
    DL_MATH_INL S                  determinant() const { return math::determinant( *this ); }
    DL_MATH_INL M4                 inverse() const { return math::inverse( *this ); }
    DL_MATH_INL M4                 inverseTranspose() const { return math::inverseTranspose( *this ); }

    DL_MATH_INL Bool               isAffine() const { return math::isAffine( *this ); }
    DL_MATH_INL Bool               isTranslation() const { return math::isTranslation( *this ); }
    DL_MATH_INL V3                 translation() const { return math::translation( *this ); }

    DL_MATH_INL M4                 nlerp( const M4& other, S t ) { return math::nlerp( *this, other, t ); }
    DL_MATH_INL M4                 slerp( const M4& other, S t ) { return math::slerp( *this, other, t ); }
    DL_MATH_INL M4                 fnlerp( const M4& other, S t ) { return math::fnlerp( *this, other, t ); }
    DL_MATH_INL M4                 onlerp( const M4& other, S t ) { return math::onlerp( *this, other, t ); }

    template<typename S2> static DL_MATH_INL M4 makeScale( S2 x, S2 y, S2 z ) { return math::makeScale<N>( ( S )x, ( S )y, ( S )z ); }
    template<typename Vec> static DL_MATH_INL M4 makeScale( Vec v ) { return makeScale( ( S )v.x, ( S )v.y, ( S )v.z ); }
    template<typename S2> static DL_MATH_INL M4 makeRotationX( S2 r ) { return math::makeRotationX<N>( ( S )r ); }
    template<typename S2> static DL_MATH_INL M4 makeRotationY( S2 r ) { return math::makeRotationY<N>( ( S )r ); }
    template<typename S2> static DL_MATH_INL M4 makeRotationZ( S2 r ) { return math::makeRotationZ<N>( ( S )r ); }
    template<typename S2> static DL_MATH_INL M4 makeRotation( S2 x, S2 y, S2 z, const char* order = "xyz" ) { return math::makeRotation<N>( ( S )x, ( S )y, ( S )z, order ); }
    template<typename S2> static DL_MATH_INL M4 makeTranslation( S2 x, S2 y, S2 z ) { return math::makeTranslation<S>( ( S )x, ( S )y, ( S )z ); }
};

//=================================================================================================
// Xform.
//=================================================================================================

template<typename S> struct XformT DL_FINAL
{
    DL_MATH_MAKE_TYPEDEFS( S )

    P3 position;
    Q  rotation;
    V3 scale;

    static const XF identity;

    static DL_MATH_INL XF make()
    {
        return XF::identity;
    }

    template<typename Pos, typename Rot, typename Sca>
    static DL_MATH_INL XF make( Pos position, Rot rotation, Sca scale )
    {
        XF o;
        o.position = ( P3 )position;
        o.rotation = ( Q )rotation;
        o.scale = ( V3 )scale;
        return o;
    }

    //
    // NOTE: decomposition will fail for matrices containing shear, or which have had rotations
    // applied after scaling. If you cannot guarantee that these conditions are met, it will be
    // better to leave your transformation in matrix form.
    //
    static DL_MATH_INL XF          make( const M3& m ) { return make( P3::zero, m.rotation(), m.scale() ); }
    static DL_MATH_INL XF          make( const M4& m ) { return make( m.translation(), m.rotation(), m.scale() ); }

    DL_EXPLICIT DL_MATH_INL       operator M3() const { return M3::make( rotation, scale ); }
    DL_EXPLICIT DL_MATH_INL       operator M4() const { return M4::make( position, rotation, scale ); }

    DL_MATH_INL XF                 nlerp( const XF& other, S t ) { return math::nlerp( *this, other, t ); }
    DL_MATH_INL XF                 slerp( const XF& other, S t ) { return math::slerp( *this, other, t ); }
    DL_MATH_INL XF                 fnlerp( const XF& other, S t ) { return math::fnlerp( *this, other, t ); }
    DL_MATH_INL XF                 onlerp( const XF& other, S t ) { return math::onlerp( *this, other, t ); }
};

//=================================================================================================
// AABB.
//=================================================================================================

template<typename S> struct AabbT DL_FINAL
{
    DL_MATH_MAKE_TYPEDEFS( S )

    P3 min;
    P3 max;

    static const AABB identity;

    static DL_MATH_INL AABB make()
    {
        return AABB::identity;
    }

    template<typename T0, typename T1>
    static DL_MATH_INL AABB make( T0 min, T1 max )
    {
        AABB o;
        o.min = ( P3 )min;
        o.max = ( P3 )max;
        return o;
    }
};

//=================================================================================================
// OBB.
//=================================================================================================

template<typename S> struct ObbT DL_FINAL
{
    DL_MATH_MAKE_TYPEDEFS( S )

    M4 frame;
    V3 size;

    static const OBB identity;

    static DL_MATH_INL OBB make()
    {
        return OBB::identity;
    }

    template<typename Mat, typename Vec>
    static DL_MATH_INL OBB make( Mat frame, Vec size )
    {
        OBB o;
        o.frame = ( M4 )frame;
        o.size = ( V3 )size;
        return o;
    }
};

//=================================================================================================
// Rgb.
//=================================================================================================

template<typename S> struct RgbT DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 3, S );

    union
    {
        struct { S r, g, b; };
        struct { S x, y, z; };
        S m[N];
        S comp[N];
        V3 v3;
        V3 xyz;
        P3 p3;
    };

    static const C3 black;
    static const C3 white;
    static const C3 grey;
    static const C3 red;
    static const C3 green;
    static const C3 blue;
    static const C3 yellow;
    static const C3 cyan;
    static const C3 magenta;

    static DL_MATH_INL C3 make()
    {
        return make( 0, 0, 0 );
    }

    static DL_MATH_INL C3 make( const S p[N] )
    {
        return make( p[0], p[1], p[2] );
    }

    template<typename S2, typename S3, typename S4>
    static DL_MATH_INL C3 make( S2 r, S3 g, S4 b )
    {
        if ( ( S )r > ( S )1 || ( S )g > ( S )1 || ( S )b > ( S )1 )
        {
            C3 o;
            o.r = ( S )r/( S )255;
            o.g = ( S )g/( S )255;
            o.b = ( S )b/( S )255;
            return o;
        }
        return C3 { ( S )r, ( S )g, ( S )b };
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator RgbT<S2>() const
    {
        return RgbT<S2>::make( r, g, b );
    }

    DL_EXPLICIT DL_MATH_INL       operator V3() const { return v3; }
    DL_EXPLICIT DL_MATH_INL       operator P3() const { return p3; }
    DL_EXPLICIT DL_MATH_INL       operator V4() const { return V4 { r, g, b, ( S )0 }; }
    DL_EXPLICIT DL_MATH_INL       operator C4() const { return C4 { r, g, b, ( S )1 }; }
    DL_EXPLICIT DL_MATH_INL       operator HSV() const { return math::toHsv( *this ); }
    DL_EXPLICIT DL_MATH_INL       operator HSL() const { return math::toHsl( *this ); }

    DL_EXPLICIT DL_MATH_INL       operator Rgb8() const;
    DL_EXPLICIT DL_MATH_INL       operator Rgba8() const;

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL C3                 abs() const { return math::abs( *this ); }
    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL C3                 round() const { return math::round( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  sum() const { return math::sum( *this ); }

    DL_MATH_INL C3 inv() const
    {
        return C3
        {
            ( S )1 - r,
            ( S )1 - g,
            ( S )1 - b
        };
    }
};

//=================================================================================================
// Rgb8.
//=================================================================================================

template<> struct RgbT<UInt8> DL_FINAL
{
    DL_MATH_SETUP_STRUCT_WITH_S( 3, UInt8 );

    union
    {
        struct { S r, g, b; };
        struct { S x, y, z; };
        S m[N];
        S comp[N];
        V3 v3;
        V3 xyz;
    };

    static DL_MATH_INL C3 make()
    {
        return make( ( S )0, ( S )0, ( S )0 );
    }

    static DL_MATH_INL C3 make( const S p[N] )
    {
        return op::from_ptr<C3>( p );
    }

    static DL_MATH_INL C3 make( S r, S g, S b )
    {
        C3 o;
        o.r = clamp<S>( r, 0, 255 );
        o.g = clamp<S>( g, 0, 255 );
        o.b = clamp<S>( b, 0, 255 );
        return o;
    }

    static C3 make( Double r, Double g, Double b );
    static C3 make( Float r, Float g, Float b );

    template<typename S2>
    static DL_MATH_INL C3 make( S2 r, S2 g, S2 b )
    {
        return make( ( S ) r, ( S ) g, ( S ) b );
    }

    DL_EXPLICIT DL_MATH_INL       operator Rgbf() const;
    DL_EXPLICIT DL_MATH_INL       operator Rgbd() const;

    DL_EXPLICIT DL_MATH_INL       operator Rgbaf() const;
    DL_EXPLICIT DL_MATH_INL       operator Rgbad() const;
    DL_EXPLICIT DL_MATH_INL       operator Rgba8() const;

    DL_EXPLICIT DL_MATH_INL       operator Hsvf() const;
    DL_EXPLICIT DL_MATH_INL       operator Hsvd() const;

    DL_EXPLICIT DL_MATH_INL       operator Hslf() const;
    DL_EXPLICIT DL_MATH_INL       operator Hsld() const;

    DL_EXPLICIT DL_MATH_INL       operator Argb8() const;
    DL_EXPLICIT DL_MATH_INL       operator ColorRef() const;

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL C3                 abs() const { return math::abs( *this ); }
    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL C3                 round() const { return math::round( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  sum() const { return math::sum( *this ); }

    DL_MATH_INL C3 inv() const
    {
        return C3
        {
            ( S )( 255 - r ),
            ( S )( 255 - g ),
            ( S )( 255 - b )
        };
    }
};

DL_MATH_INL RgbT<UInt8> RgbT<UInt8>::make( Double r, Double g, Double b )
{
    return RgbT<UInt8>::make(
           math::round<UInt8>( r*255.0 ),
           math::round<UInt8>( g*255.0 ),
           math::round<UInt8>( b*255.0 )
           );
}

DL_MATH_INL RgbT<UInt8> RgbT<UInt8>::make( Float r, Float g, Float b )
{
    return RgbT<UInt8>::make(
           math::round<UInt8>( r*255.0f ),
           math::round<UInt8>( g*255.0f ),
           math::round<UInt8>( b*255.0f )
           );
}

//=================================================================================================
// Rgba.
//=================================================================================================

template<typename S> struct RgbaT DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 4, S );

    union
    {
        struct { S r, g, b, a; };
        struct { S x, y, z, w; };
        S m[N];
        S comp[N];
        C3 c3;
        C3 rgb;
        V3 v3;
        V3 xyz;
        P3 p3;
        V4 v4;
        V4 xyzw;
    };

    static const C4 black;
    static const C4 white;
    static const C4 grey;
    static const C4 red;
    static const C4 green;
    static const C4 blue;
    static const C4 yellow;
    static const C4 cyan;
    static const C4 magenta;

    static DL_MATH_INL C4 make()
    {
        return make( 0, 0, 0, 1 );
    }

    static DL_MATH_INL C4 make( const S p[N] )
    {
        return make( p[0], p[1], p[2], p[3] );
    }

    static DL_MATH_INL C4 make( S r, S g, S b, S a )
    {
        if ( ( S )r > ( S )1 || ( S )g > ( S )1 || ( S )b > ( S )1 || ( S )a > ( S )1 )
        {
            C4 o;
            o.r = ( S )r/( S )255;
            o.g = ( S )g/( S )255;
            o.b = ( S )b/( S )255;
            o.a = ( S )a/( S )255;
            return o;
        }
        return C4 { ( S )r, ( S )g, ( S )b, ( S )a };
    }

    template<typename S2, typename S3, typename S4, typename S5>
    static DL_MATH_INL C4 make( S2 r, S3 g, S4 b, S5 a )
    {
        return make( ( S )( r ), ( S )( g ), ( S )( b ), ( S )( a ) );
    }

    template<typename S2, typename S3, typename S4>
    static DL_MATH_INL C4 make( S2 r, S3 g, S4 b )
    {
        return make( r, g, b, 1 );
    }

    template<typename S2>
    static DL_MATH_INL C4 make( C3 c, S2 s )
    {
        return make( c.r, c.g, c.b, s );
    }

    static DL_MATH_INL C4 make( C3 c )
    {
        return make( c.r, c.g, c.b );
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator RgbaT<S2>() const
    {
        return RgbaT<S2>::make( r, g, b, a );
    }

    DL_EXPLICIT DL_MATH_INL       operator C3() const { return c3; }
    DL_EXPLICIT DL_MATH_INL       operator V3() const { return v3; }
    DL_EXPLICIT DL_MATH_INL       operator P3() const { return p3; }
    DL_EXPLICIT DL_MATH_INL       operator V4() const { return v4; }
    DL_EXPLICIT DL_MATH_INL       operator HSV() const { return math::toHsv( *this ); }
    DL_EXPLICIT DL_MATH_INL       operator HSL() const { return math::toHsl( *this ); }

    DL_EXPLICIT DL_MATH_INL       operator Rgb8() const;
    DL_EXPLICIT DL_MATH_INL       operator Rgba8() const;

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL C4                 abs() const { return math::abs( *this ); }
    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL C4                 round() const { return math::round( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  sum() const { return math::sum( *this ); }
    
    DL_MATH_INL C4 inv( bool invAlpha = true ) const
    {
        return C4
        {
            ( S )1 - r,
            ( S )1 - g,
            ( S )1 - b,
            invAlpha ? ( S )1 - a : a
        };
    }
};

//=================================================================================================
// Rgba8.
//=================================================================================================

template<> struct RgbaT<UInt8> DL_FINAL
{
    DL_MATH_SETUP_STRUCT_WITH_S( 4, UInt8 );

    union
    {
        struct { S r, g, b, a; };
        struct { S x, y, z, w; };
        S m[N];
        S comp[N];
        C3 c3;
        C3 rgb;
        V3 v3;
        V3 xyz;
        V4 v4;
        V4 xyzw;
    };

    static DL_MATH_INL C4 make()
    {
        return make( ( S )0, ( S )0, ( S )0, ( S ) 0 );
    }

    static DL_MATH_INL C4 make( const S p[N] )
    {
        return op::from_ptr<C4>( p );
    }

    static DL_MATH_INL C4 make( S r, S g, S b, S a )
    {
        C4 o;
        o.r = clamp<S>( r, 0, 255 );
        o.g = clamp<S>( g, 0, 255 );
        o.b = clamp<S>( b, 0, 255 );
        o.a = clamp<S>( a, 0, 255 );
        return o;
    }

    template<typename S2>
    static DL_MATH_INL C4 make( S2 r, S2 g, S2 b, S2 a )
    {
        return make( ( S )r, ( S )g, ( S )b, ( S )a );
    }

    static C4 make( Double r, Double g, Double b, Double a );
    static C4 make( Float r, Float g, Float b, Float a );

    template<typename S2>
    static DL_MATH_INL C4 make( S2 r, S2 g, S2 b )
    {
        return make( r, g, b, ( S2 )1 );
    }

    DL_EXPLICIT DL_MATH_INL       operator Rgbf() const;
    DL_EXPLICIT DL_MATH_INL       operator Rgbd() const;
    DL_EXPLICIT DL_MATH_INL       operator Rgb8() const;

    DL_EXPLICIT DL_MATH_INL       operator Rgbaf() const;
    DL_EXPLICIT DL_MATH_INL       operator Rgbad() const;

    DL_EXPLICIT DL_MATH_INL       operator Hsvf() const;
    DL_EXPLICIT DL_MATH_INL       operator Hsvd() const;

    DL_EXPLICIT DL_MATH_INL       operator Hslf() const;
    DL_EXPLICIT DL_MATH_INL       operator Hsld() const;

    DL_EXPLICIT DL_MATH_INL       operator Argb8() const;
    DL_EXPLICIT DL_MATH_INL       operator ColorRef() const;

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL C4                 abs() const { return math::abs( *this ); }
    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL C4                 round() const { return math::round( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  sum() const { return math::sum( *this ); }

    DL_MATH_INL C4 inv( bool invAlpha = true ) const
    {
        return C4
        {
            ( S )( 255 - r ),
            ( S )( 255 - g ),
            ( S )( 255 - b ),
            ( S )( invAlpha ? ( S )255 - a : a )
        };
    }
};

DL_MATH_INL RgbaT<UInt8> RgbaT<UInt8>::make( Double r, Double g, Double b, Double a )
{
    return RgbaT<UInt8>::make(
           math::round<UInt8>( r*255.1 ),
           math::round<UInt8>( g*255.1 ),
           math::round<UInt8>( b*255.1 ),
           math::round<UInt8>( a*255.1 )
           );
}

DL_MATH_INL RgbaT<UInt8> RgbaT<UInt8>::make( Float r, Float g, Float b, Float a )
{
    return RgbaT<UInt8>::make(
           math::round<UInt8>( r*255.1f ),
           math::round<UInt8>( g*255.1f ),
           math::round<UInt8>( b*255.1f ),
           math::round<UInt8>( a*255.1f )
           );
}

//=================================================================================================
// Hsv.
//=================================================================================================

template<typename S> struct HsvT DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 3, S );

    union
    {
        struct { S h, s, v; };
        struct { S x, y, z; };
        S m[N];
        S comp[N];
        V3 v3;
    };

    static DL_MATH_INL HSV make()
    {
        return make( 0, 0, 0 );
    }

    static DL_MATH_INL HSV make( const S p[N] )
    {
        return op::from_ptr<HSV>( p );
    }

    static DL_MATH_INL HSV make( S h, S s, S v )
    {
        HSV o;
        o.h = h;
        o.s = s;
        o.v = v;
        return o;
    }

    template<typename S2, typename S3, typename S4>
    static DL_MATH_INL HSV make( S2 h, S3 s, S4 v )
    {
        return make( ( S ) h, ( S ) s, ( S ) v );
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator HsvT<S2>() const
    {
        return HsvT<S2>::make( h, s, v );
    }

    DL_EXPLICIT DL_MATH_INL       operator V3() const { return v3; }
    DL_EXPLICIT DL_MATH_INL       operator C3() const { return math::toRgb( *this ); }
    DL_EXPLICIT DL_MATH_INL       operator C4() const { return math::toRgba( *this ); }

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL HSV                abs() const { return math::abs( *this ); }
    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  sum() const { return math::sum( *this ); }
};

//=================================================================================================
// Hsl.
//=================================================================================================

template<typename S> struct HslT DL_FINAL
{
    DL_MATH_SETUP_STRUCT( 3, S );

    union
    {
        struct { S h, s, l; };
        struct { S x, y, z; };
        S m[N];
        S comp[N];
        V3 v3;
    };

    static DL_MATH_INL HSL make()
    {
        return make( 0, 0, 0 );
    }

    static DL_MATH_INL HSL make( const S p[N] )
    {
        return op::from_ptr<HSL>( p );
    }

    static DL_MATH_INL HSL make( S h, S s, S l )
    {
        HSL o;
        o.h = h;
        o.s = s;
        o.l = l;
        return o;
    }

    template<typename S2, typename S3, typename S4>
    static DL_MATH_INL HSL make( S2 h, S3 s, S4 l )
    {
        return make( ( S )h, ( S )s, ( S )l );
    }

    template<typename S2>
    DL_EXPLICIT DL_MATH_INL operator HslT<S2>() const
    {
        return HslT<S2>::make( h, s, l );
    }

    DL_EXPLICIT DL_MATH_INL       operator V3() const { return v3; }
    DL_EXPLICIT DL_MATH_INL       operator C3() const { return math::toRgb( *this ); }
    DL_EXPLICIT DL_MATH_INL       operator C4() const { return math::toRgba( *this ); }

    DL_MATH_INL const S&           operator[]( Dim i ) const { return math::elem( *this, i ); }

    DL_MATH_INL UInt64             hash() const { return dl::ds::hash( *this ); }
    DL_MATH_INL HSL                abs() const { return math::abs( *this ); }
    DL_MATH_INL S                  min() const { return math::min( *this ); }
    DL_MATH_INL S                  max() const { return math::max( *this ); }
    DL_MATH_INL S                  avg() const { return math::avg( *this ); }
    DL_MATH_INL VecT<N, UInt32>    order() const { return math::order<N>( *this ); }
    DL_MATH_INL S                  sum() const { return math::sum( *this ); }
};

//=================================================================================================
// For working with ARGB environments like web & .NET.
//=================================================================================================

struct Argb8
{
    union
    {
        UInt32 u;
        struct { UInt8 b, g, r, a; };
    };

    static DL_MATH_INL Argb8 make( UInt32 u )
    {
        Argb8 o;
        o.u = u;
        return o;
    }

    static DL_MATH_INL Argb8 make( UInt8 a, UInt8 r, UInt8 g, UInt8 b )
    {
        Argb8 o;
        o.a = a;
        o.r = r;
        o.g = g;
        o.b = b;
        return o;
    }

    static DL_MATH_INL Argb8 make( Rgb8 c )
    {
        return make( 255, c.r, c.g, c.b );
    }

    static DL_MATH_INL Argb8 make( Rgba8 c )
    {
        return make( c.a, c.r, c.g, c.b );
    }

    template<typename T>
    static DL_MATH_INL Argb8 make( RgbT<T> c )
    {
        return make( ( Rgb8 )c );
    }

    template<typename T>
    static DL_MATH_INL Argb8 make( RgbaT<T> c )
    {
        return make( ( Rgba8 )c );
    }

    DL_MATH_INL Bool               operator==( Argb8 o ) { return u == o.u; }
    DL_MATH_INL Bool               operator!=( Argb8 o ) { return u != o.u; }

    DL_EXPLICIT DL_MATH_INL       operator UInt32() const { return u; }
    DL_EXPLICIT DL_MATH_INL       operator Rgb8() const { return Rgb8::make( r, g, b ); }
    DL_EXPLICIT DL_MATH_INL       operator Rgba8() const { return Rgba8::make( r, g, b, a ); }
    DL_EXPLICIT DL_MATH_INL       operator Rgbf() const { return ( Rgbf )( Rgb8 )*this; }
    DL_EXPLICIT DL_MATH_INL       operator Rgbaf() const { return ( Rgbaf )( Rgba8 )*this; }
    DL_EXPLICIT DL_MATH_INL       operator Rgbd() const { return ( Rgbd )( Rgb8 )*this; }
    DL_EXPLICIT DL_MATH_INL       operator Rgbad() const { return ( Rgbad )( Rgba8 )*this; }
    DL_EXPLICIT DL_MATH_INL       operator ColorRef() const;
};

//=================================================================================================
// For use with Microsoft's COLORREF struct. NOTE that the order is BGR.
//=================================================================================================

struct ColorRef
{
    union
    {
        UInt32 u;
        struct { UInt8 r, g, b, unused; };
    };

    static DL_MATH_INL ColorRef make( UInt32 u )
    {
        ColorRef o;
        o.u = u;
        o.unused = ( UInt8 )0; // Per MSDN.
        return o;
    }

    static DL_MATH_INL ColorRef make( UInt8 b, UInt8 g, UInt8 r )
    {
        ColorRef o;
        o.b = b;
        o.g = g;
        o.r = r;
        o.unused = ( UInt8 )0; // Per MSDN.
        return o;
    }

    static DL_MATH_INL ColorRef make( Rgb8 c )
    {
        return make( c.b, c.g, c.r );
    }

    static DL_MATH_INL ColorRef make( Rgba8 c )
    {
        return make( c.b, c.g, c.r );
    }

    template<typename T>
    static DL_MATH_INL ColorRef make( RgbT<T> c )
    {
        return make( ( Rgb8 )c );
    }

    template<typename T>
    static DL_MATH_INL ColorRef make( RgbaT<T> c )
    {
        return make( ( Rgb8 )c );
    }

    DL_MATH_INL Bool               operator==( ColorRef o ) { return u == o.u; }
    DL_MATH_INL Bool               operator!=( ColorRef o ) { return u != o.u; }

    DL_EXPLICIT DL_MATH_INL       operator UInt32() const { return u; }
    DL_EXPLICIT DL_MATH_INL       operator Rgb8() const { return Rgb8::make( r, g, b ); }
    DL_EXPLICIT DL_MATH_INL       operator Rgba8() const { return Rgba8::make( r, g, b ); }
    DL_EXPLICIT DL_MATH_INL       operator Rgbf() const { return ( Rgbf )( Rgb8 )*this; }
    DL_EXPLICIT DL_MATH_INL       operator Rgbaf() const { return ( Rgbaf )( Rgba8 )*this; }
    DL_EXPLICIT DL_MATH_INL       operator Rgbd() const { return ( Rgbd )( Rgb8 )*this; }
    DL_EXPLICIT DL_MATH_INL       operator Rgbad() const { return ( Rgbad )( Rgba8 )*this; }
    DL_EXPLICIT DL_MATH_INL       operator Argb8() const;
};

//=================================================================================================
// Region struct.
//=================================================================================================

template<typename S> struct RegionT
{
    VecT<2, S> size;
    VecT<2, S> offset;

    RegionT() :
        size( VecT<2, S>::zero ), offset( VecT<2, S>::zero ) { }

    RegionT( VecT<2, S> size, VecT<2, S> offset ) :
        size( size ), offset( offset ) { }

    S width() const         { return size.x; }
    S height() const        { return size.y; }
    S area() const          { return size.x * size.y; }

    S left() const          { return offset.x; }
    S top() const           { return offset.y; }
    S right() const         { return offset.x + size.x; }
    S bottom() const        { return offset.y + size.y; }

    Bool isEmpty() const    { return size == VecT<2, S>::zero; }
};

//=================================================================================================
// Global operations.
//=================================================================================================

DL_MATH_INL Vec2i project( Vec2i v, Vec2i n ) { return ( Vec2i )op::project<Double, Vec2d>( ( Vec2d )v, ( Vec2d )n ); }
DL_MATH_INL Vec3i project( Vec3i v, Vec3i n ) { return ( Vec3i )op::project<Double, Vec3d>( ( Vec3d )v, ( Vec3d )n ); }

DL_MATH_INL Vec2i reflect( Vec2i v, Vec2i n ) { return ( Vec2i )op::reflect<Double, Vec2d>( ( Vec2d )v, ( Vec2d )n ); }
DL_MATH_INL Vec3i reflect( Vec3i v, Vec3i n ) { return ( Vec3i )op::reflect<Double, Vec3d>( ( Vec3d )v, ( Vec3d )n ); }

DL_MATH_INL Double distance( Vec2i a, Vec2i b ) { return ( Double )op::distance<Double, Vec2d>( ( Vec2d )a, ( Vec2d )b ); }
DL_MATH_INL Double distance( Vec3i a, Vec3i b ) { return ( Double )op::distance<Double, Vec3d>( ( Vec3d )a, ( Vec3d )b ); }

DL_MATH_INL Pos2i project( Pos2i pt, Pos2i pl, Vec2i n ) { return ( Pos2i )op::project<Double, Pos2d>( ( Pos2d )pt, ( Pos2d )pl, ( Vec2d )n ); }
DL_MATH_INL Pos3i project( Pos3i pt, Pos3i pl, Vec3i n ) { return ( Pos3i )op::project<Double, Pos3d>( ( Pos3d )pt, ( Pos3d )pl, ( Vec3d )n ); }

DL_MATH_INL Pos2i reflect( Pos2i pt, Pos2i pl, Vec2i n ) { return ( Pos2i )op::reflect<Double, Pos2d>( ( Pos2d )pt, ( Pos2d )pl, ( Vec2d )n ); }
DL_MATH_INL Pos3i reflect( Pos3i pt, Pos3i pl, Vec3i n ) { return ( Pos3i )op::reflect<Double, Pos3d>( ( Pos3d )pt, ( Pos3d )pl, ( Vec3d )n ); }

//=================================================================================================
// Initialize static fields.
//=================================================================================================

// Vec2.
template<typename S> const VecT<2, S> VecT<2, S>::zero      = VecT<2, S>::make( 0, 0 );
template<typename S> const VecT<2, S> VecT<2, S>::one       = VecT<2, S>::make( 1, 1 );
template<typename S> const VecT<2, S> VecT<2, S>::unitX     = VecT<2, S>::make( 1, 0 );
template<typename S> const VecT<2, S> VecT<2, S>::unitY     = VecT<2, S>::make( 0, 1 );

// Pos2.
template<typename S> const PosT<2, S> PosT<2, S>::zero      = PosT<2, S>::make( 0, 0 );

// Vec3.
template<typename S> const VecT<3, S> VecT<3, S>::zero      = VecT<3, S>::make( 0, 0, 0 );
template<typename S> const VecT<3, S> VecT<3, S>::one       = VecT<3, S>::make( 1, 1, 1 );
template<typename S> const VecT<3, S> VecT<3, S>::unitX     = VecT<3, S>::make( 1, 0, 0 );
template<typename S> const VecT<3, S> VecT<3, S>::unitY     = VecT<3, S>::make( 0, 1, 0 );
template<typename S> const VecT<3, S> VecT<3, S>::unitZ     = VecT<3, S>::make( 0, 0, 1 );

// Pos3.
template<typename S> const PosT<3, S> PosT<3, S>::zero      = PosT<3, S>::make( 0, 0, 0 );

// Vec4.
template<typename S> const VecT<4, S> VecT<4, S>::zero      = VecT<4, S>::make( 0, 0, 0, 0 );
template<typename S> const VecT<4, S> VecT<4, S>::one       = VecT<4, S>::make( 1, 1, 1, 1 );
template<typename S> const VecT<4, S> VecT<4, S>::unitX     = VecT<4, S>::make( 1, 0, 0, 0 );
template<typename S> const VecT<4, S> VecT<4, S>::unitY     = VecT<4, S>::make( 0, 1, 0, 0 );
template<typename S> const VecT<4, S> VecT<4, S>::unitZ     = VecT<4, S>::make( 0, 0, 1, 0 );
template<typename S> const VecT<4, S> VecT<4, S>::unitW     = VecT<4, S>::make( 0, 0, 0, 1 );

// Quat.
template<typename S> const QuatT<S> QuatT<S>::zero          = QuatT<S>::make( 0, 0, 0, 0 );
template<typename S> const QuatT<S> QuatT<S>::identity      = QuatT<S>::make( 0, 0, 0, 1 );

// Mat3.
template<typename S> const MatT<3, S> MatT<3, S>::zero      = MatT<3, S>::make( 0, 0, 0, 0, 0, 0, 0, 0, 0 );
template<typename S> const MatT<3, S> MatT<3, S>::identity  = MatT<3, S>::make( 1, 0, 0, 0, 1, 0, 0, 0, 1 );

// Mat4.
template<typename S> const MatT<4, S> MatT<4, S>::zero      = MatT<4, S>::make( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
template<typename S> const MatT<4, S> MatT<4, S>::identity  = MatT<4, S>::make( 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 );

// Xform.
template<typename S> const XformT<S> XformT<S>::identity    = XformT::make( PosT<3, S>::zero, QuatT<S>::identity, VecT<3, S>::one );

// BBoxes.
template<typename S> const AabbT<S> AabbT<S>::identity      = AabbT::make( PosT<3, S>::zero, PosT<3, S>::zero );
template<typename S> const ObbT<S> ObbT<S>::identity        = ObbT::make( MatT<4, S>::identity, VecT<3, S>::zero );

// Colors.
#define DL_MAKE_STANDARD_COLORS(TYPE,T)                                                         \
    template<typename T> const TYPE<T> TYPE<T>::black       = TYPE<T>::make( 0, 0, 0 );         \
    template<typename T> const TYPE<T> TYPE<T>::white       = TYPE<T>::make( 1, 1, 1 );         \
    template<typename T> const TYPE<T> TYPE<T>::grey        = TYPE<T>::make( 0.5, 0.5, 0.5 );   \
    template<typename T> const TYPE<T> TYPE<T>::red         = TYPE<T>::make( 1, 0, 0 );         \
    template<typename T> const TYPE<T> TYPE<T>::green       = TYPE<T>::make( 0, 1, 0 );         \
    template<typename T> const TYPE<T> TYPE<T>::blue        = TYPE<T>::make( 0, 0, 1 );         \
    template<typename T> const TYPE<T> TYPE<T>::yellow      = TYPE<T>::make( 1, 1, 0 );         \
    template<typename T> const TYPE<T> TYPE<T>::cyan        = TYPE<T>::make( 0, 1, 1 );         \
    template<typename T> const TYPE<T> TYPE<T>::magenta     = TYPE<T>::make( 1, 0, 1 );

DL_MAKE_STANDARD_COLORS( RgbT, S )
DL_MAKE_STANDARD_COLORS( RgbaT, S )

//=================================================================================================
// RgbT Impl.
//=================================================================================================

template<typename S> DL_MATH_INL RgbT<S>::operator Rgb8() const { return Rgb8::make( r, g, b ); }
template<typename S> DL_MATH_INL RgbT<S>::operator Rgba8() const { return Rgba8::make( r, g, b ); }

//=================================================================================================
// RgbaT Impl.
//=================================================================================================

template<typename S> DL_MATH_INL RgbaT<S>::operator Rgb8() const { return Rgb8::make( r, g, b ); }
template<typename S> DL_MATH_INL RgbaT<S>::operator Rgba8() const { return Rgba8::make( r, g, b, a ); }

//=================================================================================================
// Rgb8 Impl.
//=================================================================================================

DL_MATH_INL RgbT<UInt8>::operator Rgbaf() const { return Rgbaf::make( ( Float )r/255.0f, ( Float )g/255.0f, ( Float )b/255.0f ); }
DL_MATH_INL RgbT<UInt8>::operator Rgbad() const { return Rgbad::make( ( Double )r/255.0, ( Double )g/255.0, ( Double )b/255.0 ); }
DL_MATH_INL RgbT<UInt8>::operator Rgba8() const { return Rgba8::make( r, g, b ); }

DL_MATH_INL RgbT<UInt8>::operator Rgbf() const { return ( Rgbf )( Rgbaf )( *this ); }
DL_MATH_INL RgbT<UInt8>::operator Rgbd() const { return ( Rgbd )( Rgbad )( *this ); }

DL_MATH_INL RgbT<UInt8>::operator Hsvf() const { return ( Hsvf )( Rgbf )*this; }
DL_MATH_INL RgbT<UInt8>::operator Hsvd() const { return ( Hsvd )( Rgbd )*this; }

DL_MATH_INL RgbT<UInt8>::operator Hslf() const { return ( Hslf )( Rgbf )*this; }
DL_MATH_INL RgbT<UInt8>::operator Hsld() const { return ( Hsld )( Rgbd )*this; }

DL_MATH_INL RgbT<UInt8>::operator Argb8() const { return Argb8::make( *this ); }
DL_MATH_INL RgbT<UInt8>::operator ColorRef() const { return ColorRef::make( *this ); }

//=================================================================================================
// Rgba8 Impl.
//=================================================================================================

DL_MATH_INL RgbaT<UInt8>::operator Rgbaf() const { return Rgbaf::make( ( Float )r/255.0f, ( Float )g/255.0f, ( Float )b/255.0f ); }
DL_MATH_INL RgbaT<UInt8>::operator Rgbad() const { return Rgbad::make( ( Double )r/255.0, ( Double )g/255.0, ( Double )b/255.0 ); }

DL_MATH_INL RgbaT<UInt8>::operator Rgbf() const { return ( Rgbf )( Rgbaf )( *this ); }
DL_MATH_INL RgbaT<UInt8>::operator Rgbd() const { return ( Rgbd )( Rgbad )( *this ); }
DL_MATH_INL RgbaT<UInt8>::operator Rgb8() const { return Rgb8::make( r, g, b ); }

DL_MATH_INL RgbaT<UInt8>::operator Hsvf() const { return ( Hsvf )( Rgbf )*this; }
DL_MATH_INL RgbaT<UInt8>::operator Hsvd() const { return ( Hsvd )( Rgbd )*this; }

DL_MATH_INL RgbaT<UInt8>::operator Hslf() const { return ( Hslf )( Rgbf )*this; }
DL_MATH_INL RgbaT<UInt8>::operator Hsld() const { return ( Hsld )( Rgbd )*this; }

DL_MATH_INL RgbaT<UInt8>::operator Argb8() const { return Argb8::make( *this ); }
DL_MATH_INL RgbaT<UInt8>::operator ColorRef() const { return ColorRef::make( *this ); }

//=================================================================================================
// Argb8 Impl.
//=================================================================================================

DL_MATH_INL Argb8::operator ColorRef() const { return ( ColorRef )( Rgb8 )*this; }

template<typename Stream> static DL_MATH_INL Stream& operator<<( Stream& s, Argb8 c ) { return s << c.u; }
template<typename Stream> static DL_MATH_INL Stream& operator>>( Stream& s, Argb8& c ) { s >> c.u; return s; }

//=================================================================================================
// ColorRef Impl.
//=================================================================================================

DL_MATH_INL ColorRef::operator Argb8() const { return ( Argb8 )( Rgb8 )*this; }

template<typename Stream> static DL_MATH_INL Stream& operator<<( Stream& s, ColorRef c ) { return s << c.u; }
template<typename Stream> static DL_MATH_INL Stream& operator>>( Stream& s, ColorRef& c ) { s >> c.u; return s; }

//=================================================================================================
//  Pop pragmas.
//=================================================================================================

#if defined(_MSC_VER)
    #pragma warning( pop )
#elif defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

//=================================================================================================
// Ensure that Float tuple sizes are correct.
//=================================================================================================

#if !defined( DL_NO_STATIC_ASSERT )
    DL_SASSERT( sizeof( Vec2i )    ==  2 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Pos2i )    ==  2 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Vec3i )    ==  3 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Pos3i )    ==  3 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Vec4i )    ==  4 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Quati )    ==  4 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Mat3i )    ==  9 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Mat4i )    == 16 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Xformi )   == 10 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Regioni )  ==  4 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Aabbi )    ==  6 * sizeof( Int32 ) );
    DL_SASSERT( sizeof( Obbi )     == 19 * sizeof( Int32 ) );

    DL_SASSERT( sizeof( Vec2u )    ==  2 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Pos2u )    ==  2 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Vec3u )    ==  3 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Pos3u )    ==  3 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Vec4u )    ==  4 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Quatu )    ==  4 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Mat3u )    ==  9 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Mat4u )    == 16 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Xformu )   == 10 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Regionu )  ==  4 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Aabbu )    ==  6 * sizeof( UInt32 ) );
    DL_SASSERT( sizeof( Obbu )     == 19 * sizeof( UInt32 ) );

    DL_SASSERT( sizeof( Vec2f )    ==  2 * sizeof( Float ) );
    DL_SASSERT( sizeof( Pos2f )    ==  2 * sizeof( Float ) );
    DL_SASSERT( sizeof( Vec3f )    ==  3 * sizeof( Float ) );
    DL_SASSERT( sizeof( Pos3f )    ==  3 * sizeof( Float ) );
    DL_SASSERT( sizeof( Vec4f )    ==  4 * sizeof( Float ) );
    DL_SASSERT( sizeof( Quatf )    ==  4 * sizeof( Float ) );
    DL_SASSERT( sizeof( Mat3f )    ==  9 * sizeof( Float ) );
    DL_SASSERT( sizeof( Mat4f )    == 16 * sizeof( Float ) );
    DL_SASSERT( sizeof( Xformf )   == 10 * sizeof( Float ) );
    DL_SASSERT( sizeof( Regionf )  ==  4 * sizeof( Float ) );
    DL_SASSERT( sizeof( Aabbf )    ==  6 * sizeof( Float ) );
    DL_SASSERT( sizeof( Obbf )     == 19 * sizeof( Float ) );
    DL_SASSERT( sizeof( Rgbf )     ==  3 * sizeof( Float ) );
    DL_SASSERT( sizeof( Rgbaf )    ==  4 * sizeof( Float ) );
    DL_SASSERT( sizeof( Hsvf )     ==  3 * sizeof( Float ) );
    DL_SASSERT( sizeof( Hslf )     ==  3 * sizeof( Float ) );

    DL_SASSERT( sizeof( Vec2d )    ==  2 * sizeof( Double ) );
    DL_SASSERT( sizeof( Pos2d )    ==  2 * sizeof( Double ) );
    DL_SASSERT( sizeof( Vec3d )    ==  3 * sizeof( Double ) );
    DL_SASSERT( sizeof( Pos3d )    ==  3 * sizeof( Double ) );
    DL_SASSERT( sizeof( Vec4d )    ==  4 * sizeof( Double ) );
    DL_SASSERT( sizeof( Quatd )    ==  4 * sizeof( Double ) );
    DL_SASSERT( sizeof( Mat3d )    ==  9 * sizeof( Double ) );
    DL_SASSERT( sizeof( Mat4d )    == 16 * sizeof( Double ) );
    DL_SASSERT( sizeof( Xformd )   == 10 * sizeof( Double ) );
    DL_SASSERT( sizeof( Regiond )  ==  4 * sizeof( Double ) );
    DL_SASSERT( sizeof( Aabbd )    ==  6 * sizeof( Double ) );
    DL_SASSERT( sizeof( Obbd )     == 19 * sizeof( Double ) );
    DL_SASSERT( sizeof( Rgbd )     ==  3 * sizeof( Double ) );
    DL_SASSERT( sizeof( Rgbad )    ==  4 * sizeof( Double ) );
    DL_SASSERT( sizeof( Hsvd )     ==  3 * sizeof( Double ) );
    DL_SASSERT( sizeof( Hsld )     ==  3 * sizeof( Double ) );

    DL_SASSERT( sizeof( Rgb8 )     ==  3 * sizeof( UInt8 ) );
    DL_SASSERT( sizeof( Rgba8 )    ==  4 * sizeof( UInt8 ) );
    DL_SASSERT( sizeof( Argb8 )    ==  4 * sizeof( UInt8 ) );
    DL_SASSERT( sizeof( ColorRef ) ==  4 * sizeof( UInt8 ) );
#endif

//=================================================================================================
//  Ensure that types are really POD.
//=================================================================================================

#if DL_MATH_CHECK_PODNESS && !defined( DL_NO_STATIC_ASSERT )
#include <type_traits>
    DL_SASSERT( std::is_pod<Vec2d>() );
    DL_SASSERT( std::is_pod<Pos2d>() );
    DL_SASSERT( std::is_pod<Vec3d>() );
    DL_SASSERT( std::is_pod<Pos3d>() );
    DL_SASSERT( std::is_pod<Vec4d>() );
    DL_SASSERT( std::is_pod<Quatd>() );
    DL_SASSERT( std::is_pod<Mat3d>() );
    DL_SASSERT( std::is_pod<Mat4d>() );
    DL_SASSERT( std::is_pod<Xformd>() );
    DL_SASSERT( std::is_pod<Aabbd>() );
    DL_SASSERT( std::is_pod<Obbd>() );
    DL_SASSERT( std::is_pod<Rgbd>() );
    DL_SASSERT( std::is_pod<Rgb8>() );
    DL_SASSERT( std::is_pod<Rgbad>() );
    DL_SASSERT( std::is_pod<Rgba8>() );
    DL_SASSERT( std::is_pod<Hsvd>() );
    DL_SASSERT( std::is_pod<Hsld>() );
    DL_SASSERT( std::is_pod<Argb8>() );
    DL_SASSERT( std::is_pod<ColorRef>() );
#endif

} // math
} // dl

#endif // DL_CORE_MATH_H
