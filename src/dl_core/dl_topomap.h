// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_TOPOMAP_H
#define DL_CORE_TOPOMAP_H

#include "dl_types.h"
#include "dl_vector.h"

namespace dl {
namespace ds {

// TopoMap is for generating a topological sorting of a directed graph. For any type T supporting
// the equality operator you add nodes and edges, before finally calling sort, which will return
// true if the graph was able to be sorted, and false if cycles were detected.
// 
// This is used many places in Bella, for instance when retrieving a list of nodes from the scene,
// we may want them in order of dependence, and this requires traversing them, following any node
// references they may have, or nodes to which they are connected, and adding them to a TopoMap.
// 
// It is also used in parsing .bnd files, to enable inheritance of node definitions, which depends
// on being able to process the base definitions before those deriving from them, without putting
// any onerous requirements on the programmer to declare things in any particular order.
//
template<typename T> class TopoMap
{
public:
    typedef ds::Vector<T> TVector;

    class Node
    {
    public:
        enum State { Unmarked, Temporary, Permanent };

        Node( T data = T() ) : _data( data ), _dependencies(), _state( Unmarked ) { }

        T data() const
        {
            return _data;
        }

        TVector dependencies() const
        {
            return _dependencies;
        }

    private:
        friend class TopoMap;

        T       _data;
        TVector _dependencies;
        State   _state;

        bool unmarked() const
        {
            return _state == Unmarked;
        }

        bool markedTemporary() const
        {
            return _state == Temporary;
        }

        bool markedPermanent() const
        {
            return _state == Permanent;
        }

        void unmark()
        {
            _state = Unmarked;
        }

        void markTemporary()
        {
            _state = Temporary;
        }

        void markPermanent()
        {
            _state = Permanent;
        }

        bool dependsOn( Node* node )
        {
            for ( auto& data: _dependencies )
                if ( data == node->_data )
                    return true;

            return false;
        }

        bool addDependency( Node* node )
        {
            if ( _data == node->_data )
                return false;

            if ( dependsOn( node ) )
                return false;

            _dependencies.push_back( node->_data );
            return true;
        }
    };

private:
    typedef ds::Vector<Node>  NodeVector;
    typedef ds::Vector<Node*> PNodeVector;

    NodeVector _nodes;

    bool next( Node*& node )
    {
        node = nullptr;

        for ( auto& n: _nodes )
            if ( n.unmarked() )
            {
                node = &n;
                return true;
            }

        return false;
    }

    PNodeVector edgesFor( Node* node )
    {
        PNodeVector ret;

        for ( auto& n: _nodes )
            if ( node->dependsOn( &n ) )
                ret.push_back( &n );

        return ret;
    }

    void visit( Node* node, TVector& sorted, bool& stop )
    {
        if ( node->markedPermanent() )
            return;

        if ( node->markedTemporary() )
        {
            stop = true; // Cycle detected.
            return;
        }

        node->markTemporary();

        auto edges = edgesFor( node );
        for ( auto m: edges )
        {
            visit( m, sorted, stop );
            if ( stop )
                return;
        }

        node->markPermanent();
        sorted.push_back( node->data() );
    }

public:
    Size size() const
    {
        return _nodes.size();
    }

    Node* find( T data )
    {
        for ( auto& n: _nodes )
            if ( n.data() == data )
                return &n;

        return nullptr;
    }

    bool contains( T data )
    {
        return find( data ) != nullptr;;
    }

    bool addEdge( T l, T r )
    {
        if ( !contains( l ) )
            _nodes.push_back( Node( l ) );

        if ( !contains( r ) )
            _nodes.push_back( Node( r ) );

        auto a = find( l );
        auto b = find( r );

        return a->addDependency( b );
    }

    void addNode( T data )
    {
        if ( contains( data ) )
            return;

        _nodes.push_back( Node( data ) );
    }

    bool sort( TVector& sorted )
    {
        auto stop = false;

        Node* node;
        while ( next( node ) )
        {
            visit( node, sorted, stop );
            if ( stop )
                break;
        }

        for ( auto& n: _nodes )
            n.unmark();

        if ( stop )
            return false;

        DL_ASSERT( _nodes.size() == sorted.size() );
        return true;
    }
};

} // ds
} // dl

#endif // DL_CORE_TOPOMAP_H
