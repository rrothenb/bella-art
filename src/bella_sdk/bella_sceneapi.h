// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef BELLA_SDK_SCENEAPI_H
#define BELLA_SDK_SCENEAPI_H

#include "bella_types.h"
#include "../dl_core/dl_string.h"

namespace dl {
namespace bella_sdk {

//=================================================================================================
// Forwards.
//=================================================================================================

class EvalCtx;
struct NodeObserver;
struct SceneObserver;
struct SceneProgress;

namespace interfaces {

class INode;
class IPath;
class IScene;

//=================================================================================================
// IEnumItem.
//=================================================================================================

class IEnumItem : public IRefCounted
{
public:
    virtual Int                 index() const = 0;
    virtual Bool                isValid() const = 0;

    virtual Int                 intValue() const = 0;
    virtual Real                realValue() const = 0;
    virtual String              strValue() const = 0;
    virtual String              label( String lang = "auto" ) const = 0;
    virtual String              help( String lang = "auto" ) const = 0;

protected:
    virtual ~IEnumItem() = default;
};

//=================================================================================================
// IAttrInfo. Used by both IInput and IOutput.
//=================================================================================================

class IAttrInfo : public IRefCounted
{
public:

    // Inputs & Outputs.

    virtual Bool                accepts( String type ) const = 0;
    virtual Bool                accepts( INode* node ) const = 0;

    virtual StringVector        accepted() const = 0;
    virtual Bool                setAccepted( StringVector accepted ) = 0;

    virtual String              label( String lang = "auto" ) const = 0;
    virtual Bool                setLabel( String label, String lang = "auto" ) = 0;

    virtual String              help( String lang = "auto" ) const = 0;
    virtual Bool                setHelp( String help, String lang = "auto" ) = 0;

    // Inputs only.

    virtual UInt                precision() const = 0;
    virtual Bool                setPrecision( UInt precision ) = 0;

    virtual Real                step() const = 0;
    virtual Bool                setStep( Real step ) = 0;

    virtual Bool                realtime() const = 0;
    virtual Bool                setRealtime( Bool realtime ) = 0;

    virtual String              units() const = 0;
    virtual Bool                setUnits( String units ) = 0;

    virtual Bool                isEnum() const = 0;
    virtual UInt                enumItemCount() const = 0;
    virtual IEnumItem*          enumItem( UInt index ) const = 0;

    virtual IEnumItem*          findEnumItem( Int intValue ) const = 0;
    virtual IEnumItem*          findEnumItem( Real realValue ) const = 0;
    virtual IEnumItem*          findEnumItem( String strValue ) const = 0;

    virtual IEnumItem*          addEnumItem( Int value, String label = "", String help = "", String lang = "auto" ) = 0;
    virtual IEnumItem*          addEnumItem( Real value, String label = "", String help = "", String lang = "auto" ) = 0;
    virtual IEnumItem*          addEnumItem( String value, String label = "", String help = "", String lang = "auto" ) = 0;

#define INPUT_DECL_ATTRINFO_GET(GET) \
    virtual Bool GET( Bool& out ) const = 0; \
    virtual Bool GET( Int& out ) const = 0; \
    virtual Bool GET( UInt& out ) const = 0; \
    virtual Bool GET( Real& out ) const = 0; \
    virtual Bool GET( Vec2& out ) const = 0; \
    virtual Bool GET( Vec3& out ) const = 0; \
    virtual Bool GET( Vec4& out ) const = 0; \
    virtual Bool GET( Pos2& out ) const = 0; \
    virtual Bool GET( Pos3& out ) const = 0; \
    virtual Bool GET( Quat& out ) const = 0; \
    virtual Bool GET( Rgba& out ) const = 0; \
    virtual Bool GET( Mat3& out ) const = 0; \
    virtual Bool GET( Mat4& out ) const = 0; \
    virtual Bool GET( String& out ) const = 0; \
    virtual Bool GET( INode*& out ) const = 0; \
    virtual Bool GET( BufRef& out ) const = 0;

