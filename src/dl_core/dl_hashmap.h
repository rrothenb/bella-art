// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_HASHMAP_H
#define DL_CORE_HASHMAP_H

#include "dl_defines.h"
#include "dl_vector.h"
#include <algorithm>

namespace dl {
namespace ds {

// HashMap is a typical associative container similar to std::unordered_map. References to values
// are obtained via operator[](const K& key), which will default-construct a new V at index 'key'
// if one does not currently exist. Keys must support dl::ds::hash(K key), and values must be
// default-constructible, assignable, and copyable.
//
template<typename K, typename V> class HashMap
{
public:
    HashMap()
    {
        std::fill( _slots, _slots + N, Slot() );
    }

    HashMap( const HashMap& o )
    {
        std::copy( o._slots, o._slots + N, _slots );
    }

    HashMap( HashMap&& o )
    {
        swap( o );
    }

    HashMap( std::initializer_list<std::pair<K, V>> init )
    {
        for ( const std::pair<K, V>* it = init.begin(); it != init.end(); ++it )
            set( it->first, it->second );
    }

    V& missing()
    {
        static V s_v;
        return s_v;
    }

    void swap( HashMap& o )
    {
        using std::swap;
        swap( _slots, o._slots );
    }

    HashMap& operator=( HashMap o )
    {
        swap( o );
        return *this;
    }

    bool operator==( const HashMap& o ) const
    {
        if ( this == &o )
            return true;

        return _slots == o._slots;
    }

    bool operator!=( const HashMap& o ) const
    {
        return !( *this == o );
    }

    V& operator[]( const K& key ) { return find( key ); }

    V& at( const K& key )
    {
        Node* node = findNode( key );
        if ( node )
            return node->value;

        return missing();
    }

    const V& at( const K& key ) const
    {
        return DL_DECONST( HashMap*, this )->at( key );
    }

    Size size() const
    {
        Size s = 0;
        for ( Size i = 0; i < N; ++i )
            s += _slots[i].size();

        return s;
    }

    bool empty() const { return size() == 0; }

    void clear()
    {
        for ( Size i = 0; i < N; ++i )
            _slots[i].clear();
    }

    bool erase( const K& key )
    {
        UInt64 hash = keyHash( key );
        Slot& slot = *findSlot( key );
        for ( UInt i = 0; i < slot.size(); ++i )
            if ( slot[i].hash == hash )
                return slot.erase_at( i );

        return false;
    }

    Vector<K> keys() const
    {
        Vector<K> ret;
        for ( Size i = 0; i < N; ++i )
            for ( Size j = 0; j < _slots[i].size(); ++j )
                ret.push_back( _slots[i][j].key );

        return ret;
    }

    Vector<V> values() const
    {
        Vector<V> ret;
        for ( Size i = 0; i < N; ++i )
            for ( Size j = 0; j < _slots[i].size(); ++j )
                ret.push_back( _slots[i][j].value );

        return ret;
    }

    bool contains_key( const K& key ) const
    {
        return DL_DECONST( HashMap*, this )->findNode( key ) != nullptr;
    }

    bool contains_value( const V& value ) const
    {
        for ( Size i = 0; i < N; ++i )
            for ( Size j = 0; j < _slots[i].size(); ++j )
                if ( _slots[i][j].value == value )
                    return true;

        return false;
    }

private:
    struct Node
    {
        UInt64 hash;
        K key;
        V value;
    };

    typedef Vector<Node> Slot;

    static const Size N = 256;
    Slot _slots[N];

    UInt64 keyHash( const K& key )
    {
        using ds::hash;
        return hash( key );
    }

    UInt64 slotIndex( UInt64 hash )
    {
        return hash & ( N - 1 );
    }

    Slot* findSlot( const K& key )
    {
        UInt64 hash = keyHash( key );
        UInt64 idx = slotIndex( hash );
        return &_slots[idx];
    }

    Node* findNode( const K& key )
    {
        UInt64 hash = keyHash( key );
        UInt64 idx = slotIndex( hash );
        Slot& slot = _slots[idx];
        for ( UInt i = 0; i < slot.size(); ++i )
        {
            Node& node = slot[i];
            if ( node.hash == hash )
                return &node;
        }

        return nullptr;
    }

    V& set( const K& key, const V& value )
    {
        Node* node = findNode( key );
        if ( node )
        {
            node->value = value;
            return node->value;
        }

        using ds::hash;

        Slot* slot = findSlot( key );
        slot->push_back( Node { hash( key ), key, value } );
        return slot->back().value;
    }

    V& find( const K& key )
    {
        Node* node = findNode( key );
        if ( node )
            return node->value;

        return set( key, V() );
    }
};

template<typename K, typename V> static inline void swap( HashMap<K, V>& l, HashMap<K, V>& r )
{
    l.swap( r );
}

} // ds
} // dl

#endif // DL_CORE_HASHMAP_H
