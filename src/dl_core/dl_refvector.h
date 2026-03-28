// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_REF_VECTOR_H
#define DL_CORE_REF_VECTOR_H

//
// These macros are for generating wrapper vectors for things like the sdk's Node and such,
// in whichever namespace you happen to be.
//
#define DL_DECL_REF_VECTOR(T)                                               \
class DL_API T##Vector : public RefersTo<RefCounted>                        \
{                                                                           \
public:                                                                     \
    T##Vector();                                                            \
    T##Vector( std::initializer_list<T> init );                             \
    T##Vector( Size size, T t = T() );                                      \
    Size                    size() const;                                   \
    Bool                    empty() const;                                  \
    Size                    capacity() const;                               \
    T                       operator[]( Size idx );                         \
    T                       front();                                        \
    T                       back();                                         \
    T*                      begin();                                        \
    T*                      end();                                          \
    T*                      data();                                         \
    const T*                cdata();                                        \
    const T                 operator[]( Size idx ) const;                   \
    const T                 front() const;                                  \
    const T                 back() const;                                   \
    const T*                begin() const;                                  \
    const T*                end() const;                                    \
    const T*                data() const;                                   \
    Int                     find( T t ) const;                              \
    Bool                    contains( T t ) const;                          \
    void                    clear();                                        \
    void                    reserve( Size newCapacity );                    \
    void                    resize( Size newSize, T t = T() );              \
    T##Vector&              operator <<( T t );                             \
    void                    push_back( T t = T() );                         \
    void                    pop_back();                                     \
    void                    insert( Size idx, T t = T() );                  \
    T                       extract( Size idx );                            \
    Bool                    erase_at( Size idx );                           \
    Size                    erase( T t );                                   \
    T##Vector               dup() const;                                    \
};

#define DL_IMPL_REF_VECTOR(T)                                                                                                                   \
namespace T##VectorImpl {                                                                                                                       \
class Vector : public ds::Vector<T>, public RefCounted                                                                                          \
{                                                                                                                                               \
public:                                                                                                                                         \
    Vector() : ds::Vector<T>() { }                                                                                                              \
    Vector( const Vector& o ) : ds::Vector<T>( o ) { }                                                                                          \
    Vector( Vector&& o ) : ds::Vector<T>( o ) { }                                                                                               \
    Vector( std::initializer_list<T> init ) : ds::Vector<T>( init ) { }                                                                         \
    Vector( Size size, const T& t = T() ) : ds::Vector<T>( size, true, t ) { }                                                                  \
};}                                                                                                                                             \
T##Vector::T##Vector() : RefersTo( new T##VectorImpl::Vector() ) { }                                                                            \
T##Vector::T##Vector( std::initializer_list<T> init ) : RefersTo( new T##VectorImpl::Vector( init ) ) { }                                       \
T##Vector::T##Vector( Size size, T t ) : RefersTo( new T##VectorImpl::Vector( size, t ) ) { }                                                   \
Size            T##Vector::size() const                     { return ((T##VectorImpl::Vector*)(this->impl()))->size(); }                        \
Bool            T##Vector::empty() const                    { return ((T##VectorImpl::Vector*)(this->impl()))->empty(); }                       \
Size            T##Vector::capacity() const                 { return ((T##VectorImpl::Vector*)(this->impl()))->capacity(); }                    \
T               T##Vector::operator[]( Size idx )           { return ((T##VectorImpl::Vector*)(this->impl()))->operator[]( idx ); }             \
T               T##Vector::front()                          { return ((T##VectorImpl::Vector*)(this->impl()))->front(); }                       \
T               T##Vector::back()                           { return ((T##VectorImpl::Vector*)(this->impl()))->back(); }                        \
T*              T##Vector::begin()                          { return ((T##VectorImpl::Vector*)(this->impl()))->begin(); }                       \
T*              T##Vector::end()                            { return ((T##VectorImpl::Vector*)(this->impl()))->end(); }                         \
T*              T##Vector::data()                           { return ((T##VectorImpl::Vector*)(this->impl()))->data(); }                        \
const T*        T##Vector::cdata()                          { return ((T##VectorImpl::Vector*)(this->impl()))->cdata(); }                       \
const T         T##Vector::operator[]( Size idx ) const     { return ((T##VectorImpl::Vector*)(this->impl()))->operator[]( idx ); }             \
const T         T##Vector::front() const                    { return ((T##VectorImpl::Vector*)(this->impl()))->front(); }                       \
const T         T##Vector::back() const                     { return ((T##VectorImpl::Vector*)(this->impl()))->back(); }                        \
const T*        T##Vector::begin() const                    { return ((T##VectorImpl::Vector*)(this->impl()))->begin(); }                       \
const T*        T##Vector::end() const                      { return ((T##VectorImpl::Vector*)(this->impl()))->end(); }                         \
const T*        T##Vector::data() const                     { return ((T##VectorImpl::Vector*)(this->impl()))->data(); }                        \
Int             T##Vector::find( const T t ) const          { return ((T##VectorImpl::Vector*)(this->impl()))->find( t ); }                     \
Bool            T##Vector::contains( T t ) const            { return ((T##VectorImpl::Vector*)(this->impl()))->contains( t ); }                 \
void            T##Vector::clear()                          { ((T##VectorImpl::Vector*)(this->impl()))->clear(); }                              \
void            T##Vector::reserve( Size newCapacity )      { return ((T##VectorImpl::Vector*)(this->impl()))->reserve( newCapacity ); }        \
void            T##Vector::resize( Size newSize, T t )      { return ((T##VectorImpl::Vector*)(this->impl()))->resize( newSize, true, t ); }    \
T##Vector&      T##Vector::operator <<( T t )               { ((T##VectorImpl::Vector*)(this->impl()))->push_back( t ); return *this; }         \
void            T##Vector::push_back( T t )                 { ((T##VectorImpl::Vector*)(this->impl()))->push_back( t ); }                       \
void            T##Vector::pop_back()                       { ((T##VectorImpl::Vector*)(this->impl()))->pop_back(); }                           \
void            T##Vector::insert( Size idx, T t )          { ((T##VectorImpl::Vector*)(this->impl()))->insert( idx, t ); }                     \
T               T##Vector::extract( Size idx )              { return ((T##VectorImpl::Vector*)(this->impl()))->extract( idx ); }                \
Bool            T##Vector::erase_at( Size idx )             { return ((T##VectorImpl::Vector*)(this->impl()))->erase_at( idx ); }               \
Size            T##Vector::erase( T t )                     { return ((T##VectorImpl::Vector*)(this->impl()))->erase( t ); }                    \
T##Vector       T##Vector::dup() const                      { T##Vector v( size() ); for ( Size i = 0; i < size(); ++i ) { v[i] = operator[](i); } return v; }

#endif // DL_CORE_REF_VECTOR_H