    INPUT_DECL_ATTRINFO_GET( value )
    INPUT_DECL_ATTRINFO_GET( hardMin )
    INPUT_DECL_ATTRINFO_GET( hardMax )
    INPUT_DECL_ATTRINFO_GET( softMin )
    INPUT_DECL_ATTRINFO_GET( softMax )

#undef INPUT_DECL_ATTRINFO_GET

#define INPUT_DECL_ATTRINFO_SET(SET) \
    virtual Bool set##SET( Bool val ) = 0; \
    virtual Bool set##SET( Int val ) = 0; \
    virtual Bool set##SET( UInt val ) = 0; \
    virtual Bool set##SET( Real val ) = 0; \
    virtual Bool set##SET( Vec2 val ) = 0; \
    virtual Bool set##SET( Vec3 val ) = 0; \
    virtual Bool set##SET( Vec4 val ) = 0; \
    virtual Bool set##SET( Pos2 val ) = 0; \
    virtual Bool set##SET( Pos3 val ) = 0; \
    virtual Bool set##SET( Quat val ) = 0; \
    virtual Bool set##SET( Rgba val ) = 0; \
    virtual Bool set##SET( Mat3 val ) = 0; \
    virtual Bool set##SET( Mat4 val ) = 0; \
    virtual Bool set##SET( String val ) = 0; \
    virtual Bool set##SET( INode* val ) = 0; \
    virtual Bool set##SET( BufRef val ) = 0;

    INPUT_DECL_ATTRINFO_SET( Value )
    INPUT_DECL_ATTRINFO_SET( HardMin )
    INPUT_DECL_ATTRINFO_SET( HardMax )
    INPUT_DECL_ATTRINFO_SET( SoftMin )
    INPUT_DECL_ATTRINFO_SET( SoftMax )

#undef INPUT_DECL_ATTRINFO_SET

protected:
    virtual ~IAttrInfo() = default;
};

//=================================================================================================
// IAttr. Implemented by both IInput and IOutput.
//=================================================================================================

class IAttr
{
public:
    virtual String              name() const = 0;
    virtual AttrType            type() const = 0;
    virtual String              typeString() const = 0;
    virtual String              desc() const = 0;

    virtual INode*              node() const = 0;
    virtual String              path() const = 0;
    virtual UInt                index() const = 0;

    virtual Bool                isInput() const = 0;
    virtual Bool                isOutput() const = 0;

    virtual Bool                isBool() const = 0;
    virtual Bool                isInt() const = 0;
    virtual Bool                isUInt() const = 0;
    virtual Bool                isReal() const = 0;
    virtual Bool                isVec2() const = 0;
    virtual Bool                isVec3() const = 0;
    virtual Bool                isVec4() const = 0;
    virtual Bool                isPos2() const = 0;
    virtual Bool                isPos3() const = 0;
    virtual Bool                isQuat() const = 0;
    virtual Bool                isRgba() const = 0;
    virtual Bool                isMat3() const = 0;
    virtual Bool                isMat4() const = 0;
    virtual Bool                isString() const = 0;
    virtual Bool                isNode() const = 0;
    virtual Bool                isBuffer( DataType ofType = DataType_Null ) const = 0;

    // AttrInfo.

    virtual IAttrInfo*          info() const = 0;
    virtual Bool                setInfo( IAttrInfo* info ) = 0;

    virtual Bool                accepts( String type ) const = 0;
    virtual Bool                accepts( INode* node ) const = 0;

    virtual StringVector        accepted() const = 0;
    virtual Bool                setAccepted( StringVector accepted ) = 0;

    virtual String              label( String lang = "auto" ) const = 0;
    virtual Bool                setLabel( String label, String lang = "auto" ) = 0;

    virtual String              help( String lang = "auto" ) const = 0;
    virtual Bool                setHelp( String help, String lang = "auto" ) = 0;

protected:
    virtual ~IAttr() = default;
};

//=================================================================================================
// IInputContainer. Implemented by both IInput and INode.
//=================================================================================================

struct IInputContainer
{
    virtual IInput*             createInput( String name, AttrType type ) = 0;
    virtual IInput*             createArray( String name, AttrType type ) = 0;

    virtual Bool                hasInput( UInt index, AttrType type = AttrType_Null ) const = 0;
    virtual Bool                hasInput( String name, AttrType type = AttrType_Null ) const = 0;

