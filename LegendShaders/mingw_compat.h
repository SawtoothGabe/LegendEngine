#pragma once
#ifdef __MINGW32__

#include <guiddef.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cwchar>

#define _Maybenull_
#define fread_s(buf, bufsz, esz, cnt, f) fread(buf, esz, cnt, f)

inline errno_t _dupenv_s(char** buf, size_t* bufsz, const char* name)
{
    const char* val = getenv(name);
    if (!val) { *buf = NULL; if (bufsz) *bufsz = 0; return 0; }
    *buf = strdup(val);
    if (bufsz) *bufsz = strlen(val) + 1;
    return *buf ? 0 : ENOMEM;
}

// DXC Interfaces Forward Declarations
struct IDxcCompiler;
struct IDxcLibrary;
struct IDxcLinker;
struct IDxcBlob;
struct IDxcVersionInfo;
struct IDxcVersionInfo2;
struct IDxcVersionInfo3;
struct IDxcResult;
struct IDxcUtils;
struct IDxcIncludeHandler;

namespace Slang { template<typename T> struct ComPtr; }

// Helper macro to define UUID specializations for MinGW
// We use inline constexpr for IID_ constants to avoid multiple definition errors (C++17)
#define MINGW_DXC_UUID(name, g1, g2, g3, g4, g5, g6, g7, g8, g9, g10, g11) \
    __extension__ template<> struct __mingw_uuidof_s<name> { \
        static constexpr GUID __uuid_inst = { g1, g2, g3, { g4, g5, g6, g7, g8, g9, g10, g11 } }; \
    }; \
    __extension__ template<> inline const GUID& __mingw_uuidof<name>() { \
        return __mingw_uuidof_s<name>::__uuid_inst; \
    } \
    __extension__ template<> inline const GUID& __mingw_uuidof<name*>() { \
        return __mingw_uuidof_s<name>::__uuid_inst; \
    } \
    __extension__ template<> inline const GUID& __mingw_uuidof<Slang::ComPtr<name>>() { \
        return __mingw_uuidof<name>(); \
    } \
    __extension__ inline constexpr GUID IID_##name = { g1, g2, g3, { g4, g5, g6, g7, g8, g9, g10, g11 } };

MINGW_DXC_UUID(IDxcCompiler, 0x8c213b90, 0xb302, 0x47ee, 0xb2, 0x39, 0x20, 0xbb, 0x4a, 0x9d, 0x1a, 0x1e)
MINGW_DXC_UUID(IDxcLibrary, 0xeae52d42, 0x05e7, 0x4dd8, 0xbc, 0x18, 0x45, 0x56, 0x19, 0x56, 0x03, 0xc4)
MINGW_DXC_UUID(IDxcLinker, 0x3456a418, 0x806f, 0x4b4b, 0x89, 0x1a, 0xcc, 0xf0, 0x04, 0x8e, 0x98, 0x34)
MINGW_DXC_UUID(IDxcBlob, 0x8ba44848, 0x4d36, 0x49b0, 0xb0, 0x6d, 0x59, 0xfe, 0x91, 0x43, 0x54, 0x3d)
MINGW_DXC_UUID(IDxcVersionInfo, 0xb04f3fd3, 0x470a, 0x4a87, 0x9d, 0x29, 0xff, 0x40, 0x57, 0x48, 0xdf, 0xbb)
MINGW_DXC_UUID(IDxcVersionInfo2, 0xfb6b2c24, 0x985d, 0x4a9a, 0xa2, 0xd1, 0x0c, 0x5d, 0x12, 0xc0, 0x9c, 0x7b)
MINGW_DXC_UUID(IDxcVersionInfo3, 0x5e13e843, 0x9d25, 0x473c, 0x9a, 0xd2, 0x03, 0xb2, 0xd0, 0xb4, 0x4b, 0x1e)
MINGW_DXC_UUID(IDxcResult, 0x58346c10, 0xd0bc, 0x4434, 0x94, 0x04, 0x20, 0x4b, 0x90, 0xec, 0x7d, 0x9a)
MINGW_DXC_UUID(IDxcUtils, 0x603bba05, 0x056d, 0x4445, 0x91, 0xa3, 0xba, 0x0d, 0x4e, 0x08, 0x7c, 0x7b)
MINGW_DXC_UUID(IDxcIncludeHandler, 0x7f61fcad, 0x6e51, 0x4654, 0xb3, 0x61, 0x8c, 0x56, 0x87, 0x10, 0xd1, 0xfb)

// MinGW's crtexewin.o expects WinMain if subsystem:windows is used.
// Slang's main.cpp defines wmain, but MinGW might look for WinMain if linked with certain flags.
// To avoid multiple definitions, we only define it if SLANG_BOOTSTRAP is defined (for the failing target).
#if (defined(UNICODE) || defined(_UNICODE)) && defined(SLANG_BOOTSTRAP)
#include <windows.h>
#include <shellapi.h>
extern "C" int wmain(int argc, wchar_t** argv);
extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    int argc;
    wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    int result = wmain(argc, argv);
    LocalFree(argv);
    return result;
}
#endif

#endif
