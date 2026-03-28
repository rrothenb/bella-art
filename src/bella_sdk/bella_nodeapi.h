// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef BELLA_SDK_NODEAPI_H
#define BELLA_SDK_NODEAPI_H

#include "bella_types.h"
#include "bella_sceneapi.h"
#include "../dl_core/dl_logging.h"

namespace dl {
namespace bella_sdk {

//=================================================================================================
// Node API.
//=================================================================================================
// A node library consists at minimum, of a .bnd file defining a set of nodes. Optionally, it may
// also include a shared library, to implement code for any defined node Outputs. Such a library,
// named the same as its accompanying .bnd file,may define functions to be called once at library
// load, and once at library unload, in order to manage any global state:
//
//      DL_C_EXPORT bool libInit();
//      DL_C_EXPORT bool libFree();
//
// Additionally, each node may define five functions. The first four are called when an instance
// of the node is first created, just before rendering begins, during rendering, and when the node
// instance is later deleted. The fifth is called whenever the value of one of the node's inputs
// is changed.
//
// So, for instance, if you define a "superChecker" node, you could implement library functions
// for it like so:
//
//      DL_C_EXPORT bool superCheckerInit( const INode* inode, void** data );
//      DL_C_EXPORT bool superCheckerPrep( const INode* inode, void* data );
//      DL_C_EXPORT bool superCheckerEval( EvalCtx* ctx, void* output );
//      DL_C_EXPORT bool superCheckerFree( const INode* inode, void** data );
//      DL_C_EXPORT void superCheckerChanged( const IInput* input )
//
// As such, it is usually going to be best to prefix your nodes with a unique company abbreviation,
// as they will not be loaded if their names clash with those of built-in Bella nodes, or with any
// other same-named nodes that happen to be loaded first.
//

//=================================================================================================
// libInit & libFree.
//=================================================================================================
// If found, these procedures will be called once on library load, and once on library unload. Node
// libraries may use them to allocate and deallocate global state:
//
//     static int s_state = 0;
//
//     DL_C_EXPORT bool libInit()
//     {
//         DL_ASSERT( ++s_state == 0 );
//         return true;
//     }
//
//     DL_C_EXPORT bool libFree()
//     {
//         DL_ASSERT( --s_state == 0 );
//         return true;
//     }
//
typedef bool( *pFnLibInit )();
typedef bool( *pFnLibFree )();

//=================================================================================================
// nodeInit.
//=================================================================================================
// Arguments passed to nodeInit are the node instance, and a pointer to its user data, which you
// may fill with data. Generally, you will want to make a struct for the data, using the InputInfo
// and OutputInfo helpers below.
//
// In nodeInit, attributes are not necessarily yet connected, so at this time, all you should do is
// grab pointers to your Input and Output attributes. For example, here is the implementation of a
// simple checker node's data struct and init:
//
//     struct Checker
//     {
//         // The DL_MAKE_IINFO/OINFO macros define named InputInfo & OutputInfo helpers of the
//         // specified types, which must match those declared in the .bnd file.
//         //
//         DL_MAKE_IINFO( Rgba, color1 );
//         DL_MAKE_IINFO( Rgba, color2 );
//         DL_MAKE_IINFO( Vec2, uvCoord );
//         DL_MAKE_OINFO( Vec2, outColor );
//     };
//
//     DL_C_EXPORT bool checkerInit( const INode* inode, void** data )
//     {
//         // Use getNodeInfo<Checker>() to either create a new Checker & assign it to the storage
//         // pointed to by data, or retrieve an instance already assigned by a node that inherits
//         // this node. It is named "info" for the sake of the macros that follow.
//         //
//         auto info = getNodeInfo<Checker>( inode, data );
//         if ( !info )
//             return false;
//
//         DL_INIT_IINFO( color1 );
//         DL_INIT_IINFO( color2 );
//         DL_INIT_IINFO( uvCoord );
//         DL_INIT_OINFO( outColor );
//
//         return true;
//     }
//
typedef bool( *pFnNodeInit )( const INode* node, void** data );

//=================================================================================================
// nodePrep.
//=================================================================================================
// nodePrep is called just prior to rendering, so it is here that you want to check whether your
// inputs are connected (since if they are not, you can cache their immediate values and avoid
// evaluations during nodeEval). This is done for you if you use the Input/OutputInfo helpers. This
// is important, because it is not allowed to write *any* data from within nodeEval, since it is
// called in a multithreaded context.
//
//     DL_C_EXPORT bool checkerPrep( const INode* inode, void* data )
//     {
//         // Retrive the Checker instance held in the data parameter, and assign it to an "info"
//         // variable that will be used by the following macros.
//         //
//         auto info = getNodeInfo<Checker>( inode, data );
//         if ( !info )
//             return false;
//
//         // The PREP macro caches values for unconnected inputs, so that you do not have to worry
//         // about trying to manage caches yourself (though of course you may still do so).
//         //
//         DL_PREP_IINFO( color1 );
//         DL_PREP_IINFO( color2 );
//         DL_PREP_IINFO( uvCoord );
//         DL_PREP_OINFO( outColor );
//
//         return true;
//     }
//
typedef bool( *pFnNodePrep )( const INode* node, void* data );

//=================================================================================================
// nodeEval.
//=================================================================================================
// nodeEval is called during rendering, and you should take care to optimize performance here as
// much as possible. It is also called in a multithreaded context, so it is not allowed to do *any*
// writing of data from within the function; all such must be handled beforehand in nodePrep.
//
//     DL_C_EXPORT bool checkerEval( EvalCtx* ctx, void* output )
//     {
//         // Retrive the Checker instance held in the EvalCtx, and assign it to an "info" variable
//         // that will be used by the following macros.
//         //
//         auto info = getNodeInfo<Checker>( ctx );
//         if ( !info )
//             return false;
//
//         // The EVAL macro either evaluates its input or ouput, or returns data cached earlier,
//         // by the PREP macro.
//         //
//         DL_EVAL_IINFO( color1 );
//         DL_EVAL_IINFO( color2 );
//
//         // Note that "uvCoord" is a "magic" or "context" input attribute. If it is unconnected
//         // at the start of rendering, the engine will connect it to an internal node, which will
//         // return the UVs for this call to eval (they are ultimately sent by the engine, through
//         // the EvalCtx). There are various other such context attributes, to be documented at a
//         // later date.
//         //
//         DL_EVAL_IINFO( uvCoord );
//
//         auto choice = ((uvCoord.y - floor( uvCoord.y )) < 0.5) ^
//                       ((uvCoord.x - floor( uvCoord.x )) < 0.5);
//
//         auto rgba = (choice & 1) ? color2 : color1;
//
//         // Since we evaluate on a per-output basis, we use the OutputInfo::matches(EvalCtx*)
//         // method to determine what to write into the passed void* output.
//         //
//         if ( info->outColor.matches( ctx ) )
//             return writeOutput( rgba, output );
//
//         return false;
//     }
//
typedef bool( *pFnNodeEval )( EvalCtx* ctx, void* output );

//=================================================================================================
// nodeModify.
//=================================================================================================
// nodeModify is called just prior to rendering, and allows making modifications to a node, or to
// the scene. If target is null, the modifier is not attached to any node.
//
//     DL_NODE_EXPORT bool exampleNodeModify( INode* modifier, INode* target /*= nullptr*/ )
//     {
//         if ( !modifier || !target || !target->isTypeOf( "mesh" ) )
//             return false;
//
//         auto mesh = Node( target );
//         auto mod = Node( modifier );
//
//         for ( UInt i = 0; i < mesh["steps"].inputCount(); ++i )
//         {
//             Vec3fBufRef points = mesh["steps"][i]["points"].asBuffer();
//
//             for ( UInt j = 0; j < points.count; ++j )
//                 std::swap( points.ptr[j].x, points.ptr[j].y );
//         }
//
//         return true; // Return true if node was modified.
//     }
//
typedef bool( *pFnNodeModify )( INode* modifier, INode* target /*= nullptr*/ );

//=================================================================================================
// nodeFree.
//=================================================================================================
// nodeFree is called from the node's destructor. Because of this, you must make sure never to hold
// bella_sdk::Node wrappers over the INode* instances passed to these functions, as this will cause
// the node to live forever.
//
// It is always correct to call freeNodeInfo<T> in this function, since the storage pointed to by
// the data parameter will either be null, or will have been allocated in your Init function.
//
//     DL_C_EXPORT bool checkerFree( const INode* inode, void** data )
//     {
//         return freeNodeInfo<Checker>( inode, data );
//     }
//
typedef bool( *pFnNodeFree )( const INode* node, void** data );

//=================================================================================================
// nodeChanged.
//=================================================================================================
// nodeChanged allows a node to update its state whenever one of its inputs is changed. This can be
// used to implement enum string inputs that should change the values of other inputs when they are
// changed, to update the input's enabled state, and so forth.
//
//     DL_C_EXPORT void checkerChanged( const IInput* input )
//     {
//         if ( !input || !input->node() )
//             return;
//
//         if ( input->name() == "color1" )
//         {
//             Rgba rgba;
//             if ( !input->get( rgba, false ) ) // NOTE: pay attention to connection & eval.
//                 return;
//
//             IInput* color2 = input->node()->input( "color2" );
//             if ( !color2 )
//                 return;
//
//             color2->set( rgba ); // This is a devious checker node.
//         }
//     }
//
typedef bool( *pFnNodeChanged )( const IInput* input );

//=================================================================================================
// Helper for assigning to void* output in node Eval implementations.
//=================================================================================================

template<typename T> static DL_FI bool writeOutput( T src, void* dst )
{
    if ( !dst )
        return false;

    memcpy( dst, &src, sizeof( T ) );
    return true;
}

template<> DL_FI bool writeOutput( String src, void* dst )
{
    if ( !dst )
        return false;

    *( ( String* )dst ) = src;
    return true;
}

//=================================================================================================
// EvalCtx.
//=================================================================================================
// You can do basically nothing with this; it is created in the engine and passed through the chain
// of evaluation -- internal subclasses hold more data than shown here, which is used to facilitate
// "magic" attributes like uvCoord.
//
class DL_API EvalCtx
{
public:
    EvalCtx() : _output( nullptr ), _data( nullptr )
    {
    }

