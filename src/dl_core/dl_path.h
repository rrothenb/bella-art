// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_PATH_H
#define DL_CORE_PATH_H

#include "dl_string.h"
#include "dl_os.h"

namespace dl {

// Path is a class for dealing with file system paths cross platform. It uses a similar approach to
// python, though you can choose to create an instance and call methods on it, or use the matching
// static methods. It is not super comprehensive yet, but it has been enough so far.

enum PathType
{
    PathType_Invalid,
    PathType_File,
    PathType_Dir
};

class DL_API Path : public RefersTo<IRefCounted>
{
public:
    Path();
    Path( String path, bool checkDisk = true );         // If checkDisk==true, the OS will be queried.

    PathType        type() const;                       // This is PathType_Invalid, unless statOK().
    String          input() const;                      // A copy of the specified path, as requested.
    String          normalized() const;                 // A cleaned-up version of the path with native separators.
    String          canonical() const;                  // The canonical representation of this path. May return empty if the file does not exist.

    Bool            isEmpty() const;
    Bool            isUnc() const;                      // True if the path begins with double slashes.
    Bool            isRoot() const;                     // Whether the path points to a root directory.
    Bool            hasRoot() const;                    // Whether the path has a root directory.
    Bool            isAbs() const;                      // Return whether this is an absolute path.
    Bool            isRel() const;                      // Just the opposite of isAbs().

    String          toPosix() const;                    // Convert to POSIX format.
    String          toWin32() const;                    // Convert to Windows format.

    StringVector    components() const;                 // A list of the path components, separated.

    String          server() const;                     // The first component after double slashes.
    String          share() const;                      // The second component after double slashes.

    String          root() const;                       // Get the root part of the path.
    String          dir() const;                        // The directory, sans file name. Best guess when !statOK().
    String          file( bool withExt = true ) const;  // The file name, with or without extension. Best guess when !statOK().
    String          ext( bool withDot = false ) const;  // The extension, with or without leading dot. Best guess when !statOK().
    String          stripExt() const;                   // Get the path without extension.

    Bool            statOK() const;                     // This indicates success of OS query, when requested.
    UInt64          size() const;                       // File size from OS in KB, if statOK().
    UInt64          mtime() const;                      // File modified (unix) time, if statOK().
    UInt64          ctime() const;                      // File created (unix) time, if statOK().
    UInt64          atime() const;                      // File accessed (unix) time, if statOK().

    Bool            isDir() const;                      // True if statOK() and type is dir, or if dir is not empty.
    Bool            isFile() const;                     // True if statOK() and type is file, or if file & ext not empty.
    Bool            exists() const;                     // True if and only if statOK() && type is dir or file.
    Bool            dirExists() const;                  // True if dir() exists.
    Bool            hasExt( String ext ) const;         // True if extension matches ext (with/without dot, case-insensitive).

    os::ErrorInfo   info() const;                       // Holds error info encountered while checking the file.

    // Static helpers.
    //
    static String   normalize( String path );
    static String   canonical( String path );

    static String   root( String path );
    static String   dir( String path, Bool checkDisk = true );
    static String   file( String path, Bool checkDisk = true, Bool withExt = true );
    static String   ext( String path, Bool withDot = false );

    static String   stripExt( String path );
    static String   changeExt( String path, String ext );

    static String   join( StringVector components );
    template <typename... Components>
    static String   join( Components... components ) { return join( { components... } ); }

    static Bool     isUnc( String path );
    static Bool     isRoot( String path );
    static Bool     hasRoot( String path );
    static Bool     isAbs( String path );
    static Bool     isRel( String path );

    static Bool     isFile( String path, Bool checkDisk = true );
    static Bool     isDir( String path, Bool checkDisk = true );
    static Bool     exists( String path );

    static String   quote( String path, Bool force = false );
    static String   unquote( String path );

    static UInt64   size( String path );
    static UInt64   mtime( String path );
    static UInt64   ctime( String path );
    static UInt64   atime( String path );

};

} // dl

#endif // DL_CORE_PATH_H

