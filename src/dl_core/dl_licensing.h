// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_LICENSING_H
#define DL_CORE_LICENSING_H

#include "dl_file.h"
#include "dl_path.h"

// Licensing functions. Though these are in the dl_core library, they refer explicitly to Bella
// things, and are therefore not currently useful for much else.

namespace dl {
namespace licensing {

// We support full licenses, and evaluation licenses (e.g. for NFR, special testing, etc).
//
enum LicenseType : UInt8
{
    LicenseType_Full        = 0,
    LicenseType_Eval        = 1,
};

// Struct for working with licenses. Default type to full, to err on the side of the user. Version
// refers to license version, not Bella or any other version.
//
struct DL_API Info
{
    UInt8       version     = 0;
    String      path;
    String      owner;
    String      vendor;     
    UInt8       type        = LicenseType_Full;
    UInt64      expires     = 0;
    UInt16      seats       = 0;
    UInt16      nodes       = 0;
    String      key;
    Bool        valid       = false;
    String      status;

    Bool        write( File file );
    String      toString() const;

    Bool        isFull() const;
    Bool        isEval() const;
    Bool        isExpired( UInt64 t = 0 ) const;
    Bool        isValidVersion() const;

    Bool        hasOwner() const;
    bool        hasVendor() const;

    String      typeToString() const;
    String      expiresToString() const;
};

// Get the path to the license file. This returns the value of BELLA_LICENSE_PATH if it points to
// an existing file, and otherwise tries to return $HOME/bella/bella.lic, creating $HOME/bella if
// createBellaDirIfNecessary is true.
//
DL_API String   licensePath( bool createBellaDirIfNecessary = false );

// Check the supplied info to determine whether its license key is valid. The path will be ignored,
// but all other fields (except status) must be filled out correctly, or the function will fail.
// This is mainly for internal use by the checkLicenseFile function. The Info is passed by ref, in
// order to make use of its status field (nothing else will be changed).
//
DL_API Bool     checkLicenseInfo( Info& info );

// Check whether the provided string contains a valid license.
//
DL_API Bool     checkLicenseData( String licData, Info& info );

// This function attempts to open the file at 'path' and determine whether it contains valid
// license info. If 'path' is left empty, it will check for a path given by a BELLA_LICENSE_PATH
// environment variable, and if that is not set, it will attempt to check $HOME/bella/bella.lic.
// If a license path was able to be found, it will be set in the LicenseInfo, whether the license
// was found to be valid, or not.
//
DL_API Bool     checkLicenseFile( String path, Info& info );

} // licensing
} // dl

#endif // DL_CORE_LICENSING_H
