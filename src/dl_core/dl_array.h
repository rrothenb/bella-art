// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_ARRAY_H
#define DL_CORE_ARRAY_H

#include "dl_types.h"
#include <algorithm>

namespace dl {
namespace ds {

// Array wraps a static array of type T, where T is expected to implement standard copy ctor,
// destructor, and assignment operator. It deviates from our normal method-naming rules in order
// to be compatible with various template-based standard-library algorithms.
//
template<typename T, Size N> class Array
{
private:
    T _data[N];

public:
    Array( bool init = true )
    {
        if ( init ) { fill( T() ); }
    }

    Array( const Array& o )
    {
        std::copy( o.begin(), o.end(), _data );
    }

    Array( Array&& o )
    {
        std::move( o.begin(), o.end(), _data );
    }

    Array( std::initializer_list<T> init )
    {
        std::copy( init.begin(), init.end(), _data );
        if ( size() > init.size() )
            std::fill( begin() + init.size(), end(), T() );
    }

    void swap( Array& o )
    {
        using std::swap;
        swap( _data, o._data );
    }

    Array& operator=( Array o )
    {
        swap( o );
        return *this;
    }

    bool operator==( const Array& o ) const
    {
        if ( this == &o )
            return true;

        for ( Size i = 0; i < N; ++i )
            if ( _data[i] != o._data[i] )
                return false;

        return true;
    }

    bool operator!=( const Array& o ) const
    {
        return !( *this == o );
    }

    Size size() const { return N; }
    bool empty() const { return N==0; }
    Size capacity() const { return N; }

    T& operator[]( Size idx ) { DL_ASSERT( idx < N ); return _data[idx]; }
    T& front() { return _data[0]; }
    T& back() { return _data[N-1]; }
    T* begin() { return &_data[0]; }
    T* end() { return &_data[N]; }
    T* data() { return &_data[0]; }

    const T& operator[]( Size idx ) const { DL_ASSERT( idx < N ); return _data[idx]; }
    const T& front() const { return _data[0]; }
    const T& back() const { return _data[N-1]; }
    const T* begin() const { return &_data[0]; }
    const T* end() const { return &_data[N]; }
    const T* data() const { return &_data[0]; }

    Int find( const T& t ) const
    {
        for ( Size i = 0; i < N; ++i )
            if ( _data[i] == t )
                return ( Int )i;

        return -1;
    }

    bool contains( const T& t ) const
    {
        return find( t ) >= 0;
    }

    void clear()
    {
        std::fill( begin(), end(), T() );
    }

    void fill( const T& t )
    {
        std::fill( begin(), end(), t );
    }

    bool erase_at( Size idx )
    {
        if ( idx >= N )
            return false;

        _data[idx] = T();
        return true;
    }

    Size erase( const T& t )
    {
        Size erased = 0;
        Int idx = find( t );
        while ( idx >= 0 )
        {
            erase_at( idx );
            ++erased;
            idx = find( t );
        }
        return erased;
    }
};

template<typename T, Size N> static inline void swap( Array<T, N>& l, Array<T, N>& r )
{
    l.swap( r );
}

//
// The macros below specialize Array<T,N> for the various DL types for convenience.
//
#define DL_SPECIALIZE_ARRAY(NS,T)                                                       \
    template<Size N> class T##Array : public Array<NS::T, N>                            \
    {                                                                                   \
    public:                                                                             \
        T##Array( bool init = true ) : ds::Array<NS::T, N>( init ) { }                  \
        T##Array( const T##Array<N>& o ) : ds::Array<NS::T, N>( o ) { }                 \
        T##Array( T##Array<N>&& o ) : ds::Array<NS::T, N>( o ) { }                      \
        T##Array( std::initializer_list<NS::T> init ) : ds::Array<NS::T, N>( init ) { } \
        T##Array& operator=( T##Array o ) { swap( o ); return *this; }                  \
    };

#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) DL_SPECIALIZE_ARRAY(dl, TYPE)
DL_NATIVE_DATATYPES( DL_ENUMERATE_TYPES )
DL_DATATYPE_ALIASES( DL_ENUMERATE_TYPES )

#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) DL_SPECIALIZE_ARRAY(dl::math, TYPE)
DL_MATH_DATATYPES( DL_ENUMERATE_TYPES )

} // ds
} // dl

#endif // DL_CORE_ARRAY_H