    virtual UInt                inputCount() const = 0;
    virtual IInput*             input( UInt index ) = 0;
    virtual IInput*             input( UInt index ) const = 0;
    virtual IInput*             input( String name ) = 0;
    virtual IInput*             input( String name ) const = 0;

    virtual Bool                removeInput( UInt index ) = 0;
    virtual Bool                removeInput( String name ) = 0;
    virtual Bool                removeLastInput() = 0;
    virtual Bool                clearInputs() = 0;

protected:
    virtual ~IInputContainer() = default;
};

//=================================================================================================
// IInput.
//=================================================================================================

class IInput : public IAttr, public IInputContainer, public IRefCounted
{
public:
    virtual UInt64              hash( StringVector excludes = { } ) const = 0;

    virtual IInput*             meta( String name = "", AttrType type = AttrType_Null ) const = 0;
    virtual Bool                isMeta() const = 0;
    virtual IInput*             parent() const = 0;

    virtual Bool                canConnect( IOutput* output ) const = 0;
    virtual Bool                connect( IOutput* output ) = 0;
    virtual void                disconnect() = 0;

    virtual Bool                connected() const = 0;
    virtual IOutput*            output() const = 0;

    virtual void                disableOutput( Bool disable ) = 0;
    virtual IOutput*            disabledOutput() const = 0;
    virtual Bool                outputDisabled() const = 0;

    virtual AttrType            arrayType() const = 0;
    virtual DataType            bufferType() const = 0;

    virtual IInput*             appendElement() = 0;
    virtual Bool                containsElement( IInput* input ) const = 0;

    virtual Bool                set( IInput* input ) = 0;
    virtual Bool                set( Bool value ) = 0;
    virtual Bool                set( Int value ) = 0;
    virtual Bool                set( UInt value ) = 0;
    virtual Bool                set( Real value ) = 0;
    virtual Bool                set( Vec2 value ) = 0;
    virtual Bool                set( Vec3 value ) = 0;
    virtual Bool                set( Vec4 value ) = 0;
    virtual Bool                set( Pos2 value ) = 0;
    virtual Bool                set( Pos3 value ) = 0;
    virtual Bool                set( Quat value ) = 0;
    virtual Bool                set( Rgba value ) = 0;
    virtual Bool                set( Mat3 value ) = 0;
    virtual Bool                set( Mat4 value ) = 0;
    virtual Bool                set( String value ) = 0;
    virtual Bool                set( const char* value ) = 0;
    virtual Bool                set( INode* value ) = 0;
    virtual Bool                set( BufRef buffer ) = 0;

    virtual Bool                setBool( Bool value ) = 0;
    virtual Bool                setInt( Int value ) = 0;
    virtual Bool                setUInt( UInt value ) = 0;
    virtual Bool                setReal( Real value ) = 0;
    virtual Bool                setVec2( Vec2 value ) = 0;
    virtual Bool                setVec3( Vec3 value ) = 0;
    virtual Bool                setVec4( Vec4 value ) = 0;
    virtual Bool                setPos2( Pos2 value ) = 0;
    virtual Bool                setPos3( Pos3 value ) = 0;
    virtual Bool                setQuat( Quat value ) = 0;
    virtual Bool                setRgba( Rgba value ) = 0;
    virtual Bool                setMat3( Mat3 value ) = 0;
    virtual Bool                setMat4( Mat4 value ) = 0;
    virtual Bool                setString( String value ) = 0;
    virtual Bool                setString( const char* value ) = 0;
    virtual Bool                setNode( INode* value ) = 0;
    virtual Bool                setBuffer( BufRef buffer ) = 0;

