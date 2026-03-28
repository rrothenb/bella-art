// Copyright 2019 Diffuse Logic, all rights reserved.

// Use this file to implement your main function, rather than implementing the standard one. This
// is to ensure that you are able to get unicode command-line arguments, which by default, is not
// the case on windows. The general way to use it is as such:
//
//   #include "dl_core/dl_main.inl"
//   int DL_main( dl::Args args )
//   {
//       //
//       // Do what you like with the arguments, which are valid UTF-encoded strings.
//       //
//
//       return 0;
//   }
//

#include "dl_args.h"

// You must define the following function somewhere; it will be called as your main function.
//
int DL_main( dl::Args& args );

// Need to declare this, because on windows, we define both main and WinMain, letting the compiler
// decide which to use, based on the /subsystem flag.
//
int main( int argc, char** argv );

// Define WinMain; if it is used by the compiler, we'll defer to the unused main(int,char**).
//
#if defined(_WIN32)
#include <shellapi.h>
int CALLBACK WinMain( HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/ )
{
    return main( 0, nullptr );
}
#endif

// Here, we define main, which is called either by the compiler, or our WinMain function.
//
int main( int argc, char** argv )
{
#if defined(_WIN32)
    DL_UNUSED( argc );
    DL_UNUSED( argv );
    int argcw = 0;
    LPWSTR* argvw = CommandLineToArgvW( GetCommandLineW(), &argcw );
    if ( !argvw )
    {
        // TODO: report results of GetLastError.
        return 1;
    }

    dl::Args args( argcw, argvw );
    LocalFree( argvw );
    SetConsoleOutputCP( CP_UTF8 );
#else
    dl::Args args( argc, argv );
#endif
    return DL_main( args );
}


