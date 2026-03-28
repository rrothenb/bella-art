// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_NULLABLE_H
#define DL_CORE_NULLABLE_H

#include "dl_string.h"

namespace dl {

//
// Simple nullable type, currently used in the vulkan interface.
//
template<typename T> class Nullable DL_FINAL
{
public:
    Nullable() { }
    Nullable( const Nullable& o ) { setValue( o ); }
    Nullable( T v ) { setValue( v ); }
    ~Nullable() { delete _value; }

    Bool hasValue() const
    {
        return _value != nullptr;
    }

    T getValue() const
    {
        return _value ? *_value : T();
    }

    void setValue( const Nullable& o )
    {
        if ( !o.hasValue() )
            clear();
        else
            setValue( *o._value );
    }

    void setValue( T v )
    {
        if ( !_value )
            _value = new T;

        *_value = v;
    }

    void clear()
    {
        delete _value;
        _value = nullptr;
    }

    Bool isEqualTo( const Nullable& o ) const
    {
        return hasValue() == o.hasValue() && getValue() == o.getValue();
    }

    Bool isEqualTo( T v ) const
    {
        return hasValue() && *_value == v;
    }

    void operator=( const Nullable& o )
    {
        setValue( o );
    }

    void operator=( T v )
    {
        setValue( v );
    }

    Bool operator==( const Nullable& o ) const
    {
        return isEqualTo( o );
    }

    Bool operator==( T v ) const
    {
        return isEqualTo( v );
    }

    String toString() const
    {
        return hasValue() ? String::format( *_value ) : "null";
    }

private:
    T* _value = nullptr;
};

} // dl

#endif // DL_CORE_NULLABLE_H
