// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_OS_H
#define DL_CORE_OS_H

#include "dl_math.h"
#include "dl_string.h"
#include "dl_hashmap.h"
#include "dl_platform.h"
#include <thread>

namespace dl {
namespace os {

// Macros for capturing OS errors on OS calls. The upshot here is that you will use this by putting
// an entire OS call inside the DL_OS_CALL macro, like so:
// 
//    DL_OS_CALL( DWORD len = GetEnvironmentVariableW( pName, NULL, 0 ) );
//    if ( !len )
//    {
//        auto code = currentErrorCode();
//        if ( code != ERROR_ENVVAR_NOT_FOUND && code != ERROR_SUCCESS )
//        {
//            auto msg = currentErrorMessage();
//            if ( msg.isEmpty() )
//                msg = "(no specific error reported)";
//    
//            logError( "GetEnvironmentVariableW (first) call failed: %s", msg.buf() );
//        }
//    }
// 
// Internally, this is using GetLastError, errno, and/or dlerror() to find out what the OS thinks
// and formatting that for you to retrieve via the currentError functions below.
//
#if DL_IS_GCC || DL_IS_CLANG
    #define DL_CALLER_NS dl::String( __func__ )
    #define DL_CALLER    dl::String( __func__ ).fromLast( ':', false )
#else
    #define DL_CALLER_NS dl::String( __FUNCTION__ )
    #define DL_CALLER    dl::String( __FUNCTION__ ).fromLast( ':', false )
#endif

#define DL_OS_CALL(oscall) \
    dl::os::clearCurrentError(); oscall; dl::os::captureCurrentError(DL_CALLER);

struct DL_API ErrorInfo
{
    Int code;
    String caller;
    String message;
};

DL_API ErrorInfo    currentError();
DL_API Int          currentErrorCode();
DL_API String       currentErrorMessage();
DL_API String       currentErrorCaller();

// Probably never need to use these, as they exist mainly to be used by dl_core itself.
//
DL_API void         setCurrentError( Int code, String caller, String msg );
DL_API void         captureCurrentError( String caller = String() );
DL_API void         clearCurrentError();

// Not sure if this works, I have no speakers on my dev machine.
//
DL_API void         beep();

// Message box helpers. These use tiny file dialog and are not yet well tested.
//
DL_API void         infoBox( String title, String message );
DL_API void         warningBox( String title, String message );
DL_API void         errorBox( String title, String message );

// More complex OS dialogs. Also using tiny file dialog, and not yet well tested.
//
enum DialogType
{
    DialogType_OkCancel,
    DialogType_YesNo,
    DialogType_YesNoCancel
};

DL_API Bool         questionBox( String title, String message, DialogType dialogType = DialogType_YesNo, Bool defaultResponse = false, Bool* cancelled = nullptr );
DL_API Bool         okCancelBox( String title, String message, Bool defaultResponse = false );
DL_API Bool         yesNoBox( String title, String message, Bool defaultResponse = false );
DL_API Bool         yesNoCancelBox( String title, String message, Bool defaultResponse = false, Bool* cancelled = nullptr );
DL_API String       inputBox( String prompt, String message, String defaultInput = "" );
DL_API Rgb          chooseColor( String title = "Choose Color", Rgb defaultRgb = Rgb::make( 0.5, 0.5, 0.5 ) );

// Get current locale ID, e.g. "en", "es", etc. Currently reports "en" in all cases.
//
DL_API String       currentLocale();

// Get a localized item from a map<String, String>. Currently always returns the "en" lang item
// and exists to allow writing code that will work properly once we finish the system.
//
DL_API String       localizedItem( const ds::HashMap<String, String>& map, String lang = "auto" );

// Get the computer name (uses GetComputerNameEx on win32, gethostname on posix).
//
DL_API String       machineName();

// Env vars.
//
DL_API StringVector env();
DL_API String       getEnv( String name );
DL_API Int          setEnv( String name, String value, Bool overwrite = true );
DL_API Int          putEnv( String value );
DL_API Int          unsetEnv( String name );
DL_API String       getPath();
DL_API Int          setPath( String value );

// Library functions.
//
DL_API DL_HMODULE   dlOpen( String path, StringVector searchDirs = StringVector() );
DL_API bool         dlClose( DL_HMODULE mod );
DL_API DL_HSYMBOL   dlSym( String symbol, DL_HMODULE mod = DL_DEFMODULE );

// Start an application with arguments, optionally waiting for it to close. Note that the specified
// environment will not be used, if the started application requires elevation on win32. On macos,
// this will properly start a native arm64 application even when called from a rosetta process.
//
DL_API Int          execV( String exe, StringVector args, Bool wait = false, StringVector env = os::env() );

// Open a file in the default application.
//
DL_API Bool         openPath( String path, String& error );

// See if we are running under Rosetta.
//
DL_API Bool         runningOnRosetta();

// Prevent MacOS app napping.
//
DL_API void         disableAppNap();
DL_API void         enableAppNap();

// These are used by gui & cli to indicate to solvers that they are free to use high priority. In
// plugins we do not call enableHighPriority() to avoid interfering the host applications. These
// functions only set a flag and do not actually set priority.
//
DL_API void         enableHighPriority();
DL_API void         disableHighPriority();
DL_API bool         useHighPriority();

// Processor groups and thread count. This is needed on win32 because c++ hardware_concurrency()
// does not work properly for many-core systems, where Windows may assign processors to processor
// groups, with hardware_concurrency() returning the thread count for only one group.
//
DL_API UInt32       numProcessorGroups();
DL_API UInt32       numLogicalProcessors( Int32 processorGroup = -1 );

// Thread affinity. The shouldSetThreadAffinity function returns true if hardware_concurrency()
// reports less than numLogicalProcessors() and the proposed thread count is greater than that
// reported by hardware_concurrency(), in which case you must set affinity on your threads to
// get them running on all cores of all groups.
//
DL_API bool         shouldSetThreadAffinity( UInt32 numThreadsToBeUsed );
DL_API void         setThreadAffinity( std::thread& t, UInt32 affinity );

} // os
} // dl

#endif // DL_CORE_OS_H