    virtual Bool                get( Bool& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Int& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( UInt& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Real& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Vec2& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Vec3& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Vec4& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Pos2& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Pos3& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Quat& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Rgba& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Mat3& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( Mat4& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( String& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( INode*& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                get( BufRef& out, Bool eval, EvalCtx* ctx = nullptr ) const = 0;

    virtual String              valueToString() const = 0;
    virtual String              defaultToString() const = 0;

    virtual Bool                isArray( AttrType ofType = AttrType_Null ) const = 0;
    virtual Bool                isArrayElement( AttrType ofType = AttrType_Null ) const = 0;
    virtual Bool                isObject() const = 0;
    virtual Bool                isObjectArray() const = 0;

#define INPUT_DECL_INPUT_GET(GET) \
    virtual Bool GET( Bool& out ) const = 0; \
    virtual Bool GET( Int& out ) const = 0; \
    virtual Bool GET( UInt& out ) const = 0; \
    virtual Bool GET( Real& out ) const = 0; \
    virtual Bool GET( Vec2& out ) const = 0; \
    virtual Bool GET( Vec3& out ) const = 0; \
    virtual Bool GET( Vec4& out ) const = 0; \
    virtual Bool GET( Pos2& out ) const = 0; \
    virtual Bool GET( Pos3& out ) const = 0; \
    virtual Bool GET( Quat& out ) const = 0; \
    virtual Bool GET( Rgba& out ) const = 0; \
    virtual Bool GET( Mat3& out ) const = 0; \
    virtual Bool GET( Mat4& out ) const = 0; \
    virtual Bool GET( String& out ) const = 0; \
    virtual Bool GET( INode*& out ) const = 0; \
    virtual Bool GET( BufRef& out ) const = 0;

    INPUT_DECL_INPUT_GET( defaultVal )
    INPUT_DECL_INPUT_GET( hardMin )
    INPUT_DECL_INPUT_GET( hardMax )
    INPUT_DECL_INPUT_GET( softMin )
    INPUT_DECL_INPUT_GET( softMax )

#undef INPUT_DECL_INPUT_GET

#define INPUT_DECL_INPUT_SET(SET) \
    virtual Bool set##SET( Bool val ) = 0; \
    virtual Bool set##SET( Int val ) = 0; \
    virtual Bool set##SET( UInt val ) = 0; \
    virtual Bool set##SET( Real val ) = 0; \
    virtual Bool set##SET( Vec2 val ) = 0; \
    virtual Bool set##SET( Vec3 val ) = 0; \
    virtual Bool set##SET( Vec4 val ) = 0; \
    virtual Bool set##SET( Pos2 val ) = 0; \
    virtual Bool set##SET( Pos3 val ) = 0; \
    virtual Bool set##SET( Quat val ) = 0; \
    virtual Bool set##SET( Rgba val ) = 0; \
    virtual Bool set##SET( Mat3 val ) = 0; \
    virtual Bool set##SET( Mat4 val ) = 0; \
    virtual Bool set##SET( String val ) = 0; \
    virtual Bool set##SET( INode* val ) = 0; \
    virtual Bool set##SET( BufRef val ) = 0;

    INPUT_DECL_INPUT_SET( DefaultVal )
    INPUT_DECL_INPUT_SET( HardMin )
    INPUT_DECL_INPUT_SET( HardMax )
    INPUT_DECL_INPUT_SET( SoftMin )
    INPUT_DECL_INPUT_SET( SoftMax )

#undef INPUT_DECL_INPUT_SET

protected:
    virtual ~IInput() = default;
};

//=================================================================================================
// IOutput.
//=================================================================================================

class IOutput : public IAttr, public IRefCounted
{
public:
    virtual UInt64              hash( StringVector excludes = { } ) const = 0;

    virtual Bool                canConnect( IInput* input ) const = 0;
    virtual Bool                connect( IInput* input ) = 0;
    virtual void                disconnect( IInput* input ) = 0;

    virtual Bool                connected() const = 0;
    virtual Bool                connectedTo( IInput* input ) = 0;

    virtual DataType            bufferType() const = 0;

    virtual UInt                inputCount() const = 0;
    virtual IInput*             input( UInt index ) const = 0;

    virtual Bool                eval( Bool& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Int& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( UInt& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Real& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Vec2& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Vec3& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Vec4& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Pos2& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Pos3& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Quat& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Rgba& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Mat3& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( Mat4& out, EvalCtx* ctx = nullptr ) const = 0;
    virtual Bool                eval( String& out, EvalCtx* ctx = nullptr ) const = 0;
  //virtual Bool                eval( INode*& out, EvalCtx* ctx = nullptr ) const = 0; // Not supporting this for now.
    virtual Bool                eval( BufRef& out, EvalCtx* ctx = nullptr ) const = 0;

protected:
    virtual ~IOutput() = default;
};

//=================================================================================================
// ILayout.
//=================================================================================================

class ILayout : public IRefCounted
{
public:
    virtual LayoutType          type() const = 0;
    virtual String              name() const = 0;
    virtual String              path() const = 0;
    virtual String              label( String lang = "auto" ) const = 0;
    virtual Bool                expand() const = 0;
    virtual AttrType            attrType() const = 0;
    virtual AttrType            arrayType() const = 0;
    virtual UInt                layoutCount() const = 0;
    virtual ILayout*            layout( UInt idx ) const = 0;

    virtual Bool                isInvalid() const = 0;
    virtual Bool                isLayout() const = 0;
    virtual Bool                isHeader() const = 0;
    virtual Bool                isInput() const = 0;
    virtual Bool                isObject() const = 0;
    virtual Bool                isObjectArray() const = 0;
    virtual Bool                isSeparator() const = 0;

protected:
    virtual ~ILayout() = default;
};

//=================================================================================================
// INodeIterator.
//=================================================================================================

class INodeIterator : public IRefCounted
{
public:
    virtual UInt                count() const = 0;
    virtual Bool                contains( INode* node ) const = 0;

    virtual INode*              first() = 0;
    virtual INode*              next() = 0;

protected:
    virtual ~INodeIterator() = default;
};

//=================================================================================================
// IPathIterator.
//=================================================================================================

class IPathIterator : public IRefCounted
{
public:
    virtual UInt                count() const = 0;

    virtual Bool                contains( IPath* path ) const = 0;
    virtual Bool                contains( INode* node ) const = 0;

    virtual IPath*              first() = 0;
    virtual IPath*              next() = 0;

protected:
    virtual ~IPathIterator() = default;
};

//=================================================================================================
// INode.
//=================================================================================================

class INode : public IInputContainer, public IRefCounted
{
public:
    virtual Bool                subscribe( NodeObserver* observer ) = 0;
    virtual Bool                unsubscribe( NodeObserver* observer ) = 0;
    virtual Bool                unsubscribeAll() = 0;

    virtual IInput*             meta() const = 0;
    virtual IScene*             scene( Bool local = false ) const = 0;
    virtual UInt64              hash( StringVector excludes = { } ) const = 0;

    virtual Bool                dirty( Bool checkDeps = true ) const = 0;
    virtual void                setDirty( Bool dirty = true ) = 0;

    virtual String              type() const = 0;
    virtual StringVector        bases() const = 0;
    virtual INode*              convertTo( String type ) = 0;

    virtual Bool                isAbstract() const = 0;
    virtual Bool                isInternal() const = 0;

    virtual Bool                isTypeOf( String type ) const = 0;
    virtual Bool                isTypeOf( StringVector types ) const = 0;
    virtual String              typeDesc() const = 0;

    virtual String              id() const = 0;
    virtual String              name() const = 0; // Deprecated, use id().
    virtual String              label( String lang = "auto" ) const = 0;
    virtual String              help( String lang = "auto" ) const = 0;

    virtual ILayout*            layout() const = 0;
    virtual void                setLayout( ILayout* layout ) = 0;

    virtual String              displayName() const = 0;
    virtual Bool                setDisplayName( String name ) = 0;

    virtual IInput*             findInput( String path ) const = 0;
    virtual IOutput*            findOutput( String path ) const = 0;

    virtual IInput*             parseInputPath( String path ) = 0;

    virtual Bool                hasOutput( UInt index, AttrType type = AttrType_Null ) const = 0;
    virtual Bool                hasOutput( String name, AttrType type = AttrType_Null ) const = 0;

    virtual UInt                outputCount() const = 0;
    virtual IOutput*            output( UInt index ) = 0;
    virtual IOutput*            output( UInt index ) const = 0;
    virtual IOutput*            output( String name ) = 0;
    virtual IOutput*            output( String name ) const = 0;

    virtual IOutput*            createOutput( String name, AttrType type ) = 0;
    virtual Bool                removeOutput( String name ) = 0;
    virtual Bool                removeOutput( UInt index ) = 0;
    virtual Bool                removeLastOutput() = 0;
    virtual Bool                clearOutputs() = 0;

    virtual Bool                isPath() const = 0;
    virtual String              asPath() const = 0;
    virtual Bool                setPath( String dir, String file, String ext ) = 0;
    virtual Bool                setPath( String path ) = 0;

    virtual INodeIterator*      children( Int depth = -1 ) const = 0;
    virtual Int                 indexIn( INode* parent ) const = 0;
    virtual Bool                parentTo( INode* parent ) = 0;
    virtual Bool                unparentFrom( INode* parent ) = 0;

    virtual IPathIterator*      paths( String type = "", Bool excludeHidden = false ) const = 0;
    virtual IPathIterator*      paths( StringVector types, Bool excludeHidden = false ) const = 0;

    virtual IPathIterator*      pathsTo( INode* node, Bool excludeHidden = false ) const = 0;
    virtual IPathIterator*      pathsTo( String name, Bool excludeHidden = false ) const = 0;

    virtual IPath*              firstPathTo( INode* node, Bool excludeHidden = false ) const = 0;
    virtual IPath*              firstPathTo( String name, Bool excludeHidden = false ) const = 0;

    virtual String              nodeDefPath() const = 0;

    virtual Bool                hasNodeInit() const = 0;
    virtual Bool                hasNodePrep() const = 0;
    virtual Bool                hasNodeEval() const = 0;
    virtual Bool                hasNodeModify() const = 0;
    virtual Bool                hasNodeFree() const = 0;

    virtual Bool                nodeInit() = 0;
    virtual Bool                nodePrep() const = 0;
    virtual Bool                nodeEval( EvalCtx* ctx, void* output ) const = 0;
    virtual Bool                nodeModify( INode* target = nullptr ) = 0;
    virtual Bool                nodeFree() = 0;

    virtual const void*         data() const = 0;

    virtual String              svgIcon( Bool returnEmptyStringIfMissing = false ) const = 0;
    virtual String              htmlHelp( Bool asStandaloneDoc = false, String lang = "auto" ) const = 0;

    virtual INodeIterator*      dependencies( Bool followOutputs = false, Bool topoSort = false ) const = 0;
    virtual String              serialize( Bool withDeps = true, Bool writeDefaults = false ) const = 0;
    virtual INode*              clone( Bool withDeps = true ) = 0;

    virtual String              checkMesh() const = 0;

    virtual UInt                pointCloudCount() const = 0;
    virtual BufRef              pointCloud( Real detail = 1.0, Int maxPoints = -1 ) = 0;

protected:
    virtual ~INode() = default;
};

//=================================================================================================
// IPath.
//=================================================================================================

class IPath : public IRefCounted
{
public:
    virtual String              path() const = 0;
    virtual UInt64              hash() const = 0;
    virtual Bool                dirty() const = 0;

    virtual INodeIterator*      nodes() const = 0;
    virtual UInt                count() const = 0;
    virtual Bool                contains( INode* node ) const = 0;

    virtual INode*              root() const = 0;
    virtual INode*              leaf() const = 0;
    virtual INode*              parent( UInt revIdx = 0 ) const = 0;

    virtual Mat4                transform( Real time = 0.0, UInt instancerIdx = ( UInt )-1 ) const = 0;

    virtual Bool                visible() const = 0;
    virtual INode*              material() const = 0;
    virtual INode*              subdivision() const = 0;

protected:
    virtual ~IPath() = default;
};

//=================================================================================================
// IScene.
//=================================================================================================

class IScene : public IRefCounted
{
public:
    virtual Int64               id() const = 0;
    virtual UInt64              hash( StringVector excludes = { } ) const = 0;

    virtual UInt64              fileVersion() const = 0;
    virtual String              fileTime() const = 0;
    virtual StringVector        fileComments() const = 0;

    virtual Bool                subscribe( SceneObserver* observer ) = 0;
    virtual Bool                unsubscribe( SceneObserver* observer ) = 0;
    virtual Bool                unsubscribeAll() = 0;

    virtual INodeIterator*      defs( String category = "" ) const = 0;
    virtual UInt                defCount() const = 0;
    virtual StringVector        defCategories() const = 0;

    virtual UInt                clearDefs() = 0;
    virtual Bool                parseDefs( String json ) = 0;
    virtual Bool                loadDefs( String path = "" ) = 0;

    virtual INode*              defineNode( String name ) = 0;
    virtual INode*              findDef( String type ) const = 0;
    virtual INode*              findDef( UInt index ) const = 0;

    virtual UInt                nodeCount() const = 0;
    virtual INode*              createNode( String type, String name = "", String displayName = "" ) = 0;
    virtual INode*              cloneNode( INode* node, Bool withDeps = true, Bool adjustDisplayNames = true ) = 0;
    virtual INode*              cloneNode( String name, Bool withDeps = true, Bool adjustDisplayNames = true ) = 0;
    virtual INodeIterator*      cloneNodes( StringVector names, Bool withDeps = true, Bool adjustDisplayNames = true ) = 0;
    virtual INode*              convertNode( INode* node, String newType ) = 0;

    virtual Bool                canRemoveNode( INode* node ) const = 0;
    virtual Bool                removeNode( INode* node ) = 0;
    virtual Bool                removeNode( String name ) = 0;
    virtual INode*              restoreNode( String name ) = 0;

    virtual StringVector        removeNodes( String ofType ) = 0;
    virtual StringVector        removeNodes( StringVector names ) = 0;
    virtual UInt                clearNodes( Bool unreferencedOnly = false ) = 0;

    virtual INode*              findNode( String name ) const = 0;
    virtual INodeIterator*      findNodes( StringVector names, Bool topoSort = false, Bool withDeps = false ) const = 0;

    virtual IInput*             parseInputPath( String path ) = 0;

    virtual StringVector        nodeTypes() const = 0;

    virtual StringVector        nodeNames( String type = "" ) const = 0;
    virtual StringVector        nodeNames( StringVector types ) const = 0;

    virtual INodeIterator*      nodes( String type = "", Bool topoSort = false ) const = 0;
    virtual INodeIterator*      nodes( StringVector types, Bool topoSort = false ) const = 0;

    virtual IPathIterator*      paths( String type = "", Bool excludeHidden = false ) const = 0;
    virtual IPathIterator*      paths( StringVector types, Bool excludeHidden = false ) const = 0;

    virtual IPathIterator*      pathsTo( INode* node, Bool excludeHidden = false ) const = 0;
    virtual IPathIterator*      pathsTo( String name, Bool excludeHidden = false ) const = 0;

    virtual IPath*              firstPathTo( INode* node, Bool excludeHidden = false ) const = 0;
    virtual IPath*              firstPathTo( String name, Bool excludeHidden = false ) const = 0;

    virtual INode*              global( Bool create = true ) const = 0;
    virtual INode*              state( Bool create = true ) const = 0;
    virtual INode*              world( Bool create = true ) const = 0;
    virtual INode*              settings( Bool create = true ) const = 0;

    virtual String              ocioConfig() const = 0;
    virtual String              applyOcioConfig() const = 0;
    virtual String              colorSpace() const = 0;

    virtual INode*              camera() const = 0;
    virtual INode*              environment() const = 0;
    virtual INode*              sun() const = 0;
    virtual INode*              solarContext() const = 0;
    virtual INode*              groundPlane() const = 0;

    virtual Mat4                axisSystem() const = 0;
    virtual Vec3                scale() const = 0;

    virtual INode*              beautyPass() const = 0;     // This returns settings:beautyPass.
    virtual INodeIterator*      beautyPasses() const = 0;   // This returns all beauty passes.
    virtual INodeIterator*      extraPasses() const = 0;
    virtual INodeIterator*      allPasses() const = 0;

    virtual INode*              defaultMaterial() const = 0;
    virtual INode*              overrideMaterial() const = 0;

    virtual INodeIterator*      stateDependencies( StringVector types = { }, Bool topoSort = false ) const = 0;
    virtual INodeIterator*      unreferencedNodes( StringVector types = { } ) const = 0;

    virtual Bool                parse( String text ) = 0;
    virtual StringVector        supportedFileTypes() const = 0;
    virtual Bool                canRead( String path ) const = 0;
    virtual INode*              read( String path, Bool asFragment = false, SceneProgress* progress = nullptr ) = 0;
    virtual INode*              import( String pathOrFragment, SceneProgress * progress = nullptr, Bool adjustDisplayNames = true ) = 0;
    virtual Bool                write( String path, SceneProgress* progress = nullptr ) = 0;
    virtual IScene*             clone() const = 0;

    virtual String              applyMacros( String path ) const = 0;

    virtual String              location( Bool applyMacros = true ) const = 0;
    virtual String              setLocation( String path ) = 0;

    virtual String              outputName( Bool applyMacros = true ) const = 0;
    virtual String              outputDir( Bool useFallbacks = true ) const = 0;
    virtual String              outputPath( INode* pass ) const = 0;

    virtual String              resolveInputPath( INode* inputPath, Bool updateNode = true ) = 0;
    virtual Bool                resolveInputPaths( Bool updateNodes = true ) = 0;

    virtual UInt                numThreads( Bool forIPR = false ) const = 0;

    virtual String              bsiPath( INode* beautyPass ) const = 0;
    virtual String              bsiTempPath( INode* beautyPass ) const = 0;
    virtual Bool                canResumeBsi( INode* beautyPass ) const = 0;

    virtual String              htmlHelp( String lang = "auto" ) const = 0;
    virtual String              writeHtmlHelp( String& status, String lang = "auto" ) = 0;
    virtual Bool                showHtmlHelp( String& status, String lang = "auto" ) = 0;

    virtual void                enableUndo() = 0;
    virtual void                disableUndo() = 0;
    virtual Bool                undoEnabled() const = 0;

    virtual Int                 undoLimit() const = 0;
    virtual void                setUndoLimit( Int num = -1 ) = 0;
    virtual void                clearUndo() = 0;

    virtual Bool                canUndo() const = 0;
    virtual Bool                canRedo() const = 0;

    virtual Bool                undo( UInt num = 1 ) = 0;
    virtual Bool                redo( UInt num = 1 ) = 0;

    virtual StringVector        undoList() const = 0;
    virtual StringVector        redoList() const = 0;

    virtual void                pushUndoGroup() = 0;
    virtual void                popUndoGroup( String undoDesc = "", String redoDesc = "" ) = 0;
    virtual Bool                inUndoGroup() const = 0;

    virtual void                enableEvents() = 0;
    virtual void                disableEvents() = 0;
    virtual Bool                eventsEnabled() const = 0;

    virtual void                pushEventGroup() = 0;
    virtual void                popEventGroup() = 0;
    virtual Bool                inEventGroup() const = 0;

    virtual Bool                isDirty() const = 0;

    virtual StringVector        allResources( Bool includeMissing = true ) = 0;
    virtual StringVector        referencedResources( Bool includeMissing = false ) = 0;
    virtual StringVector        missingResources( Bool referencedOnly = true ) = 0;
    virtual Bool                gatherResources( Bool referencedOnly = true ) = 0;

protected:
    virtual ~IScene() = default;
};

//=================================================================================================
// IPbrTextureSet.
//=================================================================================================

class IPbrTextureSet : public IRefCounted
{
public:
    virtual UInt                resolve( String path, INode* pbr = nullptr  ) = 0;
    virtual String              summary() const = 0;

    virtual void                addMatch( String type, String match ) = 0;
    virtual void                setMatches( String type, StringVector matches ) = 0;
    virtual StringVector        getMatches( String type ) const = 0;

    virtual String              inputPath() const = 0;
    virtual StringVector        resolvedPaths() const = 0;
    virtual UInt                numResolvedPaths() const = 0;

    virtual Bool                applyTo( INode* node ) const = 0;

    virtual String              opacity() const = 0;
    virtual String              color() const = 0;

    virtual String              metallic() const = 0;
    virtual String              roughness() const = 0;

    virtual String              specular() const = 0;
    virtual String              glossiness() const = 0;

    virtual String              aniso() const = 0;
    virtual String              anisoRotation() const = 0;

    virtual String              bump() const = 0;
    virtual String              normal() const = 0;
    virtual String              displacement() const = 0;

    virtual String              emission() const = 0;
    virtual String              scattering() const = 0;
    virtual String              ao() const = 0;

protected:
    virtual ~IPbrTextureSet() = default;
};

} // interfaces
} // bella_sdk
} // dl

#endif // BELLA_SDK_SCENEAPI_H
