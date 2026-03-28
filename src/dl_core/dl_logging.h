// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_LOGGING_H
#define DL_CORE_LOGGING_H

#include "dl_string.h"

namespace dl {

enum LogType
{
    LogType_Info,
    LogType_Warning,
    LogType_Error,
    LogType_Custom,
    LogType_Progress
};

// Callback for logging functions.
//
typedef void( *pLogFunc )( void*, LogType, const char* );

// Subscribe to be called back for logging.
//
DL_API void     subscribeLog( void* ctx, pLogFunc func );
DL_API void     unsubscribeLog( void* ctx );

DL_API void     openLogFile( String path = "" );
DL_API void     closeLogFile();

// Call this when your console is ready to print messages.
//
DL_API void      flushStartupMessages();

// Call these at any time.
//
DL_API void     logMessage( LogType type, String msg );
DL_API void     logMessage( LogType type, const char* fmt, ... );
DL_API void     logMessagev( LogType type, const char* fmt, va_list ap );
DL_API void     logMessageTsv( LogType type, const char* fmt, va_list ap ); // Add timestamp

DL_API void     logInfo( const char* fmt, ... );
DL_API void     logWarning( const char* fmt, ... );
DL_API void     logError( const char* fmt, ... );
DL_API void     logCustom( const char* fmt, ... );

DL_API void     logInfoTs( const char* fmt, ... );
DL_API void     logWarningTs( const char* fmt, ... );
DL_API void     logErrorTs( const char* fmt, ... );
DL_API void     logCustomTs( const char* fmt, ... );

DL_API void     logNewLine();
DL_API void     logSeparator( );
DL_API void     logBanner( const char* fmt, ... );

// Rather than add another facility for progress messages, we use the same log function with type
// LogType_Progress, and a 0-1 progress value preceding the message, delimited by a pipe. Calling
// logProgressBegin(str) will yield a log event with "0|<str>" in the message, logProgressEnd()
// will yield one with "1|" and logProgress will yield "<floating point value>|<str>".
//
DL_API void     logProgressBegin( String msg = "" );
DL_API void     logProgressEnd();
DL_API void     logProgress( Real progress, const char* fmt, ... );

// Helper methods for parsing LogType_Progress messages. These split the message on its first pipe
// character and parse the left side as a double; logProgressValue will return -1.0 if this fails.
//
DL_API Bool     logIsProgressBegin( String msg );
DL_API Bool     logIsProgressEnd( String msg );
DL_API Real     logProgressValue( String msg );
DL_API String   logProgressText( String msg );

// LogProgress is a helper class to make using progress messages easier. You create one on the
// stack, providing the number of elements you will be notifying, optionally call start(), and then
// call step() in each iteration of your loop. An appropriate step size will be computed, such that
// calling step() may or may not actually result in a progress event.
// 
// It is usually unnecessary to provide a step value in step(i), but it may be needed if you are
// reporting progress in a multithreaded context where you must track which step you are on.
// 
// Setting minElems to a value greater than zero will prevent any notifications at all if the
// computed step size ends up being less than the minSteps value. Setting autoStart=true prevents
// the need to explicitly call start() prior to your loop. If you do not call end(), it will be
// called for you when the LogProgress is destroyed.
//
class DL_API LogProgress : public RefersTo<IRefCounted>
{
public:
    LogProgress( String info, Size nElems, Size minElems = 0, Bool autoStart = true );

    void        start();
    void        step( Int iStep = -1 );
    void        end();

    UInt        stepSize() const;
};

// Console color.
//
enum ConsoleColor
{
    ConsoleColor_Default,
    ConsoleColor_Red,
    ConsoleColor_Yellow,
    ConsoleColor_Cyan
};

DL_API void     setConsoleColor( ConsoleColor color );
DL_API void     resetConsoleColor();

#define DL_PRINT_COLOR( CONSOLE_COLOR, FMT, ... ) \
    dl::setConsoleColor( CONSOLE_COLOR ); \
    fprintf( stdout, FMT, __VA_ARGS__ ); \
    dl::resetConsoleColor();

#define DL_PRINT( FMT, ... ) \
    DL_PRINT_COLOR( dl::ConsoleColor_Default, FMT, __VA_ARGS__ )

#define DL_PRINT_RED( FMT, ... ) \
    DL_PRINT_COLOR( dl::ConsoleColor_Red, FMT, __VA_ARGS__ )

#define DL_PRINT_YELLOW( FMT, ... ) \
    DL_PRINT_COLOR( dl::ConsoleColor_Yellow, FMT, __VA_ARGS__ )

#define DL_PRINT_CYAN( FMT, ... ) \
    DL_PRINT_COLOR( dl::ConsoleColor_Cyan, FMT, __VA_ARGS__ )

} // dl

#endif // DL_CORE_LOGGING_H
