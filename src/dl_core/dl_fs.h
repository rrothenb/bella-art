// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_FS_H
#define DL_CORE_FS_H

#include "dl_string.h"
#include "dl_path.h"
#include <cstdio>

// General file system functions and helpers.
//
namespace dl {
namespace fs {

 // The platform path sepeartor and quoting characters.
 //
#if DL_IS_WIN32
    constexpr const char* sep  = "\\";
    constexpr const char* quot = "\"";
#else
    constexpr const char* sep  = "/";
    constexpr const char* quot = "'";
#endif

// FILE wrappers, using our String to support utf8 filenames.
//
DL_API FILE*        Fopen( String path, String mode );
DL_API FILE*        Freopen( String path, String mode, FILE* oldStream );
DL_API int          Fclose( FILE* stream );
DL_API bool         Fremove( String path );
DL_API int          Fseek( FILE* stream, int64_t offset, int origin );
DL_API int64_t      Ftell( FILE* stream );
DL_API size_t       Fread( void* buffer, size_t elementSize, size_t elementCount, FILE* stream );
DL_API size_t       Fwrite( const void* buffer, size_t elementSize, size_t elementCount, FILE* stream );
DL_API int          Fflush( FILE* stream );
DL_API int          Frename( String src, String dst );

// File system functions.
//
DL_API bool         mkDir( String dir );
DL_API bool         rmDir( String dir );

DL_API bool         rmFile( String path );
DL_API bool         copyFile( String src, String dst );
DL_API bool         moveFile( String src, String dst );

// Dir & file existence.
//
DL_API bool         isDir( String path );
DL_API bool         isFile( String path );
DL_API bool         exists( String path );

// List files & dirs.
//
DL_API StringVector listDirs( String dir );
DL_API StringVector listFiles( String dir, StringVector types = {} );

// ZIP functions.
//
struct DL_API ZipProgress
{
    String  info;
    Real    progress;
};

DL_API StringVector readZip( String zipPath, String outDir, std::function<void( ZipProgress )> callback );
DL_API bool         writeZip( String zipPath, StringVector inputFiles, std::function<void( ZipProgress )> callback );

// BSZ functions.
//
struct DL_API BszProgress
{
    String  info;
    Real    progress;
};

DL_API bool         readBsz( String bszPath, std::function<void( BszProgress )> callback );
DL_API bool         writeBsz( String bszPath, StringVector inputFiles, std::function<void( BszProgress )> callback );

// Common paths.
//
DL_API String       exePath(); // exe path on win32/linux, <app>.app/Contents/MacOS/<app> on macos.
DL_API String       exeDir();  // exe dir on win32/linux, <app>.app/Contents/MacOS on macos.

// MacOS-related paths.
//
DL_API String       appDir(); // exeDir() on win32/linux, dir containing <app>.app on macos.
DL_API String       resDir(); // exeDir() on win32/linux, <app>.app/Contents/Resources on macos.

// Well-known paths. These return false on failure. By default, the Bella "user dir" is $HOME/bella
// but it can also be set using the BELLA_USER_DIR env var.
//
DL_API bool         getCurrentDir( String& out );
DL_API bool         getHomeDir( String& out );
DL_API bool         getTempDir( String& out );

DL_API bool         getBellaUserDir( String& out, bool create = true );
DL_API bool         getBellaConfigDir( String& out, bool create = true );
DL_API bool         getBellaOutputDir( String& out, bool create = true );
DL_API bool         getBellaPreviewDir( String& out, bool create = true );

// Well-known paths. These return currentDir() on failure.
//
DL_API String       currentDir();
DL_API String       homeDir();
DL_API String       tempDir();

DL_API String       bellaUserDir( bool create = true );
DL_API String       bellaConfigDir( bool create = true );
DL_API String       bellaOutputDir( bool create = true );
DL_API String       bellaPreviewDir( bool create = true );
DL_API String       bellaNodeHelpPath( bool asUrl );

// Given a proposed path, search for the file in the given searchDirs. If path ends with ".*" then
// we will use the allowedExtensions to resolve the path to the first matching file name found in
// the searchDirs. Else, if the path as given exists, the canonical path form will be returned, and
// if not, then the file name will be searched in the searchDirs, which are also allowed to be
// relative dirs.
//
DL_API String resolvePath(
    String path,
    StringVector searchDirs = {},
    StringVector allowedExtensions = {}
);

// File and folder browsing. Uses tiny file dialog, not well tested.
//
DL_API String browseFile(
    String title = "Open File",
    String initialFile = "",
    String description = "All Files",
    StringVector extensions = StringVector()
);

DL_API StringVector browseFiles(
    String title = "Open Files",
    String initialDir = "",
    String description = "All Files",
    StringVector extensions = StringVector()
);

DL_API String browseDir(
    String title = "Choose Directory",
    String initialDir = ""
);

DL_API String saveFile(
    String title = "Save File",
    String initialFile = "",
    String description = "All Files",
    StringVector extensions = StringVector()
);

} // fs
} // dl

#endif // DL_CORE_FS_H

