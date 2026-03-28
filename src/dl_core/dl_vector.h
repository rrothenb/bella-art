// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_VECTOR_H
#define DL_CORE_VECTOR_H

#include "dl_types.h"
#include "dl_string.h"
#include "dl_math.h"
#include "dl_references.h"
#include <algorithm>

namespace dl {
namespace ds {

// Vector manages a dynamic array of type T, where T is expected to implement standard copy ctor,
// destructor, and assignment operator. It deviates from our normal method-naming rules in order
// to be compatible with various template-based standard-library algorithms. When it runs out of
// space, its current capacity is doubled.
//
template<typename T> class Vector
{
public:
    Vector() :
        _data( nullptr ),
        _size( 0 ),
        _capacity( 0 )
    {
    }

    Vector( const Vector& o ) :
        _data( new T[o._size] ),
        _size( o._size ),
        _capacity( o._size )
    {
        std::copy( o.begin(), o.end(), _data );
    }

    Vector( Vector&& o ) :
        _data( nullptr ),
        _size( 0 ),
        _capacity( 0 )
    {
        swap( o );
    }

    Vector( std::initializer_list<T> init ) :
        _data( new T[init.size()] ),
        _size( init.size() ),
        _capacity( init.size() )
    {
        std::copy( init.begin(), init.end(), _data );
    }

    Vector( Size size, Bool fill = true, const T& t = T() ) :
        _data( new T[size] ),
        _size( size ),
        _capacity( size )
    {
        if ( fill )
            std::fill( begin(), end(), t );
    }

    ~Vector()
    {
        delete[] _data;
    }

    void swap( Vector& o )
    {
        using std::swap;
        swap( _data, o._data );
        swap( _size, o._size );
        swap( _capacity, o._capacity );
    }

    Vector& operator=( Vector o )
    {
        swap( o );
        return *this;
    }

    bool operator==( const Vector& o ) const
    {
        if ( this == &o )
            return true;

        if ( _size != o._size )
            return false;

        for ( Size i = 0; i < _size; ++i )
            if ( _data[i] != o._data[i] )
                return false;

        return true;
    }

    bool operator!=( const Vector& o ) const
    {
        return !( *this == o );
    }

    Size size() const { return _size; }
    bool empty() const { return _size == 0; }
    Size capacity() const { return _capacity; }

    const T& operator()( Size idx ) const { DL_ASSERT( idx < _size ); return _data[idx]; }

    T& operator()( Size idx ) { DL_ASSERT( idx < _size ); return _data[idx]; }
    T& operator[]( Size idx ) { DL_ASSERT( idx < _size ); return _data[idx]; }
    T& front() { DL_ASSERT( _size > 0 ); return _data[0]; }
    T& back() { DL_ASSERT( _size > 0 ); return _data[_size-1]; }
    T* begin() { return _data; }
    T* end() { return _data + _size; }
    T* data() { return _data; }
    const T* cdata() { return _data; }

    const T& operator[]( Size idx ) const { DL_ASSERT( idx < _size ); return _data[idx]; }
    const T& front() const { DL_ASSERT( _size > 0 ); return _data[0]; }
    const T& back() const { DL_ASSERT( _size > 0 ); return _data[_size-1]; }
    const T* begin() const { return _data; }
    const T* end() const { return _data + _size; }
    const T* data() const { return _data; }
    const T* cdata() const { return _data; }

    operator BufRef() const { return BufRef( _size ? _data : nullptr, _size ); }

    Int find( const T& t ) const
    {
        for ( Size i = 0; i < _size; ++i )
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
        Vector o;
        swap( o );
    }

    void assign( Size size, const T& t )
    {
        Vector o( size, true, t );
        swap( o );
    }

    void reserve( Size newCapacity )
    {
        if ( newCapacity == 0 )
        {
            clear();
            return;
        }

        if ( newCapacity == _capacity )
            return;

        bool expanding = newCapacity > _capacity;
        Size copySize = expanding ? _size : newCapacity;

        Vector o;
        o._data = new T[newCapacity];
        o._size = copySize;
        o._capacity = newCapacity;
        std::copy( _data, _data + copySize, o._data );
        swap( o );
    }

    void resize( Size newSize, Bool fill = true, const T& t = T() )
    {
        if ( newSize == _size )
            return;

        reserve( newSize );

        if ( fill && newSize > _size )
            std::fill( _data + _size, _data + newSize, t );

        _size = newSize;
    }

    void push_back( const T& t = T() )
    {
        check_capacity();
        _data[_size++] = T( t );
    }

    Vector<T>& operator <<( const T& t )
    {
        push_back( t );
        return *this;
    }

    void pop_back()
    {
        if ( _size > 0 )
            _data[--_size] = T();
    }

    void insert( Size idx, const T& t = T() )
    {
        DL_ASSERT( idx <= _size );
        if ( idx == _size )
            return push_back( t );

        check_capacity();
        for ( Size i = _size++; i > idx; --i )
            _data[i] = T( _data[i-1] );

        _data[idx] = T( t );
    }

    T extract( Size idx )
    {
        DL_ASSERT( idx < _size );
        T t = _data[idx];

        for ( Size i = idx; i < _size; ++i )
            _data[i] = ( i+1 < _size ) ? _data[i+1] : T();

        --_size;
        return t;
    }

    bool erase_at( Size idx )
    {
        if ( idx >= _size )
            return false;

        extract( idx );
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

private:
    void check_capacity()
    {
        if ( _size == _capacity )
            reserve( _capacity ? _capacity * 2 : 2 );
    }

    T*   _data;
    Size _size;
    Size _capacity;
};

template<typename T> static inline void swap( Vector<T>& l, Vector<T>& r )
{
    l.swap( r );
}

//
// The following macros declare reference-counted, hidden-implementation (they use a derivative of
// Vector<T> internally) specializations of Vector<T>, for each type found in the DL_DATATYPES group
// in dl_core/dl_types.h. Specializations are also generated for String, and the general Vec2/Vec3/etc
// typedefs in dl_core/dl_types.h. NOTE that unlike Vector<T>, operator== and != work in terms of reference
// equality.
//
// In case it is not clear, this generates StringVector, RgbVector, FloatVector, etc, etc, which have all
// the same methods as Vector<T> (with the exception of using reference equality).
//
#define DL_DS_DECL_VECTOR(NS,T)                                             \
class DL_API T##Vector : public RefersTo<RefCounted>                        \
{                                                                           \
    typedef NS::T T;                                                        \
public:                                                                     \
    T##Vector();                                                            \
    T##Vector( std::initializer_list<T> init );                             \
    T##Vector( Size size, Bool fill = true, const T& t = T() );             \
    void        swap( T##Vector& o );                                       \
    Size        size() const;                                               \
    bool        empty() const;                                              \
    Size        capacity() const;                                           \
    T&          operator[]( Size idx );                                     \
    T&          front();                                                    \
    T&          back();                                                     \
    T*          begin();                                                    \
    T*          end();                                                      \
    T*          data();                                                     \
    const T*    cdata();                                                    \
    const T&    operator[]( Size idx ) const;                               \
    const T&    front() const;                                              \
    const T&    back() const;                                               \
    const T*    begin() const;                                              \
    const T*    end() const;                                                \
    const T*    data() const;                                               \
    const T*    cdata() const;                                              \
    operator    BufRef() const;                                             \
    Int         find( const T& t ) const;                                   \
    bool        contains( const T& t ) const;                               \
    void        clear();                                                    \
    void        assign( Size size, const T& t );                            \
    void        reserve( Size newCapacity );                                \
    void        resize( Size newSize, Bool fill = true, const T& t = T() ); \
    void        push_back( const T& t = T() );                              \
    T##Vector&  operator <<( const T& t );                                  \
    void        pop_back();                                                 \
    void        insert( Size idx, const T& t = T() );                       \
    T           extract( Size idx );                                        \
    Bool        erase_at( Size idx );                                       \
    Size        erase( const T& t );                                        \
};

#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) DL_DS_DECL_VECTOR(dl, TYPE)
DL_NATIVE_DATATYPES( DL_ENUMERATE_TYPES )
DL_DATATYPE_ALIASES( DL_ENUMERATE_TYPES )

#undef  DL_ENUMERATE_TYPES
#define DL_ENUMERATE_TYPES(TYPE) DL_DS_DECL_VECTOR(dl::math, TYPE)
DL_MATH_DATATYPES( DL_ENUMERATE_TYPES )

//
// Various helper typedefs.
//
typedef ds::Vector<Byte> Buffer;

} // ds
} // dl

//
// This is moved out to its own file in order to be able to include it separately for swig, as
// we don't want to wrap all the dozens of vectors created above for the bella types.
//
#include "dl_refvector.h"

#endif // DL_CORE_VECTOR_H