    EvalCtx( const IOutput* output ) : _output( nullptr ), _data( nullptr )
    {
        setOutput( output );
        setData( output && output->node() ? output->node()->data() : nullptr );
    }

    // This is too expensive, so make sure to keep a reference to the parent node.
    //
    //virtual ~EvalCtx()
    //{
    //    if ( _output )
    //        ( ( IOutput* )_output )->decRef();
    //}

    virtual Bool                    hasXform() const                    { return false; }
    virtual const INode*            xform() const                       { return nullptr; }
    virtual Mat4                    pathTransform() const               { return Mat4::identity; }

    virtual Bool                    hasOutput() const                   { return _output != nullptr; }
    virtual const IOutput*          output() const                      { return _output; }
    virtual UInt64                  hash() const                        { return 0; }

    virtual Bool                    hasData() const                     { return _data; }
    virtual const void*             data() const                        { return hasData() ? _data : nullptr; }
    template<typename T> const T*   dataAs() const                      { return hasData() ? ( T* )_data : nullptr; }

    virtual Bool                    isForScalar() const                 { return false; }
    virtual Bool                    isForNormal() const                 { return false; }

    virtual UInt                    numUVs() const                      { return 0; }
    virtual const Vec2f*            uvs() const                         { return nullptr; }

    // This is for use when a node must mutate UVs, which are then evaluated by further nodes. You
    // should rarely need to do this, you should be very careful if you do, and you must make sure
    // to restore the previous UVs before returning from the function in which you changed them.
    //
    virtual void                    setUVs( const Vec2f* uvs )          { DL_UNUSED( uvs ); }

