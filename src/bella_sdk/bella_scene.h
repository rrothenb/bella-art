// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef BELLA_SDK_SCENE_H
#define BELLA_SDK_SCENE_H

#include "api.h"
#include "bella_sceneapi.h"

namespace dl {
namespace bella_sdk {

using namespace interfaces;

//=================================================================================================
// Forwards.
//=================================================================================================

class EnumItem;
class Input;
class Output;
class Layout;
class Node;
class Path;
class Scene;

class InputVector;
class OutputVector;
class NodeVector;
class LayoutVector;
class EnumItemVector;
class PathVector;

struct NodeObserver;
struct SceneObserver;
class PbrTextureSet;

class Image;
class ColorConfig;

//=================================================================================================
// EnumItem.
// 
// This class is used to represent enum attributes, which may have arbitrary values. To know which
// value to use, consult the type of the attribute.
//=================================================================================================

class DL_API EnumItem DL_FINAL : public RefersTo<IEnumItem>
{
public:
    EnumItem() = default;
    EnumItem( IEnumItem* impl ) : RefersTo( impl ) { }

    Int                         index() const;
    Bool                        isValid() const; // Just a shortcut for index() >= 0.

    Int                         intValue() const;
    Real                        realValue() const;
    String                      strValue() const;

    String                      label( String lang = "auto" ) const;
    String                      help( String lang = "auto" ) const;
};

//=================================================================================================
// Input.
// 
// Nodes have inputs, and inputs may have inputs as well. Changing the value of an input will
// cause a running Engine to update.
//=================================================================================================

class DL_API Input DL_FINAL : public RefersTo<IInput>
{
public:
    Input() = default;
    Input( IInput* impl ) : RefersTo( impl ) { }

    // An input's name, which must be a valid C identifier, never changes.
    //
    String                      name() const;

    // This input's type. An input may be created ad hoc, in which case it will be AttrType_Null
    // until some value is assigned to it.
    //
    AttrType                    type() const;

    // A friendlier version of this input's type. For example, may return "vec3f[]", where the
    // input is of type AttrType_Buffer, with buffer type DataType_Vec3f.
    //
    String                      typeString() const;

    // A more detailed description of this input.
    //
    String                      desc() const;

    // Not typically useful, this is used internally in our object lifetime tests.
    //
    static Int                  instanceCount();

    // Get a hash for this input. This is a bit expensive, as it computes the hashes of any child
    // attrs, as well as of its output, if connected. This is because it is typically used to check
    // whether things are dirty and in need of update. Excludes may be node type names, node names,
    // input names and output names.
    //
    UInt64                      hash( StringVector excludes = { } ) const;

    // "meta" is something you should probably ignore. Since it is possible to define nodes and add
    // or remove attrs at runtime, it is sometimes useful to add metadata to a node or an input,
    // and this is done by just adding more Inputs, under this special "_meta" input on the node
    // or parent input.
    //
    Input                       meta( String name = "", AttrType type = AttrType_Null ) const;
    Bool                        isMeta() const;

    // Get the parent node of this input. Inputs always have a parent node.
    //
    Node                        node() const;

    // Get the "path" for this input, which is a unique path starting at this input's node's name
    // and going through intervening inputs, e.g. "myNode.someArray[1].someAttr.thisAttr". Inputs
    // may be located in a scene using their path.
    //
    String                      path() const;

    // Get this input's index in its parent node or attr.
    //
    UInt                        index() const;

    // Get this attr's parent attr, if it has one. If not, it is a direct child of its node.
    //
    Input                       parent() const;

    // Get the array or buffer type, for an attr that is of type array or buffer.
    //
    AttrType                    arrayType() const;
    DataType                    bufferType() const;

    // Connect or disconnect this input to/from an output.
    //
    Bool                        canConnect( Output output ) const;
    Bool                        connect( Output output );
    void                        disconnect();
    void                        operator|=( Output output ); // NOTE: disconnect using input|=nullptr.

    // Find out whether this input is connected, and get the output to which it is connected.
    //
    Bool                        connected() const;
    Output                      output() const;

    // We allow temporarily disregarding a connected output, in which case the input's immediate
    // value will be used. Technically, this is done by moving the connected output to a meta input
    // so that other logic continues to work as usual. If a disabled input is disconnected or
    // connected, this will be reset (i.e. the meta input will be disconnected), and evaluation
    // will once again be enabled as usual. These actions are bundled into singular undo records.
    // This will have no effect on inputs of type null, node, buffer, array, or object.
    // 
    // If you think about how much logic is based on whether outputs are connected, from computing
    // dependencies to writing files, it becomes clear that this actual disconnection is necessary,
    // e.g. as opposed to storing a bool flag and simply pretending the input is disconnected. Such
    // a solution would also necessitate altering the file formats to store the flag, whereas this
    // is made unnecessary by actually moving the output to a meta input.
    //
    void                        disableOutput( Bool disable );
    Output                      disabledOutput() const;
    Bool                        outputDisabled() const;

    // Usually not used, these are methods for creating child inputs. Input names must be valid C
    // identifiers, and lookup is case-insensitive, so it is not allowed to define an input with a
    // name differing only by case from another input. If you attempt to do this the existing input
    // will be returned instead, and errors will be printed if you attempt to use it as the wrong
    // input type.
    //
    Input                       createInput( String name, AttrType type );
    Input                       createArray( String name, AttrType type );

    // If this is an array input, append a new element. There is a detail here, arising from the
    // design of the system, in which there is always at least one element for an object[] input.
    // The reason for this is that we have no concept of node template, and instead use the first
    // element as blueprint for further elements.
    //
    Input                       appendElement();

    // Find whether an input is an element of an array input.
    //
    Bool                        containsElement( Input input ) const;

    // Check if this input has a child of given type. Note that since it is not allowed to define
    // attrs whose names differ only by case, input lookups can be case insensitive.
    //
    Bool                        hasInput( UInt index, AttrType type = AttrType_Null ) const;
    Bool                        hasInput( String name, AttrType type = AttrType_Null ) const;

    // Find out how many inputs this node has, and retrieve them by index or name. The non-const
    // overloads taking a name will create a null input, which will take its type from the first
    // time you assign something to it. The others will return a null input (use appendElement()
    // to add elements to an array input).
    //
    // Note that there is a special case for inputs of type AttrType_Node, where if the node
    // reference is not null, we will look up the given name in the referenced node, and return an
    // input from it. So instead of writing "node["a"].asNode()["b"] = 1.0;" you can simply write
    // "node["a"]["b"] = 1.0;".
    //
    UInt                        inputCount() const;
    Input                       input( UInt index );
    Input                       input( UInt index ) const;
    Input                       input( String name );
    Input                       input( String name ) const;
    InputVector                 inputs() const;

    // Get an input by indexing. Since it is fine for an input to be null, you can chain these
    // operators together for convenient use that would otherwise involve a lot of null checking
    // (and incRef/decRef calls), like: "a["b"]["c"]["d"].asReal()". You can always check for null
    // if necessary using isNull() or the bang operator (implemented by RefersTo<T>). These use
    // the input(name) and input(index) methods internally, so have the same behavior regarding
    // creation of ad hoc inputs.
    //
    Input                       operator[]( UInt index );
    Input                       operator[]( UInt index ) const;
    Input                       operator[]( String name );
    Input                       operator[]( String name ) const;

    // Remove child inputs. You should use clearInputs() if that is what you are trying to do, as
    // it will take care of preserving the first element "template" input of object[] inputs.
    //
    Bool                        removeInput( UInt index );
    Bool                        removeInput( String name );
    Bool                        removeLastInput();
    Bool                        clearInputs();

    // Shortcuts for getting this input's type.
    //
    Bool                        isBool() const;
    Bool                        isInt() const;
    Bool                        isUInt() const;
    Bool                        isReal() const;
    Bool                        isVec2() const;
    Bool                        isVec3() const;
    Bool                        isVec4() const;
    Bool                        isPos2() const;
    Bool                        isPos3() const;
    Bool                        isQuat() const;
    Bool                        isRgba() const;
    Bool                        isMat3() const;
    Bool                        isMat4() const;
    Bool                        isString() const;
    Bool                        isNode() const;
    Bool                        isBuffer( DataType ofType = DataType_Null ) const;
    Bool                        isArray( AttrType ofType = AttrType_Null ) const;
    Bool                        isArrayElement( AttrType ofType = AttrType_Null ) const;
    Bool                        isObject() const;
    Bool                        isObjectArray() const;

