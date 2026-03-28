// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_VERSION_H
#define DL_CORE_VERSION_H

#include "dl_string.h"

namespace dl {

// Rudimentary version struct, constructed from components or a string. We do not use the revision
// in our public software, as not all installer technologies deal with it gracefully. So we adopt
// the convention [year].[build number].[patch number], where patch number is rarely used, but if
// so is e.g. used for releasing a quick plugin fix prior to the next full build being released.
//
struct DL_API Version
{
    UInt major;
    UInt minor;
    UInt build;
    UInt revision;

    Version( UInt major = 0, UInt minor = 0, UInt build = 0, UInt revision = 0 );
    Version( String str );
    Version& operator=( String str );

    bool operator <( Version other ) const;
    bool operator >( Version other ) const;
    bool operator ==( Version other ) const;
    bool operator !=( Version other ) const;

    String toString( bool withRevision = true ) const;
};

} // dl

#endif // DL_CORE_VERSION_H