    // This is used by node implementations to point the context at a sub-object in their own node
    // data, for calling base node implementations they inherit.
    //
    void                            setData( const void* data )         { _data = data; }
    
    virtual Aabb                    bbox() const                        { return Aabb::identity; }
    virtual Mat3                    tangentFrame() const                { return Mat3::identity; }
    virtual Pos3                    localPosition() const               { return Pos3::zero; }

    // These are for cases where a caller must set a custom tangent frame, local position, etc.
    //
    virtual void                    setTangentFrame( Mat3 frame )       { DL_UNUSED( frame ); }
    virtual void                    setLocalPosition( Pos3 position )   { DL_UNUSED( position ); }

protected:
    void setOutput( const IOutput* output )
    {
        // This is too expensive, so make sure to keep a reference to the parent node.
        //
        //if ( output )
        //    ( ( IOutput* )output )->incRef();
        //
        //if ( _output )
        //    ( ( IOutput* )_output )->decRef();

        _output = output;
    }

private:
    const IOutput*                  _output = nullptr;
    const void*                     _data = nullptr;

public:

    // Since evaluations are performed on a per-output basis, we must set the output to be eval'd
    // in the EvalCtx; when one output ends up evaluating another, we must remember the previous
    // one and restore it after evaluation.
    //
    // The system takes care of this, so there should be no reason for API clients to use this.
    //
    class DL_API EvalScope
    {
    public:
        EvalScope( EvalCtx* ctx, const IOutput* next ) : _ctx( ctx ), _output( nullptr ), _data( nullptr )
        {
            if ( _ctx )
            {
                _output = _ctx->output();
                _ctx->setOutput( next );

                _data = _ctx->data();
                _ctx->setData( next && next->node() ? next->node()->data() : nullptr );
            }
        }
        ~EvalScope()
        {
            if ( _ctx )
            {
                _ctx->setOutput( _output );
                _ctx->setData( _data );
            }
        }
    private:
        EvalCtx* _ctx = nullptr;
        const IOutput* _output = nullptr;
        const void* _data = nullptr;
    };

