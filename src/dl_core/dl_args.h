// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_ARGS_H
#define DL_CORE_ARGS_H

#include "dl_platform.h"
#include "dl_string.h"
#include "dl_fs.h"

namespace dl {

// Args is a class designed to accept arguments from main or WinMain, parse them, and provide
// methods of creating a collection of command-line switches that are then queried to find whatt
// was passed.
// 
// This will not be used directly, and will rather be given to you when you use the dl_main.inl
// file to implement your main. Upon entering main, you will add switches to the Args, and then
// use them to retrieve information. It may look something like this:
// 
//     #include "dl_core/dl_main.inl"
//     int DL_main( Args& args )
//     {
//         args.add( "o", "one", "", "Argument one." );
//         args.add( "t", "two", "no", "Argument two, accepts 'yes' or 'no'." );
//     
//         if ( args.versionReqested() )
//         {
//             printf( "%s", myAppVersion() ); // Print with no newline and bail.
//             return 0;
//         }
//     
//         if ( args.helpRequested() )
//         {
//             printf( "%s", args.help( "My App", fs::exePath(), myAppVersion() );
//             return 0;
//         }
//     
//         if ( args.have( "--one" ) )
//             doThatOneThing();
//     
//         if ( args.value( "--two", "no" ).compareNoCase( "yes" ) == 0 )
//             doThatOtherThing();
//     
//         return 0;
//     }
//
// The help() method will print a report of all the defined switches, their default values, and
// the help strings associated with them.
//
class DL_API Args : public RefersTo<IRefCounted>
{
public:
    Args( StringVector argsIn );
    Args( int argcIn, char** argvIn );
#if DL_IS_WIN32
    Args( int argcIn, LPWSTR* argvIn );
#endif

    // The original data.
    //
    int                     argc() const;
    const char**            argv() const;

    // Our versions of the data.
    //
    const StringVector      sArgs() const;

    // These are predefined -i/--input and -o/--output switches, used for input & output files.
    //
    dl::Path                iPath() const;
    dl::Path                oPath() const;

    // Also predefined are -h/--help and -v/--version.
    //
    Bool                    versionReqested() const;
    Bool                    helpRequested() const;

    // Prints a string with all the currently defined switches.
    //
    String                  usage() const;

    // Prints out all of the switches, with their values and helps, using the given exe path,
    // version, most important args, an example, and company copyright.
    //
    String                  help( String app, String exe, String version, String mainArgs = "",
                                  String example = "", String copyright = "") const;

    // Add a new argument.
    //
    void                    add( String shortForm, String longForm, String value, String info );
    
    // Find out if we have a particular argument.
    //
    Bool                    have( String arg ) const;

    // Get the value of an argument that is not a unary switch, meaning one that takes a value
    // after the colon, such as --input:my/file/path. This returns the value for the last of
    // potentially multiple copies of this argument.
    //
    String                  value( String id, String def = String() ) const;

    // Get all potential values for a given argument ID.
    //
    StringVector            values( String id ) const;

    // A collection of unknown arguments. ok() just reports whether unknown() is empty.
    //
    StringVector            unknown() const;
    Bool                    ok() const;

    // Print a report about arguments received and unknown arguments.
    //
    void                    report() const;

    // Get a string version of the raw arguments.
    //
    String                  rawArguments() const;
};

} // dl

#endif // DL_CORE_ARGS_H
