// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_TIME_H
#define DL_CORE_TIME_H

#include "dl_defines.h"
#include "dl_string.h"
#include "dl_platform.h"

namespace dl {
namespace sys {

// Some basic time facilities. Internally, this uses the high frequency timers of the various
// platforms. A very easy usage is to create an instance on the stack, and then at later points
// to just call elapsedToString(), which internally uses String::formatMicroSeconds.
//
class DL_API Time : public RefersTo<RefCounted>
{
public:
    static constexpr const char* Fmt_Iso8601       = "%FT%T%z";
    static constexpr const char* Fmt_BellaDateTime = "%d-%m-%Y %I:%M:%S";

    Time();

    PlatformTime        value() const;
    String              format( const char* fmt = Fmt_BellaDateTime, Bool local = true );

    UInt                elapsed() const;
    String              elapsedToString() const;

    static Time         now();
    static UInt         year();
    static UInt         diff( Time t0, Time t1 );
    static String       diffToSTring( Time t0, Time t1 );
    static String       diffToString( UInt microseconds );
    static String       strftime( const char* fmt = Fmt_Iso8601, Bool local = true );
};

} // sys
} // dl

#endif // DL_CORE_TIME_H