    // Get this attr's value, optionally evaluating its output if connected. If no EvalCtx is given
    // a default one will be used.
    //
    Bool                        asBool( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Int                         asInt( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    UInt                        asUInt( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Real                        asReal( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Vec2                        asVec2( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Vec3                        asVec3( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Vec4                        asVec4( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Pos2                        asPos2( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Pos3                        asPos3( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Quat                        asQuat( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Rgba                        asRgba( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Mat3                        asMat3( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Mat4                        asMat4( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    String                      asString( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    BufRef                      asBuffer( Bool eval = false, EvalCtx* ctx = nullptr ) const;
    Node                        asNode( Bool eval = false, EvalCtx* ctx = nullptr ) const; // NOTE: this is never evaluated.
    template<typename T> BufRefT<T> asBufferT( Bool eval = false, EvalCtx* ctx = nullptr ) const { return asBuffer( eval, ctx ).as<T>(); }

    // Get a nice string describing this input's value or default value.
    //
    String                      valueToString() const;
    String                      defaultToString() const;

    // Set this input's value implcitly. Note that the set(Input) overload does not set the given
    // input as this input's value (since there are not inputs of type input), rather it reads the
    // input's value and sets it in this input. So it is like calling setBool(input.asBool()), but
    // prevents you needing to know the type.
    //
    Bool                        set( Input input );
    Bool                        set( Bool value );
    Bool                        set( Int value );
    Bool                        set( UInt value );
    Bool                        set( Real value );
    Bool                        set( Vec2 value );
    Bool                        set( Vec3 value );
    Bool                        set( Vec4 value );
    Bool                        set( Pos2 value );
    Bool                        set( Pos3 value );
    Bool                        set( Quat value );
    Bool                        set( Rgba value );
    Bool                        set( Mat3 value );
    Bool                        set( Mat4 value );
    Bool                        set( String value );
    Bool                        set( const char* value );
    Bool                        set( Node value );
    Bool                        set( BufRef buffer );

    // Set this input's value explicitly.
    //
    Bool                        setBool( Bool value );
    Bool                        setInt( Int value );
    Bool                        setUInt( UInt value );
    Bool                        setReal( Real value );
    Bool                        setVec2( Vec2 value );
    Bool                        setVec3( Vec3 value );
    Bool                        setVec4( Vec4 value );
    Bool                        setPos2( Pos2 value );
    Bool                        setPos3( Pos3 value );
    Bool                        setQuat( Quat value );
    Bool                        setRgba( Rgba value );
    Bool                        setMat3( Mat3 value );
    Bool                        setMat4( Mat4 value );
    Bool                        setString( String value );
    Bool                        setString( const char* value );
    Bool                        setNode( Node value );
    Bool                        setBuffer( BufRef value );

    // NOTE: there is no operator=(Input) since it would break reference assignment and equality,
    // so you must use set(Input) to set input values generically from other inputs.
    //
    void                        operator=( Bool value );
    void                        operator=( Int  value );
    void                        operator=( UInt value );
    void                        operator=( Real value );
    void                        operator=( Vec2 value );
    void                        operator=( Vec3 value );
    void                        operator=( Vec4 value );
    void                        operator=( Pos2 value );
    void                        operator=( Pos3 value );
    void                        operator=( Quat value );
    void                        operator=( Rgba value );
    void                        operator=( Mat3 value );
    void                        operator=( Mat4 value );
    void                        operator=( String value );
    void                        operator=( const char* value );
    void                        operator=( Node value );
    void                        operator=( BufRef value );
    template<typename T> void   operator=( BufRefT<T> value ) { setBuffer( BufRef( value ) ); }

    // Inputs may specify the types of nodes they can reference (for node references), or the types
    // of nodes that may be connected to them. Since array element inputs do not have this info,
    // they return the value given by their parent array input.
    // 
    // Note that the string overload will only check the given string against the accepts() list,
    // but that the node overload will run through asking if node.isTypeOf(type) for each accepted
    // type.
    //
    Bool                        accepts( String type ) const;
    Bool                        accepts( Node node ) const;
    StringVector                accepted() const;
    Bool                        setAccepted( StringVector accepted );

    // Label is a friendly name for the input, eventually localizable. Usually set in .bnd.
    //
    String                      label( String lang = "auto" ) const;
    Bool                        setLabel( String label, String lang = "auto" );

    // Help is a string associated with the attr, eventually localizable. Usually set in .bnd.
    //
    String                      help( String lang = "auto" ) const;
    Bool                        setHelp( String help, String lang = "auto" );

    // Precision is the number of decimals appropriate for this attr in a GUI. Usually set in .bnd.
    //
    UInt                        precision() const;
    Bool                        setPrecision( UInt precision );

    // Step is the amount to increase this attr for each mouse detent or spinbox increment. Usually
    // set in .bnd.
    //
    Real                        step() const;
    Bool                        setStep( Real step );

    // Most attrs are fine to update in realtime as GUI controls are used, but some are better off
    // if their change is applied only when the user stops changing the value. This does not change
    // how the input works, it is only a hint to GUI code about how to handle it.
    //
    Bool                        realtime() const;
    Bool                        setRealtime( Bool realtime );

    // This is a units string to show in GUIs. Usually set in .bnd.
    //
    String                      units() const;
    Bool                        setUnits( String units );

    // Indicates whether this attr is of type enum, and allows getting the various enum values.
    //
    Bool                        isEnum() const;
    UInt                        enumItemCount() const;
    EnumItem                    enumItem( UInt index ) const;
    EnumItemVector              enumItems() const;

    // Find the enum item matching a value. A null EnumItem is returned if no match is found.
    //
    EnumItem                    findEnumItem( Int intValue ) const;
    EnumItem                    findEnumItem( Real realValue ) const;
    EnumItem                    findEnumItem( String strValue ) const;

    // Add enum items for this input. Only used when constructing nodes at runtime.
    //
    EnumItem                    addEnumItem( Int value, String label = "", String help = "", String lang = "auto" );
    EnumItem                    addEnumItem( Real value, String label = "", String help = "", String lang = "auto" );
    EnumItem                    addEnumItem( String value, String label = "", String help = "", String lang = "auto" );

    // Each input attr has a default value, as well as hard & soft min/max values. Hard min/max
    // are intended to be the absolute limits for the attr, while soft min/max are used to decide
    // which range to show by default in a GUI (i.e. they indicate the typically useful range).
    //
    template<typename T> T      defaultVal( T def = T() ) const { T t; return ( impl() && impl()->defaultVal( t ) ) ? t : def; }
    template<typename T> T      hardMin( T def = T() ) const { T t; return ( impl() && impl()->hardMin( t ) ) ? t : def; }
    template<typename T> T      hardMax( T def = T() ) const { T t; return ( impl() && impl()->hardMax( t ) ) ? t : def; }
    template<typename T> T      softMin( T def = T() ) const { T t; return ( impl() && impl()->softMin( t ) ) ? t : def; }
    template<typename T> T      softMax( T def = T() ) const { T t; return ( impl() && impl()->softMax( t ) ) ? t : def; }

    template<typename T> Bool   setDefaultVal( T val ) { return impl() && impl()->setDefaultVal( val ); }
    template<typename T> Bool   setHardMin( T val ) { return impl() && impl()->setHardMin( val ); }
    template<typename T> Bool   setHardMax( T val ) { return impl() && impl()->setHardMax( val ); }
    template<typename T> Bool   setSoftMin( T val ) { return impl() && impl()->setSoftMin( val ); }
    template<typename T> Bool   setSoftMax( T val ) { return impl() && impl()->setSoftMax( val ); }
};

//=================================================================================================
// Output.
// 
// Scalar outputs are expected to return values in the range [0,1].
//=================================================================================================

class DL_API Output DL_FINAL : public RefersTo<IOutput>
{
public:
    Output() = default;
    Output( IOutput* impl ) : RefersTo( impl ) { }

    // An output's name, which must be a valid C identifier, never changes.
    //
    String                      name() const;

    // This output's type. Unlike inputs, outputs are not created ad hoc.
    //
    AttrType                    type() const;

    // A friendlier version of this output's type. For example, may return "vec3f[]", where the
    // output is of type AttrType_Buffer, with buffer type DataType_Vec3f.
    //
    String                      typeString() const;

    // A more detailed description of this output.
    //
    String                      desc() const;

    // Not typically useful, this is used internally in our object lifetime tests.
    //
    static Int                  instanceCount();

    // Get a hash for this output. This can be a bit expensive as it must compute the hash of its
    // node as well. Excludes may be node type names, node names, input names and output names.
    //
    UInt                        hash( StringVector excludes = { } ) const;

    // Get the parent node of this output. Outputs always have a parent node, but unlike inputs,
    // they do not have a "parent()" method, as they are always direct children of a node.
    //
    Node                        node() const;

    // Get the "path" for this attr, which is a unique path consisting of this output's node's name
    // and the output's name, e.g. "myNode.myOutput". Outputs may be located in a scene using their
    // path
    //
    String                      path() const;

    // Get this output's index in its parent node or attr.
    //
    UInt                        index() const;

    // Outputs may be buffers, but unlike inputs they are never arrays.
    //
    DataType                    bufferType() const;

    // Connect or disconnect this output to/from an input.
    //
    Bool                        canConnect( Input input ) const;
    Bool                        connect( Input input );
    void                        disconnect( Input input );
    Bool                        operator+=( Input input );
    void                        operator-=( Input input );

    // Find out whether this output is just connected, or connected to a specific input.
    //
    Bool                        connected() const;
    Bool                        connectedTo( Input input );

    // Get information about the inputs connected to this output. Where inputs are connected to at
    // most one output, outputs may be connected to multiple inputs.
    //
    UInt                        inputCount() const;
    Input                       input( UInt index );
    Input                       input( UInt index ) const;
    InputVector                 inputs() const;

    Input                       operator[]( UInt index );
    Input                       operator[]( UInt index ) const;

    // Shortcuts for getting this output's type.
    //
    Bool                        isBool() const;
    Bool                        isInt() const;
    Bool                        isUInt() const;
    Bool                        isReal() const;
    Bool                        isVec2() const;
    Bool                        isVec3() const;
    Bool                        isVec4() const;
    Bool                        isPos2() const;
    Bool                        isPos3() const;
    Bool                        isQuat() const;
    Bool                        isRgba() const;
    Bool                        isMat3() const;
    Bool                        isMat4() const;
    Bool                        isString() const;
    Bool                        isNode() const;
    Bool                        isBuffer( DataType ofType = DataType_Null ) const;

    // Evaluate this output as a given type. If no EvalCtx is given, a default one will be used.
    //
    Bool                        asBool( EvalCtx* ctx = nullptr ) const;
    Int                         asInt( EvalCtx* ctx = nullptr ) const;
    UInt                        asUInt( EvalCtx* ctx = nullptr ) const;
    Real                        asReal( EvalCtx* ctx = nullptr ) const;
    Vec2                        asVec2( EvalCtx* ctx = nullptr ) const;
    Vec3                        asVec3( EvalCtx* ctx = nullptr ) const;
    Vec4                        asVec4( EvalCtx* ctx = nullptr ) const;
    Pos2                        asPos2( EvalCtx* ctx = nullptr ) const;
    Pos3                        asPos3( EvalCtx* ctx = nullptr ) const;
    Quat                        asQuat( EvalCtx* ctx = nullptr ) const;
    Rgba                        asRgba( EvalCtx* ctx = nullptr ) const;
    Mat3                        asMat3( EvalCtx* ctx = nullptr ) const;
    Mat4                        asMat4( EvalCtx* ctx = nullptr ) const;
    String                      asString( EvalCtx* ctx = nullptr ) const;
    BufRef                      asBuffer( EvalCtx* ctx = nullptr ) const;
    Node                        asNode( EvalCtx* ctx = nullptr ) const; // NOTE: not supporting this for now.
    template<typename T> BufRefT<T> asBufferT( EvalCtx* ctx = nullptr ) const { return asBuffer( ctx ).as<T>(); }

    // This is a helper for sampling textures. Given a network of texture nodes, you can get the
    // outColor output of the last one in the chain and sample it across uv space. Sampling this
    // outColor will in turn cause any inputs of the parent node to evaluate their connected
    // outputs, and so forth.
    //
    Rgba                        sample( Vec2 uv ) const;

    // Outputs may specify the types of nodes allowed to connect to them.
    //
    Bool                        accepts( String type ) const;
    Bool                        accepts( Node node ) const;
    StringVector                accepted() const;
    Bool                        setAccepted( StringVector accepted );

    // Label is a friendly name for the output, eventually localizable. Usually set in .bnd.
    //
    String                      label( String lang = "auto" ) const;
    Bool                        setLabel( String label, String lang = "auto" );

    // Help is a string associated with the output, eventually localizable. Usually set in .bnd.
    //
    String                      help( String lang = "auto" ) const;
    Bool                        setHelp( String help, String lang = "auto" );
};

//=================================================================================================
// Layout.
// 
// A nested structure built from a .bnd layout, which describes how to present controls for a
// node's GUI. Since nodes & inputs have well known structures & types, this makes it possible to
// generate GUI algorithmically. You can do that also without a layout, but there is no guarantee
// as to the orderinng of inputs, nor will you get extra info like where to put headers, separators
// and such.
//=================================================================================================

class DL_API Layout DL_FINAL : public RefersTo<ILayout>
{
public:
    Layout() = default;
    Layout( ILayout* impl ) : RefersTo( impl ) { }

    LayoutType                  type() const;
    String                      name() const;
    String                      path() const;
    String                      label( String lang = "auto" ) const;
    Bool                        expand() const;
    AttrType                    attrType() const;
    AttrType                    arrayType() const;
    UInt                        layoutCount() const;
    Layout                      layout( UInt index ) const;
    Layout                      operator[]( UInt index ) const;
    LayoutVector                layouts() const;

    Bool                        isInvalid() const;
    Bool                        isLayout() const;
    Bool                        isHeader() const;
    Bool                        isInput() const;
    Bool                        isObject() const;
    Bool                        isObjectArray() const;
    Bool                        isSeparator() const;
};

//=================================================================================================
// Node. Conceptually a container of inputs & outputs.
//=================================================================================================

class DL_API Node DL_FINAL : public RefersTo<INode>
{
public:
    Node() = default;
    Node( INode* impl ) : RefersTo( impl ) { }

    // Get the name for this node. Currently, A node's name never changes after creation (unless
    // it is imported into another scene, after which it will again not change). The rule for node
    // names is that they must be valid C identifiers. This allows .bsa to be much cleaner, since
    // we do not have to quote every node/attr name in the file.
    // 
    // Note that since we must maintain unique names there can be some benefit, while writing an
    // exporter, to preemptively sequentially-numbering nodes, since though it is easy to find if
    // a name has been used, it can take much more work to find a unique permutation of that name.
    //
    // NOTE: it has caused confusion for this to be called "name" so we are moving to "id".
    //
    String                      id() const;
    String                      name() const; // Deprecated, use id().

    // Since node names are really IDs, and it may not always be most advantageous to use names
    // that are friendly to users (e.g. in Rhino we use object GUID in order to allow refer back
    // to objects in the source Rhino document if necessary), we also make use of the node["name"]
    // attribute to implement a display name. This also allows users to set their display names at
    // will, with no restrictions. If no display name has been set, this will return node.name().
    // 
    // In the specific case of nodes deriving from "path", if the node["name"] attr has not been
    // set, this will return the file name, if one has been set.
    //
    String                      displayName() const;

    // This really just sets the node["name"] attribute, if it exists.
    //
    Bool                        setDisplayName( String name );

    // Get the friendly typename for this node. This is set via .bnd.
    //
    String                      label( String lang = "auto" ) const;

    // Get the help text associated with this node. This is set via .bnd.
    //
    String                      help( String lang = "auto" ) const;

    // Get the layout for this node, if there is one. This is created via .bnd.
    //
    Layout                      layout() const;

    // Not typically useful, this is used internally in our object lifetime tests.
    //
    static Int                  instanceCount();

    // Get a hash for this node. This is a bit expensive, as it computes the hashes of any child
    // attrs. This is because it is typically used to check whether things are dirty and in need
    // of update. Excludes may be node type names, node names, input names and output names.
    //
    UInt                        hash( StringVector excludes = { } ) const;

    // Subscribe & unsubscribe observers to this node, to be notified of changes affecting it.
    //
    Bool                        subscribe( NodeObserver* observer );
    Bool                        unsubscribe( NodeObserver* observer );
    Bool                        unsubscribeAll();

    // Custom metadata may be added as inputs under this input.
    //
    Input                       meta() const;

    // Get the scene to which this node belongs. Nodes are always created and owned by a scene.
    // 
    // With referenced scenes, it becomes ambiguous which scene is meant, the node's parent scene,
    // or the scene that contains the reference. For instance, you may wish to get the world xform
    // of the parent scene, or you may wish to find a sibling node in the node's referenced scene.
    // 
    // Passing local=true will return the reference scene if it exists, else the top level parent
    // scene will be returned as usual.
    //
    Scene                       scene( Bool local = false ) const;

    // These are used internally, and currently are not called explicitly by any client code. If
    // you find a case where you think you need to call them, that may indicate a case that should
    // be better handled internally, so let us know the scenario, if you find one.
    //
    Bool                        dirty( Bool checkDeps = true ) const;
    void                        setDirty( Bool dirty = true );

    // This gets the node's concrete type.
    //
    String                      type() const;

    // This gets the base types of this node, if any.
    //
    StringVector                bases() const;

    // These will return true if the node's type, or the type of any of its bases, match the given
    // type or types.
    //
    Bool                        isTypeOf( String type ) const;
    Bool                        isTypeOf( StringVector types ) const;

    // This returns the node's type, along with its base types, like "type [base0, base1]".
    //
    String                      typeDesc() const;

    // Node definitions may inherit from multiple other definitions. When a definition is purely
    // intended to be used as a base, it is marked as abstract, and should not be exposed in GUI.
    //
    Bool                        isAbstract() const;

    // Nodes may be defined as internal, in which case they should not appear in user interfaces,
    // or be considered as being for public use, since they may be removed or altered at any time.
    //
    Bool                        isInternal() const;

    // This will attempt to convert this node to another type, carrying over any values that can
    // be matched by name and type.
    //
    Node                        convertTo( String type );

    // Search an input or output by path in this node. The path is allowed to omit the node's name,
    // and no attrs are created (as occurs with parseInputPath) in order to try to find it.
    //
    Input                       findInput( String path ) const;
    Output                      findOutput( String path ) const;

    // Find whether this node has inputs or outputs of a given type at a given name or index.
    //
    Bool                        hasInput( UInt index, AttrType type = AttrType_Null ) const;
    Bool                        hasInput( String name, AttrType type = AttrType_Null ) const;
    Bool                        hasOutput( UInt index, AttrType type = AttrType_Null ) const;
    Bool                        hasOutput( String name, AttrType type = AttrType_Null ) const;

    // Find how many inputs this node has, and retrieve them by index or name. The non-const
    // overloads taking a name will create a null input, which will take its type from its first
    // assignment. The others will return a null input (use appendElement() to add elements to
    // an array input).
    //
    UInt                        inputCount() const;
    Input                       input( UInt index );
    Input                       input( UInt index ) const;
    Input                       input( String name );
    Input                       input( String name ) const;
    InputVector                 inputs() const;

    // Get an input by indexing. Since it is fine for an input to be null, you can chain these
    // operators together for convenient use that would otherwise involve a lot of null checking
    // (and incRef/decRef calls), like: a["b"]["c"]["d"].asReal(). You can always check for null
    // if you like using the bang operator (implemented by RefersTo<T>). These use the input(name)
    // and input(index) methods internally, so have the same behavior regarding creation of ad hoc
    // inputs.
    //
    Input                       operator[]( UInt index );
    Input                       operator[]( UInt index ) const;
    Input                       operator[]( String name );
    Input                       operator[]( String name ) const;

    // Find how many outputs this node has, and retrive them by index or name. There is no indexing
    // operator for outputs because it is already used for inputs.
    //
    UInt                        outputCount() const;
    Output                      output( UInt index ) const;
    Output                      output( String name ) const;
    OutputVector                outputs() const;

    // Parse a path to locate a given input, using a path like "nodename.attr.arrayAttr[3].attr".
    // "path" may start with this node name, or one of this node's inputs, else a null input will
    // be returned. New array elements will be appended as necessary to return the specified path.
    // If invalid inputs are found in the path, a null input will be returned.
    //
    Input                       parseInputPath( String path );

    // These are for runtime definition of a node. Usually this is done by authoring a .bnd file,
    // but it can also be useful to create an ad hoc node definition in some cases. For instance,
    // in Rhino, we have a generic "attr" control, so it is useful to create some custom node types
    // for various bits of the UI, in order to make use of that generic control, even though we
    // don't want to add these definitions to Bella proper.
    //
    Input                       createInput( String name, AttrType type );
    Input                       createArray( String name, AttrType type );
    Output                      createOutput( String name, AttrType type );

    Bool                        removeInput( String name );
    Bool                        removeInput( UInt index );
    Bool                        removeLastInput();
    Bool                        clearInputs();

    Bool                        removeOutput( String name );
    Bool                        removeOutput( UInt index );
    Bool                        removeLastOutput();
    Bool                        clearOutputs();

    // Get all the dependencies of this node, optionally following output connections, and also
    // optionally sorting them topologically (i.e. by dependence).
    //
    NodeVector                  dependencies( Bool followOutputs = false, Bool topoSort = false ) const;

    // Serialize this node to bsa format, optionally including dependencies, and also optionally
    // writing default values. It can be useful to write the default values since if you later read
    // the serialized data into another node, its non-default values will not be reset, since they
    // will not appear in the bsa text.
    //
    String                      serialize( Bool withDeps = true, Bool writeDefaults = false ) const;

    // Create a clone of this node, optionally including its dependencies.
    //
    Node                        clone( Bool withDeps = true );

    // These are helpers for dealing with "path" based nodes, such as fileTexture. Paths are stored
    // separated into path["dir"], path["file"], and path["ext"], and these take care of getting or
    // setting those with a single call.
    //
    Bool                        isPath() const;
    String                      asPath() const;
    Bool                        setPath( String dir, String file, String ext );
    Bool                        setPath( String path );

    // For use with xform nodes, retrieve xform["children"], optionally iterating those children to
    // depth N.
    //
    NodeVector                  children( Int depth = -1 ) const;

    // Find the index of this node in parent["children"]. Return values indicate:
    //
    //  -6: parent is this node.
    //  -5: parent is null.
    //  -4: parent has no "children" input.
    //  -3: parent["children"] is not an array.
    //  -2: parent["children"] is not of type node[].
    //  -1: This node was not found in node[] input parent["children"].
    //
    Int                         indexIn( Node parent ) const;

    // Add this node to parent["children"], if it is not already included.
    //
    Bool                        parentTo( Node parent );

    // Remove this node from parent["children"], if it is currently included.
    //
    Bool                        unparentFrom( Node parent );

    // Get all paths starting at this node, optionally filtering by type and visible status.
    //
    PathVector                  paths( String type = "", Bool excludeHidden = false ) const;
    PathVector                  paths( StringVector types, Bool excludeHidden = false ) const;

    // Get all paths from this node to another, optionally excluding hidden nodes.
    //
    PathVector                  pathsTo( Node node, Bool excludeHidden = false ) const;
    PathVector                  pathsTo( String name, Bool excludeHidden = false ) const;

    // Find the first path from this node to another, optionally excluding hidden nodes.
    //
    Path                        firstPathTo( Node node, Bool excludeHidden = false ) const;
    Path                        firstPathTo( String name, Bool excludeHidden = false ) const;

    // Get the path to the .bnd file in which this node is defined. The default Bella nodes are
    // defined using an embedded .bnd, so the path will not exist on disk.
    //
    String                      nodeDefPath() const;

    // Check whether this node has library implementations.
    //
    Bool                        hasNodeInit() const;
    Bool                        hasNodePrep() const;
    Bool                        hasNodeEval() const;
    Bool                        hasNodeModify() const;
    Bool                        hasNodeFree() const;

    // Call this node's library implementations. NOTE that these return true when the called
    // function is not implemented; if you need to know whether a call has truly failed, you must
    // use the hasNodePrep/Modify methods above. Mostly these are called by the scene or engine,
    // though you may need to call nodePrep e.g. prior to baking a texture, to allow the node to
    // set up any state it may need to use in nodeEval.
    //
    Bool                        nodePrep() const;
    Bool                        nodeModify( Node target = Node() );

    // This is used by node library implementations, to attach arbitrary data to an instance of a
    // node, for use in evaluation. You should not attempt to use it from client code.
    //
    const void*                 data() const;

    // Each node can have an svg icon embedded.
    //
    String                      svgIcon( Bool returnEmptyStringIfMissing = false ) const;

    // Each node can provide html help generated from its bnd definition.
    //
    String                      htmlHelp( Bool asStandaloneDoc = false, String lang = "auto" ) const;

    // Function for checking the state of (mesh) nodes. If this returns the empty string, there is
    // no issue detected with the mesh, otherwise the returned string describes the problem.
    //
    String                      checkMesh() const;

    // Get a point cloud representation for this node. pointCloudCount() returns the number of
    // points in the cloud, if it already exists. When it exists, it points to a Pos3f buffer that
    // is owned & managed by the node. If the node type is mesh, the point cloud will be based on
    // the mesh points, in their local space; if the node is a type of xform, it will be based on
    // all visible meshes parented to it, directly or indirectly.
    //
    UInt                        pointCloudCount() const;
    BufRef                      pointCloud( Real detail = 1.0, Int maxPoints = -1 );
};

template<> Node Input::defaultVal<Node>( Node def ) const;

//=================================================================================================
// Path.
// 
// Since the only restriction on parenting of xforms is to avoid creating cycles, it is possible
// for a given xform to appear at any number of places in the world hierarchy. Each way of reaching
// a given node in the tree is referred to as a Path, represented by an instance of this class.
//=================================================================================================

class DL_API Path DL_FINAL : public RefersTo<IPath>
{
public:
    Path() = default;
    Path( IPath* impl ) : RefersTo( impl ) { }

    // This is a string representation of the path, like "world|xform0|xform1|mesh". Hash is just
    // the hash of path(), and dirty() is used internally.
    //
    String                      path() const;
    UInt64                      hash() const;
    Bool                        dirty() const;

    // Get the nodes included in this path.
    //
    UInt                        count() const;
    Bool                        contains( Node node ) const;
    NodeVector                  nodes() const;

    // Root is the furthest-up xform in this path. In most usages it will be the world. Leaf is the
    // furthest node in the path, and may be an xform or geometry. For convenience, you may reach
    // the Nth parent using the revIdx argument to parent().
    //
    Node                        root() const;
    Node                        leaf() const;
    Node                        parent( UInt revIdx = 0 ) const;

    // Gets the final transform for this path's leaf, at a given time (always 0.0 for now), and
    // referring by index to an element of an instancer, in the case the leaf is being located by
    // an instancer.
    //
    Mat4                        transform( Real time = 0.0, UInt instancerIdx = ( UInt )-1 ) const;

    // These account for Bella material & visibility inheritance rules, returning the effective
    // values for this specific path.
    //
    Bool                        visible() const;
    Node                        material() const;
    Node                        subdivision() const;
};

//=================================================================================================
// NodeObserver.
// 
// Inherit this and subscribe it to a node to be notified of changes to that node.
//=================================================================================================

struct DL_API NodeObserver
{
    virtual void                onRemoved()                             { }
    virtual void                onRenamed( String prev )                { DL_UNUSED( prev ); }
    virtual void                onConverted( Node to )                  { DL_UNUSED( to ); }

    virtual void                onInputChanged( Input input )           { DL_UNUSED( input ); }
    virtual void                onInputConnected( Input input )         { DL_UNUSED( input ); }

    virtual ~NodeObserver() = default;
};

//=================================================================================================
// SceneObserver.
// 
// Inherit this and subscribe to a scene to be notified of changes to the scene.
//=================================================================================================

struct DL_API SceneObserver
{
    virtual void                onNodeAdded( Node node )                { DL_UNUSED( node ); }
    virtual void                onNodeRemoved( Node node )              { DL_UNUSED( node ); }
    virtual void                onNodeRenamed( Node node, String prev ) { DL_UNUSED( node ); DL_UNUSED( prev ); }
    virtual void                onNodeConverted( Node from, Node to )   { DL_UNUSED( from ); DL_UNUSED( to ); }

    virtual void                onInputAdded( Input input )             { DL_UNUSED( input ); }
    virtual void                onInputRemoved( Input input )           { DL_UNUSED( input ); }

    virtual void                onInputChanged( Input input )           { DL_UNUSED( input ); }
    virtual void                onInputConnected( Input input )         { DL_UNUSED( input ); }

    virtual void                onBeginEventGroup()                     { }
    virtual void                onEndEventGroup()                       { }

    virtual void                onBeginUndoGroup()                      { }
    virtual void                onEndUndoGroup()                        { }

    virtual ~SceneObserver() = default;
};

//=================================================================================================
// Scene.
// 
// Scene holds a list of nodes that may reference, or be connected to each other through their
// inputs & outputs, forming a dependency graph.
// 
// One of these nodes, an xform, is designated as the "world" and becomes the root of a hierarchy
// of xforms and other nodes, forming a directed acyclic graph. It is called the world because
// everything visible in a rendered Bella scene is ultimately parented to this xform.
// 
// Any xform may be parented to any other xform, as long as no cycles are produced (and the SDK
// input-assignment code will prevent this occurring if you try).
// 
// The world xform is also used to determine the global scale & orientation of geometry in the
// scene; rather than have plugins physically transform geometry while writing Bella files, the
// geometry is transferred verbatim, and then scaled & oriented using the world xform.
// 
// Scenes may easily be written to, and read from files, using the Bella .bsa (text-based), .bsx
// (binary, compressed), or .bsz (a .bsx zipped along with external resources like textures) file.
// 
// When a scene is being rendered, there is no need to do any special thread synchronization, as
// this will be handled internally by the render engine. It is however possible to pause events
// temporarily, in order to make bulk changes without the engine processing them all individually.
// 
// This is accomplished by the engine subscribing to the scene as a SceneObserver, queueing changes
// as they are made, and committing those changes to the running solver. The same mechanism, the
// SceneObserver, is also available for you to use, to implement an interactive GUI.
// 
// A specially-treated type of node in the scene is referred to as a "node definition"; these are
// held in the scene's "defs" list. In actuality these are nodes like any other, just that they are
// used as templates for creating new node instances in the scene.
// 
// Though it is possible to create node definitions in code, this is usually done by authoring a
// .bnd (Bella Node Definitions) file and loading that file with the scene's loadDefs method.
// Calling loadDefs() with no argument will load the default Bella node definitions.
//=================================================================================================

struct DL_API SceneProgress
{
    virtual void onProgress( String status, Real progress ) { DL_UNUSED( status ); DL_UNUSED( progress ); }
    virtual ~SceneProgress() = default;
};

class DL_API Scene DL_FINAL : public RefersTo<IScene>
{
public:
    Scene();
    Scene( IScene* impl ) : RefersTo( impl ) { }

    // Not typically useful, this is used internally in our object lifetime tests.
    //
    static Int                  instanceCount();

    // This is just a number that is incremented for each scene created.
    //
    Int64                       id() const;

    // This is quite expensive, as it generates a hash based on nearly all nodes in the scene. It
    // is mostly used at this point to verify that a .bsi file is valid to resume from a given
    // scene.
    //
    UInt64                      hash( StringVector excludes = { } ) const;

    // The bsx file version for this file. This is set to the current bsx file version in the ctor
    // and otherwise read from a file. fileTime & fileComments are not currently used.
    //
    UInt64                      fileVersion() const;
    String                      fileTime() const;
    StringVector                fileComments() const;

    // Subscribe to this scene, to be notified of changes made to it. The render engine uses this
    // same approach, so it is capable and well-tested.
    //
    Bool                        subscribe( SceneObserver* observer );
    Bool                        unsubscribe( SceneObserver* observer );
    Bool                        unsubscribeAll();

    // Get definitions, optionally by category. Categories are a list of strings made up of various
    // node base types, along with the catch-all "misc.". They are useful for categorizing any list
    // of nodes you may have, for a menu, or whatever else.
    //
    NodeVector                  defs( String category = "" ) const;
    UInt                        defCount() const;
    StringVector                defCategories() const;

    // Clear all definitions, parse them from a .bnd string, and load them (or the default Bella
    // definitions if path is empty) from a .bnd file .
    //
    UInt                        clearDefs();
    Bool                        parseDefs( String json );
    Bool                        loadDefs( String path = "" );

    // Define a new node. This just creates a new node that lives in the defs list. Definitions
    // must have a valid C identifier type name, and there must not be an existing definition by
    // the same name, case-insensitive.
    //
    Node                        defineNode( String name );
    Node                        findDef( String type ) const;
    Node                        findDef( UInt index ) const;

    // Create a node. If no name is given, type will be used to generate a name. If no displayName
    // is given, the node will return name() when asked for displayName().
    //
    Node                        createNode( String type, String name = "", String displayName = "" );

    // Clone nodes, by reference or by name. Clone just the given node, or along with all of its
    // dependencies. If adjustDisplayNames is true, display names will have incremental numbering
    // added, since cloning many nodes can yield a confusing result, if they all appear to share
    // the same names -- however, this can be fairly expensive, so you may wish to forgo it in some
    // cases.
    //
    Node                        cloneNode( Node node, Bool withDeps = true, Bool adjustDisplayNames = true );
    Node                        cloneNode( String name, Bool withDeps = true, Bool adjustDisplayNames = true );
    NodeVector                  cloneNodes( StringVector names, Bool withDeps = true, Bool adjustDisplayNames = true );

    // Convert a node to another type. During the process, we will attempt to bring over any values
    // for inputs whose names match.
    //
    Node                        convertNode( Node node, String newType );

    // Remove nodes from the scene. Some nodes cannot be removed, including the current global,
    // state, world, settings, environment, beautyPass, defaultMaterial, camera, lens, and sensor.
    //
    Bool                        canRemoveNode( Node node ) const;
    Bool                        removeNode( Node node );
    Bool                        removeNode( String name );
    StringVector                removeNodes( String ofType );
    StringVector                removeNodes( NodeVector nodes );
    StringVector                removeNodes( StringVector names );
    UInt                        clearNodes( Bool unreferencedOnly = false );
    UInt                        nodeCount() const;

    // Nodes are not really deleted, they rather have their references & attrs cleared, and are
    // then put into a special list, allowing them to be restored later. The undo system also
    // makes use of this. When working with an interactive renderer, it can therefore be a good ida
    // to try to restore a given node before recreating it.
    //
    Node                        restoreNode( String name );


    // Find a node or nodes by name, optionally sorting topologically with or without dependencies.
    //
    Node                        findNode( String name ) const;
    NodeVector                  findNodes( StringVector names, Bool topoSort = false, Bool withDeps = false ) const;

    // If path starts with a valid node name, find the input indicated by path. New array elements
    // will be appended to arrays as necessary to return the specified path. If invalid inputs are
    // found in the path, a null Input will be returned.
    //
    Input                       parseInputPath( String path );

    // Get lists of all node types, and all node names, optionally filtering by type.
    //
    StringVector                nodeTypes() const;
    StringVector                nodeNames( String type = "" ) const;
    StringVector                nodeNames( StringVector types ) const;

    // Get a list of all nodes, optionally sorting by type, optionally topologically sorted.
    //
    NodeVector                  nodes( String type = "", Bool topoSort = false ) const;
    NodeVector                  nodes( StringVector types, Bool topoSort = false ) const;

    // Get a list of paths in the scene, optionally filtering by type and visibility.
    //
    PathVector                  paths( String type = "", Bool excludeHidden = false ) const;
    PathVector                  paths( StringVector types, Bool excludeHidden = false ) const;

    // Get a list of paths to a given node, optionally excluding hidden paths.
    //
    PathVector                  pathsTo( Node node, Bool excludeHidden = false ) const;
    PathVector                  pathsTo( String name, Bool excludeHidden = false ) const;

    // Get the first path to a given node, optionally excluding hidden paths.
    //
    Path                        firstPathTo( Node node, Bool excludeHidden = false ) const;
    Path                        firstPathTo( String name, Bool excludeHidden = false ) const;

    // Get the global, state, world, and settings nodes, optionally creating them if they do not
    // yet exist.
    //
    Node                        global( Bool create = true ) const;
    Node                        state( Bool create = true ) const;
    Node                        world( Bool create = true ) const;
    Node                        settings( Bool create = true ) const;

    // ocioConfig() and colorSpace() return settings["ocioConfig"] and settings["colorSpace"] if
    // there is a settings node. applyOcioConfig() attempts to load OCIO using the ocioConfig()
    // value, which may be an env var, or a path to an OCIO config file; in addition to loading
    // the OCIO config, it will also update enum values of related (colorSpace) inputs in the
    // scene.
    //
    String                      ocioConfig() const;
    String                      applyOcioConfig() const;
    String                      colorSpace() const;

    // Get the current camera and its path. This will be the first path to the camera specified
    // at settings:camera, if the settings node exists.
    //
    Node                        camera() const;
    Path                        cameraPath() const;

    // Get current environment, sun, and ground plane, as specified in settings, if the settings
    // node exists. solarContext is a base of the sun and skyDome nodes, which deals with date,
    // time, location, and sun direction.
    //
    Node                        environment() const;
    Node                        sun() const;
    Path                        sunPath() const;
    Node                        solarContext() const;
    Node                        groundPlane() const;

    // Get the transformation and scale components of the world xform.
    //
    Mat4                        axisSystem() const;
    Vec3                        scale() const;

    // Get the beauty pass, and other passes, if they exist. beautyPass() returns the node at
    // settings:beautyPass (beautyPasses() returns all beautyPasses).
    //
    Node                        beautyPass() const;
    NodeVector                  beautyPasses() const;
    NodeVector                  extraPasses() const;
    NodeVector                  allPasses() const;

    // Get the default & override materials, if they are set (on settings).
    //
    Node                        defaultMaterial() const;
    Node                        overrideMaterial() const;

    // Parse bsa text into the scene. This may create nodes, and overwrite values in nodes already
    // found to exist.
    //
    Bool                        parse( String text );

    // Read a bsa/bsx/bsz (or other supported type of) file, optionally as a fragment. When
    // asFragment=false, the scene will be cleared before reading the file. When asFragment=true,
    // the scene will not be cleared, and the reading of the file may produce new nodes, or
    // overwrite values on ones already found to exist.
    // 
    // This also supports various other file formats; as of this writing, usd/usda/usdc/usdz/abc
    // through Pixar OpenUSD, and 3ds/blend/dae/fbx/obj/stl through Open Asset Import Library. The
    // latter group may work better or worse depending on file type and version (typically OBJ is
    // the best-supported type, as others may depend on file version). The node returned will be
    // parented to the world if asFragment=false, and its transform compensated by the current
    // world transform. When asFragment=true the node will not be added to the world, and you will
    // need to decide whether to do that, and/or whether to compensate its transform by the
    // inverse of the current scene's world xform.
    //
    // supportedFileTypes() returns the list of file extensions (with preceding dot) that can be
    // read by this scene, and canRead(path) returns whether the given path has a supported file
    // extension and exists.
    //
    StringVector                supportedFileTypes() const;
    Bool                        canRead( String path ) const;
    Node                        read( String path, Bool asFragment = false, SceneProgress* progress = nullptr );

    // Import a bsa/bsx/bsz (or other supported type of) file, or a bsa text fragment. If a file,
    // and it has a world node, its display name will be set to the filename, and the node will be
    // returned. If there's no world node (e.g. importing a material) a null node will be returned.
    // 
    // Note that in the case of a world node, it will not have been added to the hierarchy of the
    // current scene, so you will need to choose whether to do that, and whether to compensate its
    // transform by the inverse of the current scene's world xform.
    //
    // As with the read() method, this also supports usd/usda/usdc/usdz through Pixar OpenUSD and
    // 3ds/blend/dae/fbx/obj/stl through Open Asset Import Library, with the same disclaimers. As
    // with the clone methods, adjustDisplayNames controls whether display names get incrememtal
    // numerical suffixes added.
    //
    Node                        import( String pathOrFragment, SceneProgress* progress = nullptr, Bool adjustDisplayNames = true );

    // Write the scene to a bsa/bsx/bsz file. When saving as bsz, all referenced inputPath nodes in
    // the scene will have their files zipped into the bsz, and will have their "dir" attributes
    // cleared. We only include referenced resources by default because the main purpose of bsz is
    // to allow sending scenes to other machines or render farms for rendering. If you wish include
    // all resources, set scene.global()["writeMinimalBsz"] to false before calling write.
    //
    Bool                        write( String path, SceneProgress* progress = nullptr );

    // Clone the entire scene.
    //
    Scene                       clone() const;

    // This will replace any occurrences of special macros with their values. Currently that is
    // only the strings $cam and $camera, which are replaced with the current camera's name.
    //
    String                      applyMacros( String path ) const;

    // This applies macros to the current file path, if there is one. When opening a file, we set
    // the "location" to the path from which it was opened; this is then used for various purposes,
    // such as resolving texture paths, and determining whether saving is a "save as" or not.
    //
    String                      location( Bool applyMacros = true ) const;
    String                      setLocation( String path );

    // Get the current output name, which will be used for any file outputs that do not use their
    // own override. This is the value of settings["outputName"] if settings exists. If applyMacros
    // is true, then macros like $cam and $camera (actually the only supported macros at this time)
    // appearing in the name will be replaced with their corresponding scene-specific values.
    //
    String                      outputName( Bool applyMacros = true ) const;

    // Get the current output directory. This is the value of settings["outputDir"] if settings
    // exists, and if the given directory exists. If not, and useFallbacks is true, then location()
    // will be used, and if that also does not exist, then the $HOME/bella/output directory will be
    // used.
    //
    String                      outputDir( Bool useFallbacks = true ) const;

    // Get the output path for a given pass. This depends on various factors like the current
    // settings output name & dir, as well as the optional path override set in the pass.
    //
    String                      outputPath( Node pass ) const;

    // Get all dependencies reachable from the current state node, which is the node that sets
    // the current settings & world xform. Optionally, filter by a list of types, and sort the
    // list topologically.
    //
    NodeVector                  stateDependencies( StringVector types = { }, Bool topoSort = false ) const;

    // Get a list of all nodes currently unreferenced by the current state, optionally filtering
    // by a list of node types.
    //
    NodeVector                  unreferencedNodes( StringVector types = { } ) const;

    // Resolve an input path, or all input paths, optionally updating the data in the node.
    //
    String                      resolveInputPath( Node inputPath, Bool updateNode = true );
    Bool                        resolveInputPaths( Bool updateNodes = true );

    // Get the effective resolution for a camera, given IPR/production & license status.
    //
    static Vec2u                resolution( Node camera, Bool forIPR, Bool isLicensed );

    // Get the effective region for a camera, given IPR/production & license status, optionally
    // constraining the region to the effective camera resolution.
    //
    static Region               region( Node camera, Bool forIPR, Bool isLicensed, Bool constrain = true );

    // Compute the number of actual threads, given a desired number, positive or negative. Note
    // that userThreads may be negative, which means (max_threads - userThreads).
    //
    static UInt                 computeNumThreads( Int userThreads = 0 );

    // Get the number of threads set in settings (if settings exists), for IPR or production.
    //
    UInt                        numThreads( Bool forIPR = false ) const;

    // bsi file path computation. The scene manages a temp bsi file that holds rendering data when
    // the scene has unsaved changes. Upon saving the scene, this temp file is renamed or copied to
    // make it the official bsi counterpart to the newly-saved (or saved-as) scene file. 
    //
    String                      bsiPath( Node beautyPass ) const;
    String                      bsiTempPath( Node beautyPass ) const;
    Bool                        canResumeBsi( Node beautyPass ) const;

    // The scene has various svg files embedded, to use for Bella nodes and other things.
    //
    static String               findSvg( String name );

    // Generate html help for the entire scene, optinoally writing it to file, and opening that
    // file in the default application for html. The file is written to the bella home dir.
    //
    String                      htmlHelp( String lang = "auto" ) const;
    String                      writeHtmlHelp( String& status, String lang = "auto" );
    Bool                        showHtmlHelp( String& status, String lang = "auto" );

    // The scene has built-in undo. If enabled, all you need to do is provide GUI to show whether
    // any undos or redos are currently available, and to call undo/redo. If you enable undo, you
    // should look into use of the UndoScope class declared below, which is used to group actions
    // together for undo/redo.
    //
    void                        enableUndo();
    void                        disableUndo();
    Bool                        undoEnabled() const;

    Int                         undoLimit() const;
    void                        setUndoLimit( Int mb = -1 );
    void                        clearUndo();

    Bool                        canUndo() const;
    Bool                        canRedo() const;

    Bool                        undo( UInt num = 1 );
    Bool                        redo( UInt num = 1 );

    StringVector                undoList() const;
    StringVector                redoList() const;

    // Undo groups are used to batch several actions together into one undo record. Observers will
    // have their onBeginUndoGroup methods called when the initial group is pushed, and will also
    // have their onEndUndoGroup methods called when the final group is popped. Undo grouping is
    // implemented as a conceptual stack, so that you do not need to try to track undo group state
    // yourself -- you only need to ensure that calls to push and pop are balanced, and to help
    // with this, you may use the UndoScope class found below.
    //
    void                        pushUndoGroup();
    void                        popUndoGroup( String undoDesc = "", String redoDesc = "" );
    Bool                        inUndoGroup() const;

    struct DL_API UndoScope DL_FINAL : public RefersTo<RefCounted>
    {
        UndoScope( Scene scene, String undoDesc = "", String redoDesc = "" );
    };

    // By default, any observers are notified of changes made to the scene. However, in some cases it
    // is desirable that notifications should not be made. Disable/enable of events is implemente as
    // a conceptual stack, so that you need not try and manage disabled state yourself -- it is only
    // necessary to ensure that calls to enable/disable are balanced, and to help with this, you may
    // use the DisableEvents class found below.
    //
    void                        enableEvents();
    void                        disableEvents();
    Bool                        eventsEnabled() const;

    struct DL_API DisableEvents DL_FINAL : public RefersTo<RefCounted>
    {
        DisableEvents( Scene scene );
    };

    // Event groups are used to group sets of actions, such that observers may pause processing of
    // changes, while the group is in effect. Observers recieve the onBeginEventGroup notification
    // when the initial group is pushed, and the onEndEventGroup notification when the final event
    // group is popped. Event groups are implemented as a conceptual stack, so that you do not need
    // to try to track group state yourself -- it is only necessary to make sure that calls to push
    // and pop are balanced, and to help with this, you may use the EventScope class found below.
    //
    // NOTE: this can be particularly important for setting groups of related attributes, such as
    // mesh buffers, which would cause issues if given to the render engine in an invalid state. By
    // putting such changes into an event group, we ensure that the engine will only update using
    // a complete and valid mesh.
    //
    void                        pushEventGroup();
    void                        popEventGroup();
    Bool                        inEventGroup() const;

    struct DL_API EventScope DL_FINAL : public RefersTo<RefCounted>
    {
        EventScope( Scene scene );
    };

    // Get paths for all inputPaths in the scene, optionally excluding missing paths.
    //
    StringVector                allResources( Bool includeMissing = true );

    // Get paths for all inputPaths referenced by the current state, optionally including missing
    // paths.
    //
    StringVector                referencedResources( Bool includeMissing = false );

    // Get missing paths for all inputPaths, or only for those referenced by the current state.
    //
    StringVector                missingResources( Bool referencedOnly = true );

    // Copy all referenced resources to a /res dir next to the current scene.
    //
    Bool                        gatherResources( Bool referencedOnly = true );

    // This is potentially expensive as it traverses the state dependencies and has them all check
    // for dirty state. However, it is far less expensive than doing that yourself, since the value
    // for each is cached and can be re-used internally until a change has been made. That said,
    // we do not know of any case where an SDK user should have to think about this, and you should
    // let us know if you think you do.
    //
    Bool                        isDirty() const;
};

//=================================================================================================
// Vectors.
// 
// These are refcounted typed vectors used to return lists of inputs, outputs, nodes, and paths,
// where the underlying API returns iterators.
//=================================================================================================

DL_DECL_REF_VECTOR( Input );
DL_DECL_REF_VECTOR( Output );
DL_DECL_REF_VECTOR( Node );
DL_DECL_REF_VECTOR( Layout );
DL_DECL_REF_VECTOR( EnumItem );
DL_DECL_REF_VECTOR( Path );

//=================================================================================================
// PbrTextureSet.
// 
// This class is used to encapsulate a set of PBR textures, and provide methods for easily working
// with them.
//=================================================================================================

class DL_API PbrTextureSet DL_FINAL : public RefersTo<IPbrTextureSet>
{
public:
    PbrTextureSet();
    PbrTextureSet( IPbrTextureSet* impl ) : RefersTo( impl ) { }

    // Call resolve with a path to one texture of a PBR set, or to a .zip containing a PBR set, and
    // PbrTextureSet will analyze the related textures and set its paths accordingly. For .zip it
    // will unzip to the temp directory.
    //
    UInt                        resolve( String path, Node pbr = Node() );
    String                      summary() const;

    // The class has a default set of matches per texture type ("opacity", "color", etc). Usually
    // you do not need to do anything with this, but it is possible to add/remove matches.
    //
    void                        addMatch( String type, String match );
    void                        setMatches( String type, StringVector matches );
    StringVector                getMatches( String type ) const;

    // Get the original input path, or all resolved paths.
    //
    String                      inputPath() const;
    StringVector                resolvedPaths() const;
    UInt                        numResolvedPaths() const;

    // Apply this set to a PBR or uber material.
    //
    Bool                        applyTo( Node node ) const;

    // Get the path for each channel.
    //
    String                      opacity() const;
    String                      color() const;
    String                      metallic() const;
    String                      roughness() const;
    String                      specular() const;
    String                      glossiness() const;
    String                      aniso() const;
    String                      anisoRotation() const;
    String                      bump() const;
    String                      normal() const;
    String                      displacement() const;
    String                      emission() const;
    String                      scattering() const;
    String                      ao() const;

    // Find out whether we have a path for a given channel.
    //
    Bool                        hasOpacity() const;
    Bool                        hasColor() const;
    Bool                        hasMetallic() const;
    Bool                        hasRoughness() const;
    Bool                        hasSpecular() const;
    Bool                        hasGlossiness() const;
    Bool                        hasAniso() const;
    Bool                        hasAnisoRotation() const;
    Bool                        hasBump() const;
    Bool                        hasNormal() const;
    Bool                        hasDisplacement() const;
    Bool                        hasEmission() const;
    Bool                        hasScattering() const;
    Bool                        hasAO() const;
};

//=================================================================================================
// Image.
// 
// This image is used to pass around images from the engine. It does not own the image memory.
//=================================================================================================

class DL_API Image DL_FINAL : public RefersTo<IRefCounted>
{
public:
    Image();
    Image( IRefCounted* impl ) : RefersTo( impl ) { }
    Image( UInt width, UInt height, Rgba8* rgba8, Rgbf* rgbf );
    Image( UInt width, UInt height, Region region, Rgba8* rgba8, Rgbf* rgbf );

    // When isValid() returns true, it is guaranteed that width & height are non-zero, and that the
    // buffers are not null.
    //
    Bool                        isValid() const;
    UInt                        width() const;
    UInt                        height() const;

    // When rendering a region, this will indicate the size and offset of the rendered pixels,
    // within the overall image rectangle.
    //
    Region                      region() const;

    // When isValid() returns true, rgba() and rgbf() are guaranteed to be non-null. Image does not
    // own these buffers, it is just used for passing them around, usually along with a pass name.
    // Typically, rgba8() will have had the current display color space applied, while rgbf() will
    // remain linear.
    //
    Rgba8*                      rgba8() const;
    Rgbf*                       rgbf() const;
};

//=================================================================================================
// ColorConfig.
// 
// This class is used to load & use OCIO configs.
//=================================================================================================

class DL_API ColorConfig DL_FINAL
{
public:
    // Load an OCIO config. If pathOrEnvVar is empty, will look for $HOME/bella/ocio.txt, and will
    // otherwise check the OCIO env var. If pathOrEnvVar is not empty it will be treated as a path
    // to an OCIO config, or an env var containing such a path. If none of these yields an OCIO
    // config path, we will use our built-in linear, sRGB and Rec709 spaces.
    //
    static String               loadOcioConfig( String pathOrEnvVar = "" );

    // OCIO state.
    //
    static StringVector         colorSpaces();
    static String               parseColorSpace( String fileName, Bool returnBestGuess = false );
    static String               findColorSpace( String candidate );

    // Color conversion.
    //
    static Rgba                 transform( Rgba color, String srcSpace, String dstSpace );
    static Rgba                 colorSpaceToLinear( Rgba color, String srcSpace = "sRGB" );
    static Rgba                 linearToColorSpace( Rgba color, String dstSpace = "sRGB" );

    // Image conversion. NOTE: transform is applied to Image::rgba(), and not rgbf().
    //
    static Bool                 transform( Image image, String srcSpace, String dstSpace );
    static Bool                 colorSpaceToLinear( Image image, String srcSpace = "sRGB" );
    static Bool                 linearToColorSpace( Image image, String dstSpace = "sRGB" );
};

//=================================================================================================
// Utilities. These require linking the bella_sdk library.
//=================================================================================================

// Bella adopts Maya's convention for fitting the film gate to the resolution gate. Conversion to
// and from string is given since the enum values for Bella's sensor are "horizontal", "vertical",
// "fill" and "overscan".
//
enum FilmFit
{
    FilmFit_Horizontal, // Fit film gate to match resolution in the horizontal direction.
    FilmFit_Vertical,   // Fit film gate to match resolution in the vertical direction.
    FilmFit_Fill,       // Use horizontal or vertical fit, whichever is larger.
    FilmFit_Overscan    // Use horizontal or vertical fit, whichever is smaller.
};
DL_API FilmFit                  filmFitFromString( String str );
DL_API String                   filmFitToString( FilmFit ff );
DL_API Vec2                     fitFilmToResolution( FilmFit filmFit, Vec2 filmSize, Vec2 resolution, Real pixelAspect );
DL_API Bool                     isFilmFitHorizontal( FilmFit filmFit, Vec2 filmSize, Vec2 resolution, Real pixelAspect );

// Compute the intersection defined by a size, and a positive or negative offset.
//
DL_API dl::Region               constrainRegion( Vec2u size, Vec2i offset, Vec2u bounds );
DL_API dl::Region               constrainRegion( Vec2 size, Vec2 offset, Vec2 bounds );

// Compute reduced resolution used when not licensed (max area of 1080p, 1920x1080).
//
DL_API Vec2u                    renderedResolution( Vec2u res, Bool isLicensed );

// Camera navigation; delta should be small, usually from something like mouse movement. These
// make it trivial to implement rudimentary navigation in an IPR window.
//
DL_API Mat4                     panCamera( Path cameraPath, Vec2 delta, Bool isLicensed );
DL_API Mat4                     orbitCamera( Path cameraPath, Vec2 delta );
DL_API Mat4                     zoomCamera( Path cameraPath, Vec2 delta, Bool updateFocus = true );
DL_API Mat4                     zoomExtents( Path cameraPath, Vec3 target, Real radius );

// Date, time, sun angles & direction.
//
DL_API UInt                     parseMonth( String month );
DL_API Real                     computeTimeOfDay( Real hour, Real minute, Real second, Real utcOffset, Bool useDst );
DL_API UInt                     computeDayOfYear( Int year, Int month, Int day );
DL_API UInt                     computeJulianDay( Int year, Int month, Int day );
DL_API Real                     computeJulianDate( Int year, Int month, Int day, Int hour, Int minute, Real second, Real utcOffset, Bool useDst );
DL_API Vec2                     computeSunAngles( Real julianDate, Real latitude, Real longitude );
DL_API Vec2                     computeSunAngles( Node dateTime, Node location );
DL_API Mat3                     computeSunDirection( Real julianDate, Real latitude, Real longitude );
DL_API Mat3                     computeSunDirection( Node dateTime, Node location );

// Compute oriented bounding box for a set of points. Two algorithms are used, depending on the
// value given for maxPointsForPreciseBox. The (much) more expensive one is used when there are
// fewer than maxPointsForPreciseBox points. This is fine, since the more complex the mesh, the
// less we generally care about getting a precise fit (e.g. we want a precise fit if we are going
// to replace the mesh with an analytical box, accurately sized & positioned).
//
DL_API Obb                      computeObb( BufRef points, Int maxPointsForPreciseFit = 128 );

// Get the path to the standard preview scene, which is embedded in the sdk lib, and will be
// written to the $HOME/bella/preview directory.
//
DL_API String                   previewPath();

} // bella_sdk
} // dl

//=================================================================================================
// Misc.
//=================================================================================================

namespace dl {
namespace ds {

// Explicit instantiation for these pointer types to prevent implicit construction below.
//
template<> inline UInt64        hash( dl::bella_sdk::interfaces::IEnumItem* x ) { return ( UInt64 )x; }
template<> inline UInt64        hash( dl::bella_sdk::interfaces::IInput* x )    { return ( UInt64 )x; }
template<> inline UInt64        hash( dl::bella_sdk::interfaces::IOutput* x )   { return ( UInt64 )x; }
template<> inline UInt64        hash( dl::bella_sdk::interfaces::ILayout* x )   { return ( UInt64 )x; }
template<> inline UInt64        hash( dl::bella_sdk::interfaces::INode* x )     { return ( UInt64 )x; }
template<> inline UInt64        hash( dl::bella_sdk::interfaces::IPath* x )     { return ( UInt64 )x; }
template<> inline UInt64        hash( dl::bella_sdk::interfaces::IScene* x )    { return ( UInt64 )x; }

// To allow use of these types as keys in ds::HashMap.
//
template<> inline UInt64        hash( dl::bella_sdk::EnumItem x )   { return hash( x.impl() ); }
template<> inline UInt64        hash( dl::bella_sdk::Input x )      { return hash( x.impl() ); }
template<> inline UInt64        hash( dl::bella_sdk::Output x )     { return hash( x.impl() ); }
template<> inline UInt64        hash( dl::bella_sdk::Layout x )     { return hash( x.impl() ); }
template<> inline UInt64        hash( dl::bella_sdk::Node x )       { return hash( x.impl() ); }
template<> inline UInt64        hash( dl::bella_sdk::Path x )       { return hash( x.impl() ); }
template<> inline UInt64        hash( dl::bella_sdk::Scene x )      { return hash( x.impl() ); }

} // ds
} // dl

#endif // BELLA_SDK_SCENE_H