    // This is used by node implementations to evaluate using implementations of their base nodes.
    // If a node wishes to use the implementations of its base nodes, it will include a copy of the
    // structures they use, in its own node data, and use this object to temporarily point an eval
    // context to those structures, before evaluating the base node outputs.
    //
    class DL_API DataScope
    {
    public:
        DataScope( EvalCtx* ctx, const void* data ) : _ctx( ctx ), _data( nullptr )
        {
            if ( ctx )
            {
                _data = _ctx->data();
                _ctx->setData( data );
            }
        }
        ~DataScope()
        {
            if ( _ctx )
                _ctx->setData( _data );
        }
    private:
        EvalCtx* _ctx = nullptr;
        const void* _data = nullptr;
    };
};

//=================================================================================================
// Helpers used internally for implementing evaluation of outputs.
//=================================================================================================

template<typename T> static DL_FI bool evalOutput( EvalCtx* ctx, const IOutput* output, T& value )
{
    //RefScope ors( ( IOutput* )output );

    if ( !output )
        return false;

    INode* node = output->node();
    if ( !node )
        return false;

    //RefScope nrs( node );

    EvalCtx::EvalScope es( ctx, output );
    return node->nodeEval( ctx, &value );
}

template<> DL_FI bool evalOutput( EvalCtx* ctx, const IOutput* output, BufRef& value )
{
    //RefScope ors( ( IOutput* )output );

    if ( !output )
        return false;

    INode* node = output->node();
    if ( !node )
        return false;

    //RefScope nrs( node );

    auto type = output->bufferType();
    if ( type == DataType_Null )
        return false;

    EvalCtx::EvalScope es( ctx, output );
    BufRef bufRef;
    if ( !node->nodeEval( ctx, &bufRef ) )
        return false;

    value = bufRef;
    return true;
}

//=================================================================================================
// A helper used internally for evaluation of inputs.
//=================================================================================================

template<typename T> static DL_FI T evalInput( EvalCtx* ctx, const IInput* input, T def = T() )
{
    //RefScope irs( ( IInput* )input );

    T t;
    if ( !input->connected() )
        return input->get( t, false ) ? t : def; // NOTE: no eval (and it's disconnected anyway).

    return evalOutput( ctx, input->output(), t ) ? t : def;
}

//=================================================================================================
// Input helper for structs used in node eval.
//=================================================================================================

#define DL_MAKE_IINFO(T, NAME) InputInfo<T> NAME = { #NAME, nullptr, T(), true }
#define DL_INIT_IINFO(NAME) if ( !info->NAME.init(inode) ) { logInfo("nodeInit() failed for "#NAME); return false; }
#define DL_PREP_IINFO(NAME) if ( !info->NAME.prep() ) { logInfo("nodePrep() failed for "#NAME); return false; }
#define DL_EVAL_IINFO(NAME) auto NAME = info->NAME.eval( ctx )

template<typename T> struct InputInfo
{
    String          path;
    const IInput*   input;
    T               cache;
    Bool            useCache;
    Bool            isUvCoord;

    DL_FI bool init( const INode* node )
    {
        input       = nullptr;
        cache       = T();
        useCache    = false;
        isUvCoord   = false;

        if ( !node )
        {
            logError( "Cannot init '%s' due to null input.", path.buf() );
            return false;
        }

        IInput* tmp = node->findInput( path );
        if ( !tmp )
        {
            logError( "Input '%s' not found on node '%s'.", path.buf(), node->name().buf() );
            return false;
        }

        if ( tmp->type() != dl::bella_sdk::attrType<T>() )
        {
            logError( "Wrong type for '%s': .", tmp->path().buf(), tmp->typeString().buf() );
            return false;
        }

        if ( tmp->name() == "uvCoord" )
        {
            isUvCoord = true;
        }

        input = tmp;
        return true;
    }

    DL_FI bool prep()
    {
        //RefScope irs( ( IInput* )input );

        if ( !input )
        {
            logError( "Cannot prep '%s' due to null input.", path.buf() );
            return false;
        }

        useCache = !isUvCoord && !input->connected();
        return input->get( cache, false ); // NOTE: no eval, we're caching immediate value.
    }

    DL_FI T eval( EvalCtx* ctx = nullptr ) const
    {
        //RefScope irs( ( IInput* )input );

        if ( useCache || !input )
            return cache;

        T t;
        return input->get( t, true, ctx ) ? t : cache;
    }
};

template<> inline dl::Rgba InputInfo<dl::Rgba>::eval( EvalCtx* ctx ) const
{
    //RefScope irs( ( IInput* )input );

    if ( useCache || !input )
        return cache;

    dl::Rgba t;
    return input->get( t, true, ctx ) ? t :cache;
}

//=================================================================================================
// Output helper for structs used in node eval.
//=================================================================================================

#define DL_MAKE_OINFO(T, NAME) OutputInfo<T> NAME = { #NAME, nullptr }
#define DL_INIT_OINFO(NAME) if ( !info->NAME.init( inode ) ) { logInfo("nodeInit() failed for "#NAME); return false; }
#define DL_PREP_OINFO(NAME) if ( !info->NAME.prep() ) { logInfo("nodePrep() failed for "#NAME); return false; }

template<typename T> struct OutputInfo
{
    const String    name;
    const IOutput*  output;

    DL_FI bool init( const INode* node )
    {
        output = nullptr;

        const IOutput* tmp = node ? node->output( name ) : nullptr;
        if ( !tmp )
        {
            logError( "Output '%s' not found on node '%s'.", name.buf(), node->name().buf() );
            return false;
        }

        output = tmp;
        return true;
    }

    DL_FI bool prep()
    {
        return true; // Nothing to do.
    }

    DL_FI bool matches( EvalCtx* o ) const
    {
        //RefScope ors( ( IOutput* )output );
        return o && o->output() == output;
    }

    DL_FI bool write( T v, void* dst )
    {
        return writeOutput( v, dst );
    }
};

//=================================================================================================
// Helpers for use in node implementations.
//=================================================================================================

// This function is intended to be called at the beginning of a node Init function, like this:
//
//      auto info = initNodeInfo<NodeData>( inode, data );
//      if ( !info )
//          return false;
//
// If data points to non-null data, it assumed to be pointing to a sub-object in the node data of a
// subclass of the node type, in which case that data is returned.
//
// Otherwise, an instance of NodeData is allocated, assigned to the storage pointed to by the data
// data parameter, and returned.
//
// In either case it is fine to use the freeNodeInfo<T>( void** ) function in your nodeFree function,
// since it will only be called in the latter case.
//
template<typename T>
static DL_FI T* initNodeInfo( const INode* node, void** data )
{
    //RefScope nrs( ( INode* )node );

    if ( !node )
    {
        logError( "Null node passed to initNodeInfo." );
        return nullptr;
    }

    if ( data && *data )
        return ( T* )*data;

    *data = new T();
    return ( T* )*data;
}

// This function is intended to be called at the beginning of a node Prep function, like this:
//
//      auto info = getNodeInfo<NodeData>( inode, data );
//      if ( !info )
//          return false;
//
// If simply interprets the given data as NodeData.
//
template<typename T>
static DL_FI T* getNodeInfo( const INode* node, void* data )
{
    //RefScope nrs( ( INode* )node );

    if ( !node )
    {
        logError( "Null node passed to getNodeInfo." );
        return nullptr;
    }

    if ( !data )
    {
        logError( "Null data passed to getNodeInfo." );
        return nullptr;
    }

    return ( T* )data;
}

// This function is intended to be called at the beginning of a node Eval function, like this:
//
//      auto info = getNodeInfo<NodeData>( ctx );
//      if ( !info )
//          return false;
//
// If simply extracts and interprets the EvalCtx data as NodeData.
//
template<typename T>
static DL_FI T* getNodeInfo( EvalCtx* ctx )
{
    if ( !ctx || !ctx->hasData() )
        return nullptr;

    return ( T* )ctx->data();
}

// This function is intended to be called in a node Free function, like this:
//
//      return freeNodeInfo<NodeData>( inode, data );
//
// It is always safe to call this, and do nothing else, since the storage pointed to by the data
// parameter was either allocated by you, or is null.
//
template<typename T>
static DL_FI bool freeNodeInfo( const INode* node, void** data )
{
    DL_UNUSED( node );
    DL_ASSERT( node->data() == *data );

    delete ( T* )*data;
    *data = nullptr;
    return true;
}

// This function is intended to be called in a node Init function, like this:
//
//      if ( !initBase( fileTextureInit, &info->fileTexture, node ) )
//          return false;
//
// Where fileTexture is a base of this node type, and info->fileTexture is a FileTexture struct
// embedded in the current node's data.
//
template<typename Func>
static DL_FI bool initBase( Func init, void* data, const INode* node )
{
    //RefScope nrs( ( INode* )node );
    return init( node, &data );
}

// This function is intended to be called in a node Prep function, like this:
//
//      if ( !prepBase( fileTexturePrep, &info->fileTexture, node ) )
//          return false;
//
// Where fileTexture is a base of this node type, and info->fileTexture is a FileTexture struct
// embedded in the current node's data.
//
template<typename Func>
static DL_FI bool prepBase( Func prep, void* data, const INode* node )
{
    //RefScope nrs( ( INode* )node );
    return prep( node, data );
}

// This function is intended to be called in a node Eval function, like this:
//
//      if ( !info->outNormal.matches( ctx ) )
//          return evalBase( fileTextureEval, &info->fileTexture, ctx, output );
//
// This you can see, it is used to delegate evaluation to the base node's implementation, when the
// current node does not want to either ignore or override the base node behavior.
//
// It takes care of calling EvalCtx::setData with the base node data, and then restore it after
// the call.
//
template<typename Func>
static DL_FI bool evalBase( Func eval, void* data, EvalCtx* ctx, void* output )
{
    EvalCtx::DataScope ds( ctx, data );
    return eval( ctx, output );
}

// This function is intended to be called in a node Eval function, like this:
//
//      auto map = evalBase( info->fileTexture.outVector, &info->fileTexture, ctx );
//
// This simplifies directly evaluating the output of a base node implementation, given data stored
// in the current node's data.
//
// It takes care of calling EvalCtx::setData with the base node data, and then restore it after
// the call.
//
template<typename T>
static DL_FI T evalBase( const OutputInfo<T>& info, void* data, EvalCtx* ctx )
{
    //RefScope ors( ( IOutput* )info.output );

    T t;
    EvalCtx::DataScope ds( ctx, data );
    return info.output->eval( t, ctx ) ? t : T();
}

} // bella_sdk
} // dl

#endif // BELLA_SDK_NODEAPI_H
