// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_REFERENCES_H
#define DL_CORE_REFERENCES_H

//
// The classes in this header are intended to provide for a standardized way of dealing with ref-
// counted, hidden implmentation classes, and the wrappers through which they're used. For example,
// consider something like the dl::bella_sdk::Node class and its implementation. We begin with the
// INode interface, which inherits the IRefCounted interface:
//
//      class INode : public IRefCounted
//      {
//      public:
//          virtual void* data() const = 0; // Just for example; INode doesn't have this.
//      protected:
//          virtual ~INode() = default;
//      };
//
// INode (like IRefCounted) is pure virtual, with an internal implementation, impl::Node:
//
//      namespace impl {
//      class Node DL_FINAL : public INode
//      {
//          DL_IMPLEMENT_IREFCOUNTED
//      public:
//          Node() : _data( nullptr ) { }
//          virtual ~Node() { delete _data; }
//          virtual void* data() const { return _data; }
//      private:
//          void* _data;
//      };
//      } // impl
//
// Through the DL_IMPLEMENT_IREFCOUNTED macro, impl::Node implements the required incRef() and
// decRef() methods from IRefCounted, along with its self-deleting behavior. If it is not required
// for your class to be pure virtual (i.e. it is okay for the std::atomic<int> _refcount to be a
// data member of your class), then you can derive not from the IRefCounted interface, but from
// the RefCounted class, which will prevent the need to use the DL_IMPLEMENT_IREFCOUNTED macro.
//
// Finally, we have the Node wrapper class:
//
//      class Node DL_FINAL : public RefersTo<INode>
//      {
//      public:
//          Node() = default;
//          Node( INode* impl ) : RefersTo( impl ) {}
//
//          void* data() const { return impl() ? impl()->data() : nullptr }
//      };
//
// The "_impl" pointer for the Node wrapper class is thus hidden, and has its reference-counting
// and self-deleting behaviors defined only once, correctly, in this file, preventing any potential
// bugs.
//
// However, in some cases, a more customized approach is desired. For example, examine the String
// class, and the way it completely hides all the machinery, but without duplicating its interface
// explicitly. It inherits from RefersTo<IRefCounted> rather than RefersTo<impl::String>, exposing
// itself as its only interface. Internally, it requires customized behavior, allocating a "null"
// instance in its default constructor, and providing its own logic for equality operators. Since
// it does not expose a separate interface, impl::String is free to inherit from RefCounted rather
// than IRefCounted.
// 
// As you can see above, this design also makes wrapper classes very nice to work with, since they
// can provide sensible default behavior for when their implementation is null. Since they keep
// the instances they reference alive, they should be passed around by value. Since wrappers can
// be created from pointers to the interfaces they wrap, this extends to passing pointers to those
// interfaces between libraries as well, with no object ownership issues arising, since the actual
// instances represented by those interfaces always delete themselves.
//

#include "dl_defines.h"
#include <atomic>

namespace dl {

// IRefCounted is the interface for reference-counted, self-deleting classes.
//
class DL_API IRefCounted
{
public:
    virtual void incRef() = 0;
    virtual void decRef() = 0;
protected:
    virtual ~IRefCounted();
};

// This macro is provided in order to allow IRefCounted subclasses to remain pure virtual. Put it
// at the top of your implementation class, and it will take care of things automatically.
//
#define DL_IMPLEMENT_IREFCOUNTED                        \
private:                                                \
    std::atomic<int> _refcount = ATOMIC_VAR_INIT( 0 );  \
public:                                                 \
    virtual void incRef()                               \
    {                                                   \
        _refcount.fetch_add( 1 );                       \
    }                                                   \
    virtual void decRef()                               \
    {                                                   \
        if ( _refcount.fetch_add( -1 ) == 1 )           \
            delete this;                                \
    }

// RefCounted is for classes which do not need to be pure virtual, and just prevents them having
// to put the DL_IMPLEMENT_IREFCOUNTED in their declaration.
//
class RefCounted : public IRefCounted
{
private:
    std::atomic<int> _refcount = ATOMIC_VAR_INIT( 0 );
public:
    DL_API virtual void incRef();
    DL_API virtual void decRef();
protected:
    DL_API virtual ~RefCounted();
};

// RefersTo is inherited by classes wrapping implementations of IRefCounted subclasses.
//
template<typename TRefCounted> class RefersTo
{
public:
    RefersTo() : _impl( nullptr ) { }

    RefersTo( TRefCounted* impl )
    {
        if ( impl )
            impl->incRef();

        _impl = impl;
    }

    RefersTo( const RefersTo& ref )
    {
        if ( ref._impl )
            ref._impl->incRef();

        _impl = ref._impl;
    }

    RefersTo& operator=( TRefCounted* impl )
    {
        if ( impl )
            impl->incRef();

        TRefCounted* prev = _impl;

        _impl = impl;

        if ( prev )
            prev->decRef();

        return *this;
    }

    RefersTo& operator=( const RefersTo& ref )
    {
        if ( ref._impl )
            ref._impl->incRef();

        TRefCounted* prev = _impl;

        _impl = ref._impl;

        if ( prev )
            prev->decRef();

        return *this;
    }

    virtual ~RefersTo()
    {
        if ( _impl )
            _impl->decRef();
    }

    bool operator==( const RefersTo& o ) const
    {
        return _impl == o._impl;
    }

    bool operator!=( const RefersTo& o ) const
    {
        return _impl != o._impl;
    }

    bool isNull() const
    {
        return _impl == nullptr;
    }

    DL_EXPLICIT operator bool() const
    {
        return _impl != nullptr;
    }

    const TRefCounted* impl() const
    {
        return _impl;
    }

    TRefCounted* impl()
    {
        return _impl;
    }

    const TRefCounted* operator->() const
    {
        return _impl;
    }

    TRefCounted* operator->()
    {
        return _impl;
    }

private:
    TRefCounted* _impl;
};

// This class is used to ensure a refcounted instance stays alive for the duration of a scope.
//
template<typename T> class RefScope
{
public:
    RefScope( T* t ) : _t( t )
    {
        if ( _t )
            _t->incRef();
    }

    ~RefScope()
    {
        if ( _t )
            _t->decRef();
    }

    bool isNull() const
    {
        return _t == nullptr;
    }

    DL_EXPLICIT operator bool() const
    {
        return _t != nullptr;
    }

    T* operator->()
    {
        return _t;
    }

private:
    T* _t;
};

// This class is used to ensure a refcounted instance is dereferenced after the close of a scope.
//
template<typename T> class DeRefScope
{
public:
    DeRefScope( T* t ) : _t( t )
    {
        if ( _t )
            _t->incRef();
    }

    ~DeRefScope()
    {
        if ( _t )
        {
            _t->decRef();
            _t->decRef();
        }
    }

    bool isNull() const
    {
        return _t == nullptr;
    }

    DL_EXPLICIT operator bool() const
    {
        return _t != nullptr;
    }

    T* operator->()
    {
        return _t;
    }

private:
    T* _t;
};

} // dl

#endif // DL_CORE_REFERENCES_H
